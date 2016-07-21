/********************************************
函数的定义和声明默认情况下是extern的，
但静态函数只是在声明他的文件当中可见，不能被其他文件所用。
inline必须用于函数定义，对于函数声明，inline不起作用，
inline定义的函数只在本地文件可见，应放在头文件中。
********************************************/

#include "list_hc.h"

//双链表的初始化
void list_init(struct list_node *head)
{
	head->next = head;
	head->prev = head;
}
//向双链表中插入一个节点,两个参数节点相邻
void list_add(struct list_node *newnode, struct list_node *prevnode, struct list_node *nextnode)
{
	nextnode->prev = newnode;	//先要把下一节点的前后指向关系重新链接
	newnode->next = nextnode;
	newnode->prev = prevnode;
	prevnode->next = newnode;
}
//作为下一节点随后插入
void list_add_after(struct list_node *newnode, struct list_node *entry)
{
	list_add(newnode, entry, entry->next);	//注意两个参数节点
}
//作为前一结点在前插入
void list_add_before(struct list_node *newnode, struct list_node *entry)
{
	list_add(newnode, entry->prev, entry);	//注意两个参数节点
}
//从双链表中删除一个节点,两个参数节点中间间隔一个要删除的节点
void list_del(struct list_node *prev, struct list_node *next)
{
	prev->next = next;
	next->prev = prev;
}
void list_del_node(struct list_node *entry)
{
	list_del(entry->prev, entry->next);	//注意两个参数节点
	entry->next = LIST_POISON1;		//删除的节点前后指向无效地址区
    entry->prev = LIST_POISON2;
}
//判断链表是否为空
int list_empty(const struct list_node *head)
{
    return head->next == head;
}
int list_empty_careful(const struct list_node *head)
{
        struct list_node *next = head->next;
        return (next == head) && (next == head->prev);
}
/****
//链表拼接
void __list_splice(struct list_head *list,
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
void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head);
}
*****/
