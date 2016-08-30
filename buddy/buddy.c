#include "buddy.h"

#ifndef	NULL
#	define	NULL	0
#endif

/* free_area 구조체, page 구조체 자료 구조를 위한 공간을 위해 */
#define	STRUCT_DATA	(1*1024*1024)

/* 메모리 할당 함수, size에 대해 버디에서 페이지를 할당
 * mmap 인터페이스를 통해 메모리를 할당한다
 */
 void ready_for_memory(void)
 {
 	 real_memory = mmap(0, mem_size + STRUCT_DATA, PROT_READ | PROT_WRITE |
		PROT_EXEC, MAP_ANO | MAP_PRIVATE, -1, 0);
	 printf("memory is ready, address is %lx\n", (unsigned long)real_memory);
 }

 /* 버디 시스템을 위한 특정 자료구조의 공간 할당을 위한 함수 */
 void * get_address_map(int size)
 {
 	 char * addr;
 	 addr = (char *)((char*)real_memory + mem_offset);
 	 memset(addr, (int)0, size);
 	 mem_offset += size;
 	 return addr;
 }

 /* 실제 물리적인 메모리는 page 크기만큼 분활하여 사용하는데 각각의 물리적인
  * page는 page구조체 하나로 표현한다. 그래서 모둔 가용한 메모리에 대해 page 구
  * 조체를 할당 해주어야 한다
  */
void mapping_page(mem_map_t * mem_map)
{
	unsigned long temp = mem_offset;
	while(mem_offset <= mem_size + STRUCT_DATA) {
		mem_map->addr = (unsigned long *)((char*)real_memory +
			mem_offset);
		mem_offset += PAGE_SIZE;
		mem_map++;
	}
	mem_offset = temp;
}

/* 요청된 크기의 order를 구하여 반환 */
int cal_cur_order(unsigned long mem)
{
	int i = BUDDY_MAX_ORDER - 1 ;
	while(i >= 0) {
		if((mem)a == (PAGE_SIZE << i))
			return i;
		i--;
	}

	if(mem > (PAGE_SIZE << (BUDDY_MAX_ORDER -1)))
		return BUDDY_MAX_ORDER;
	return 0;
}

/* 버디 시스템의 비트맵의 비트를 변경 및 확인 시켜주는 함수
 * 특정 위치를  bit값을 확인
 */
static __inline__ int constant_test_bit(int nr, const volatile void * addr)
{
	return ((1UL << (nr & 31)) &
		(((const volatile unsigned int *) addr)[nr >> 5])) != 0;
}

/* 특정 위치의 bit값을 변경, 0이면 1, 1이면 0 */
static __inline__ void __change_bit(int nr, volatile void * addr) {
	if(constant_test_bit(nr, addr) == 1) {
		(((volatile unsigned int *) addr)[nr >> 5])
			&= (0xffffff ^ (1UL << (nr & 31)));
	} else {
		(((volatile unsigned int *) addr)[nr >> 5])
			&= (1UL << (nr & 31));
	}
}


