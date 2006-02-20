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
struct _list_iterator;
typedef struct _list_iterator *ListIterator;

/* this is for the memory pools */
void lists_init(void);
void lists_cleanup(void);

/* List operations */
void addToList(List *, void *);
void addToListHead(List *, void *);
void *getHeadOfList(List);
void *peekListElement(List, size_t);
void *getListElement(List, size_t);
inline void *peekAheadInList(List);
inline unsigned long listLen(List);
void removeFromList(List, void *);
void freeList(List *);

/* ListIterator operations */
ListIterator getListIterator(List);
void *currentListElement(ListIterator);
void *nextListElement(ListIterator);
void resetListIterator(ListIterator);
void freeListIterator(ListIterator);

#endif
