#ifndef __TIMEWHEEL__
#define __TIMEWHEEL__

#define WHEELSIZE 15

typedef struct timeNode_s{
    int netfd;
    struct timeNode_s *next;
} timeNode_t;

typedef struct timeSlot_s {
    timeNode_t *head;
    timeNode_t *tail;
} timeSlot_t;

int headInsert(timeSlot_t *slot, int netfd);
int headDel(timeSlot_t *slot);
int deltimeNode(timeSlot_t *slot, int netfd);

#endif 
