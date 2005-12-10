#include <stdlib.h>					   /* for free() */
#include <stdio.h>					   /* for fprintf() */
#include <string.h>					   /* for memset() */

#include "list.h"

/* structures for constructing the list */
struct _listheader
{
	struct _listheader *next;
	void *payload;
	struct _listheader *pool_next;
};
typedef struct _listheader *ListHeader;

struct _list
{
	ListHeader head;
	ListHeader tail;
	unsigned long len;
	struct _list *pool_next;
	struct _list *pool_prev;
};

struct _list_iterator
{
	struct _listheader *cur;
	List l;
};

struct cleanup
{
	struct cleanup *next;
	void *data;
};

static List lPool = NULL;
static List lPoolUsed = NULL;
static unsigned long long lPoolSize = 0;	/* for exponential growth */
static struct cleanup *lCleanupPool = NULL;
static inline List get_l(void);
static void fill_l_pool(void);

static ListHeader lhPool = NULL;
static unsigned long long lhPoolSize = 0;	/* for exponential growth */
static struct cleanup *lhCleanupPool = NULL;
static inline ListHeader get_lh(void);
static void fill_lh_pool(void);
static void poolReturn(ListHeader lh);

/* This sets up the memory pool(s) */
void lists_init()
{									   /*{{{ */
	fill_lh_pool();
	fill_l_pool();
}									   /*}}} */

/* This clean up the memory pool(s) */
void lists_cleanup()
{									   /*{{{ */
	struct cleanup *freec;
	ListHeader freeme;
	List freeme2;

	while (lhCleanupPool != NULL) {
		freec = lhCleanupPool;
		freeme = freec->data;
		lhCleanupPool = lhCleanupPool->next;
		free(freeme);
		free(freec);
	}
	while (lCleanupPool != NULL) {
		freec = lCleanupPool;
		freeme2 = freec->data;
		lCleanupPool = lCleanupPool->next;
		free(freeme2);
		free(freec);
	}
}									   /*}}} */

/* This returns a struct to the pool */
void poolReturn(ListHeader lh)
{									   /*{{{ */
	memset(lh, 0, sizeof(struct _listheader));
	lh->pool_next = lhPool;
	lhPool = lh;
}									   /*}}} */

/* This fills the list header memory pool with 1024 items (or doubles the pool
 * size) */
static void fill_lh_pool()
{									   /*{{{ */
	ListHeader tlist;
	ListHeader temp;
	struct cleanup *ch;
	size_t newobjcount;
	size_t i;

	newobjcount = (lhPoolSize == 0) ? 1024 : lhPoolSize;
	tlist = calloc(newobjcount, sizeof(struct _listheader));
	ch = calloc(1, sizeof(struct cleanup));
	ch->data = tlist;
	ch->next = lhCleanupPool;
	lhCleanupPool = ch;
	for (i = 0; i < newobjcount; ++i) {
		temp = &(tlist[i]);
		temp->pool_next = lhPool;
		lhPool = temp;
	}
	lhPoolSize += newobjcount;
}									   /*}}} */

/* This fills the list item memory pool with 2 items (or doubles the pool
 * size) */
static void fill_l_pool()
{									   /*{{{ */
	size_t i, newobjcount;
	List temp;
	List tlist;
	struct cleanup *ch;

	newobjcount = (lPoolSize == 0) ? 2 : lPoolSize;
	tlist = calloc(newobjcount, sizeof(struct _list));
	ch = calloc(1, sizeof(struct cleanup));
	ch->data = tlist;
	ch->next = lCleanupPool;
	lCleanupPool = ch;
	for (i = 0; i < newobjcount; ++i) {
		temp = &(tlist[i]);
		temp->pool_next = lPool;
		lPool = temp;
	}
	lPoolSize += newobjcount;
}									   /*}}} */

/* This frees up the memory allocated by the list */
void freeList(List * lst)
{									   /*{{{ */
	List thelist;

	if (!lst) {
		return;
	}

	thelist = *lst;
	if (!thelist) {
		return;
	}

	while (thelist->len > 0 && thelist->head != NULL) {
		ListHeader h = thelist->head;

		thelist->head = h->next;
		thelist->len--;
		poolReturn(h);
	}
	if (lPoolUsed == thelist) {
		lPoolUsed = thelist->pool_next;
	}
	if (thelist->pool_next) {
		thelist->pool_next->pool_prev = thelist->pool_prev;
	}
	if (thelist->pool_prev) {
		thelist->pool_prev->pool_next = thelist->pool_next;
	}
	thelist->pool_next = lPool;
	thelist->pool_prev = NULL;
	lPool = thelist;
	*lst = NULL;
}									   /*}}} */

/* this fetches a new list header from the pool */
static inline ListHeader get_lh()
{									   /*{{{ */
	ListHeader lh;

	if (NULL == lhPool) {
		fill_lh_pool();
	}
	lh = lhPool;
	lhPool = lh->pool_next;
	lh->pool_next = NULL;
	return lh;
}									   /*}}} */

/* this fetches a new list from the pool */
static inline List get_l()
{									   /*{{{ */
	List l;

	if (NULL == lPool) {
		fill_l_pool();
	}
	l = lPool;
	lPool = l->pool_next;
	l->pool_next = lPoolUsed;
	if (lPoolUsed) {
		lPoolUsed->pool_prev = l;
	}
	lPoolUsed = l;
	lPoolUsed->pool_prev = NULL;
	return l;
}									   /*}}} */

/* This adds "addme" to the end of the list */
void addToList(List * lst, void *addme)
{									   /*{{{ */
	List list;
	ListHeader pl;

	pl = get_lh();
	pl->payload = addme;
	pl->next = NULL;
	if (!lst) {
		fprintf(stderr, "null list passed\n");
		exit(EXIT_FAILURE);
	}
	list = *lst;
	if (!list) {
		list = *lst = get_l();
		list->tail = list->head = pl;
		list->len = 1;
	} else {
		if (list->tail) {
			list->tail->next = pl;
			list->tail = pl;
			list->len++;
		} else {
			list->tail = list->head = pl;
			list->len = 1;
		}
	}
}									   /*}}} */

/* This adds "addme" to the head of the list */
void addToListHead(List * lst, void *addme)
{									   /*{{{ */
	List list;
	ListHeader pl;

	pl = get_lh();
	pl->payload = addme;
	pl->next = NULL;
	if (!lst) {
		fprintf(stderr, "null list passed\n");
		exit(EXIT_FAILURE);
	}
	list = *lst;
	if (!list) {
		list = *lst = get_l();
		list->tail = list->head = pl;
		list->len = 1;
	} else {
		if (list->head) {
			pl->next = list->head;
			list->head = pl;
			list->len++;
		} else {
			list->tail = list->head = pl;
			list->len = 1;
		}
	}
}									   /*}}} */

/* This removes the front of the list from the list and returns it */
void *getHeadOfList(List list)
{									   /*{{{ */
	void *payload;
	ListHeader pl;

	if (!list || !list->head) {
		if (!list) {
			fprintf(stderr, "getHeadOfList: null List passed\n");
		}
		return NULL;
	}
	payload = list->head->payload;
	pl = list->head;
	list->head = list->head->next;
	poolReturn(pl);
	list->len--;
	if (list->len == 0) {
		list->head = list->tail = NULL;
	}
	return payload;
}									   /*}}} */

/* This returns the n'th element of the list, as if the list was an array */
void *peekListElement(List list, size_t n)
{									   /*{{{ */
	ListHeader pl;
	size_t counter;

	if (!list || !list->head || list->len <= n) {
		if (!list) {
			fprintf(stderr, "peekListElement: null List passed\n");
		}
		return NULL;
	}
	pl = list->head;
	for (counter = 1; counter <= n; counter++) {
		if (pl->next == NULL) {
			return NULL;
		}
		pl = pl->next;
	}
	return pl->payload;
}									   /*}}} */

/* This returns the head of the list */
inline void *peekAheadInList(List list)
{									   /*{{{ */
	if (list && list->head) {
		return list->head->payload;
	}
	return NULL;
}									   /*}}} */

/* This tells you how many items there are in the list */
inline unsigned long listLen(List list)
{									   /*{{{ */
	if (list) {
		return list->len;
	} else {
		return 0;
	}
}									   /*}}} */

/* This returns a list iterator to a list */
ListIterator getListIterator(List list)
{
	ListIterator li = malloc(sizeof(ListIterator));

	li->cur = list->head;
	li->l = list;
	return li;
}

/* This returns the value of the current element in the list Iterator */
void * currentListElement(ListIterator li)
{
	if (!li || !(li->cur)) {
		return NULL;
	}
	return li->cur->payload;
}

/* This iterates a list iterator */
void *nextListElement(ListIterator li)
{
	void * payload;

	if (!li || !(li->cur)) {
		return NULL;
	}
	payload = li->cur->payload;
	li->cur = li->cur->next;
	return payload;
}

/* This sets the iterator back to the beginning */
void resetListIterator(ListIterator li)
{
	if (!li) {
		return;
	}
	li->cur = li->l->head;
}

/* This frees up a list iterator */
void freeListIterator(ListIterator li)
{
	free(li);
}
