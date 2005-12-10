/*
 *
 * This file is the header of a basic, generic list (well, technically, a
 * queue) object implementation.
 *
 */
#ifndef WCALC_PACKETLIST
#define WCALC_PACKETLIST

struct _list;
typedef struct _list *List;

/* this is for the memory pools */
void lists_init(void);
void lists_cleanup(void);

void addToList(List *, void *);
void *getHeadOfList(List);
void *peekListElement(List, size_t);
inline void *peekAheadInList(List);
inline unsigned long listLen(List);
void freeList(List *);

#endif
