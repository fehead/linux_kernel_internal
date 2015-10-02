#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <malloc.h>
#include <memory.h>
#include <sys/time.h>
#include "schedule.h"

// task switching시 저장되어야 하는 정보
struct frame {
	unsigned long flags;
	unsigned long ebp;
	unsigned long edi;
	unsigned long esi;
	unsigned long edx;
	unsigned long ecx;
	unsigned long ebx;
	unsigned long eax;
	unsigned long retaddr;
	unsigned long retaddr2;
	unsigned long data;
};

typedef struct sch_handle_tag {
	int child_task;

	TaskInfo running_task;
	TaskInfo root_task;
} SchHandle;

// global schedule handler
SchHandle gh_sch;

// task data struct
TaskInfo task_get_runningtask();
void task_insert(TaskInfo taskinfo);
void task_delete(TaskInfo taskinfo);
void task_next();
void scheduler();
void parent_task();

// thread_create : task를 생성하는 함수로 taskinfo 구조체를 할당하고 구성한다
TaskInfo thread_create(TaskFunc callback, void *context)
{
	TaskInfo taskinfo;
	// task를 위한 공간 할당
	taskinfo = malloc(sizeof(*taskinfo));
	memset(taskinfo, 0x00, sizeof(*taskinfo));

	{
		struct frame *f = (struct frame *)&taskinfo->stack[
			THREAD_STACKSIZE - sizeof(struct frame)/4];
		// taskinfo로할당된 공간 중 stack부분 뒤쪽에 frame을 위한 공간으
		// 로 할당 이어 task가 수행되면서 stack공간을 활용
		int i;
		for(i = 0; i < THREAD_STACKSIZE; ++i) // stack overfllow check
			taskinfo->stack[i] = i;
		memset(f, 0, sizeof(struct frame));
		f->retaddr = (unsigned long)callback;
		f->retaddr2 = (unsigned long)thread_kill;
		f->data = (unsigned long)context;
		taskinfo->sp = (unsigned long)&f->flags;
		f->ebp = (unsigned long)&f->eax;
	}
	// task 생성에 따라 gh_sch에 child task가 늘었을을 표시
	gh_sch.child_task++;
	// gh_sch.child_task 값으로 task_id 할당
	taskinfo->task_id = gh_sch.child_task;
	// task 생성시 TASK_READY로 상태를 설정함
	taskinfo->status = TASK_READY;
	// taskinfo구조체들의 linkedlist에 새 thread의 taskinfo 구조체를 삽입
	task_insert(taskinfo);

	return taskinfo;
}

/* thread_init : 초기화 함수로 main 함수가 처음에 호출하여
 * global scheduler handeler를 초기화 하고, parent_task를 생성한다
 */
void thread_init()
{
	gh_sch.root_task = NULL;
	gh_sch.running_task = NULL;

	gh_sch.child_task = 0;
	
	thread_create(parent_task, NULL);
}

