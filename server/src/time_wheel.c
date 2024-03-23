#include "../include/main.h"

// 时间轮头插
int headInsert(timeSlot_t *slot, int netfd){
    timeNode_t *new_timeNode = (timeNode_t *)calloc(1, sizeof(timeNode_t));
    new_timeNode->netfd = netfd;
    new_timeNode->next = slot->head;
    slot->head = new_timeNode;
    if(slot->tail == NULL){
        slot->tail = new_timeNode;
    }
    return 0;
}

// 在时间轮当前轮槽头删
int headDel(timeSlot_t *slot){
    if (slot == NULL || slot->head == NULL){
        return 0;
    }
    // 如果只有一个节点
    if (slot->head == slot->tail){
        free(slot->head);
        slot->head = NULL;
        slot->tail = NULL;
        return 0;
    }
    // 删除头结点
    timeNode_t *temp = slot->head;
    slot->head = slot->head->next;
    free(temp);
}


// 链表任意位置删除结点
int deltimeNode(timeSlot_t *slot, int netfd){
    // return 1 表示删除失败，return 0 表示找到并删除成功
    if(slot == NULL || slot->head == NULL){
        return 1;
    }
    // 如果删除的是头结点
    if (slot->head->netfd == netfd){
        // 如果只有一个结点
        if(slot->head == slot->tail){
            free(slot->head);
            slot->head = NULL;
            slot->tail = NULL;
            return 0;
        }
        timeNode_t *temp = slot->head;
        slot->head = slot->head->next;
        free(temp);
        return 0;
    }
    // 找到待删除节点的前一个结点
    timeNode_t *prev = slot->head;
    while(prev->next != NULL && prev->next->netfd != netfd){
        prev = prev->next;
    }
    // 如果找到了待删除结点
    if (prev->next != NULL){
        timeNode_t *temp = prev->next;
        prev->next = prev->next->next;
        // 如果删除的是尾结点，则更新尾结点
        if (prev->next == NULL){
            slot->tail == prev;
        }
        free(temp);
        return 0;
    }
    printf("del fail\n");
    return 1;
}
