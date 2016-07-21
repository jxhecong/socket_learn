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
//双链表的初始化
static inline void list_init(struct list_node *head)
{
	head->next = head;
	head->prev = head;
}
//向双链表中插入一个节点,两个参数节点相邻
static inline void list_add(struct list_node *newnode, struct list_node *prevnode, struct list_node *nextnode)
{
	nextnode->prev = newnode;	//先要把下一节点的前后指向关系重新链接
	newnode->next = nextnode;
	newnode->prev = prevnode;
	prevnode->next = newnode;
}
//作为下一节点随后插入
static inline void list_add_after(struct list_node *newnode, struct list_node *entry)
{
	list_add(newnode, entry, entry->next);	//注意两个参数节点
}
//作为前一结点在前插入
static inline void list_add_before(struct list_node *newnode, struct list_node *entry)
{
	list_add(newnode, entry->prev, entry);	//注意两个参数节点
}
//从双链表中删除一个节点,两个参数节点中间间隔一个要删除的节点
static inline void list_del(struct list_node *prev, struct list_node *next)
{
	prev->next = next;
	next->prev = prev;
}
static inline void list_del_node(struct list_node *entry)
{
	list_del(entry->prev, entry->next);	//注意两个参数节点
	entry->next = LIST_POISON1;		//删除的节点前后指向无效地址区
    entry->prev = LIST_POISON2;
}
//判断链表是否为空
static inline int list_empty(const struct list_node *head)
{
    return head->next == head;
}
static inline int list_empty_careful(const struct list_node *head)
{
        struct list_node *next = head->next;
        return (next == head) && (next == head->prev);
}
/****
//链表拼接
static inline void __list_splice(struct list_head *list,
                 struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}
static inline void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head);
}
*****/
#endif
