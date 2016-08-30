#ifndef _BUDDY_H_
#define	_BUDDY_H_

#include "list.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PAGE_SHIFT	12
#define	PAGE_SIZE	(1UL << PAGE_SHIFT)	/* 한개의 페이지 크기 4k */
#define	PAGE_MASK	(~(PAGE_SIZE-1))	/* 4k alignment */

#define	PAGE_ALIGN(addr)	(((addr)+PAGE_SIZE-1) & PAGE_MASK)
#define	LONG_ALIGN(x)	(((x)+(sizeof(long))-1)&~((sizeof(long))-1))
#define	BUDDY_MAX_ORDER	10	/* 버디의 최상위 order는 512개 page들을 관리 */
#define	TOTAL_PAGES(size)	(size >> PAGE_SHIFT)

/* GET_NR_PAGE(addr)	addr에 해당하는 페이지 번호를 반환 */
#define	GET_NR_PAGE(addr)	((addr) - ((unsigned long)real_memory + \
					mem_offset) >> (PAGE_SHIFT)

/* page의 물리 주소 */
#define	page_address(page)	((page)->addr)
unsigned int	mem_size;	/* 시뮬레이터의 메모리 크기 */
unsigned long	mem_offset;

void *	real_memory;		/* mmap 인터페이스로 받아온 메모리 영역 */
unsigned long	free_pages;

/* free page들을 관리하기 위한 구조체 */
typedef struct free_area_struct {
	struct list_head	free_list;
	unsigned long		* map;
} free_area_t;
free_area_t free_area[BUDDY_MAX_ORDER];	/* free page는 10개의 order로 구성 */

typedef struct page {
	struct list_head list;	/* free_area_t 구조체와 연결하기 위한 리스트 */
	unsigned long	flags;	/* noghting to do! */
	void		* addr;	/* page의 실제 메모리 주소 */
	int		order;
} mem_map_t;
mem_map_t	* lmem_map;

/* struct zone
 * 실제 리눅스의 메모리 할당, 해제 함수의 인터 페이스와 동일한 인자를 받도록
 * 만들어주기 위해 생성
 */
typedef struct zonelist_struct {
	int	i;	/* zone member */
} zonelist_t;

typedef struct zone_struct {
	int	j;
} zone_t;

/* 함수 윈형 선언부 */
void	init_memory(void);
void	input_size(void);
void	free_memory(void);
void	init_buddy(void);

void	alloc_bitmap(unsigned long *, unsigned long);
void	ready_for_memory(void);
void *	get_address_map(int);
void	mapping_page(mem_map_t *);

#define	ADDR(*(volatile long*)addr)
unsigned long __get_free_pages(unsigned int, unsigned int);
struct page *	alloc_pages(unsigned int, unsigned int);
struct page *	__alloc_pages(unsigned int, unsigned int, zonelist_t *);

struct page *	expand(zone_t *, struct page *, unsigned long, int, int,
			free_area_t *);

void	_free_pages(void *ptr);
void	__free_pages(struct page *, unsigned int);
void	__free_pages_ok(struct page *, unsigned int);

int	cal_cur_order(unsigned long);
void	_show_free_order_list(int);
void	_show_free_list_map(int);

#endif /* _BUDDY_H_ */
