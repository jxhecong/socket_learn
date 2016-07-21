/********************************************
函数的定义和声明默认情况下是extern的，
但静态函数只是在声明他的文件当中可见，不能被其他文件所用。
inline必须用于函数定义，对于函数声明，inline不起作用，
inline定义的函数只在本地文件可见，应放在头文件中。
********************************************/
#ifndef _LIST_HC_H
#define _LIST_HC_H

//通过已知的成员member的地址ptr获取该结构体type的首地址
//#define list_entry(ptr, type, member) \
//    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
//遍历链表，删除操作不可用，如当前链表pos被删除，pos = pos->next错误

#define list_for_each(pos, head) \
    for (pos = head->next; pos != head; pos = pos->next)
//遍历链表，进行删除节点操作时使用
#define list_for_each_safe(pos, n, head) \
    for (pos = head->next, n = pos->next; pos != head; \
        pos = n, n = pos->next)

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

//双链表的定义
struct list_node
{
	struct list_node *prev;
	struct list_node *next;
};

void list_init(struct list_node *head);
//static inline void list_add(struct list_node *newnode, struct list_node *prevnode, struct list_node *nextnode);
void list_add_after(struct list_node *newnode, struct list_node *entry);
void list_add_before(struct list_node *newnode, struct list_node *entry);
//static inline void list_del(struct list_node *prev, struct list_node *next);
void list_del_node(struct list_node *entry);
int list_empty(const struct list_node *head);
int list_empty_careful(const struct list_node *head);

#endif
