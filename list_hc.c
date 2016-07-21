/********************************************
�����Ķ��������Ĭ���������extern�ģ�
����̬����ֻ�������������ļ����пɼ������ܱ������ļ����á�
inline�������ں������壬���ں���������inline�������ã�
inline����ĺ���ֻ�ڱ����ļ��ɼ���Ӧ����ͷ�ļ��С�
********************************************/

#include "list_hc.h"

//˫����ĳ�ʼ��
void list_init(struct list_node *head)
{
	head->next = head;
	head->prev = head;
}
//��˫�����в���һ���ڵ�,���������ڵ�����
void list_add(struct list_node *newnode, struct list_node *prevnode, struct list_node *nextnode)
{
	nextnode->prev = newnode;	//��Ҫ����һ�ڵ��ǰ��ָ���ϵ��������
	newnode->next = nextnode;
	newnode->prev = prevnode;
	prevnode->next = newnode;
}
//��Ϊ��һ�ڵ�������
void list_add_after(struct list_node *newnode, struct list_node *entry)
{
	list_add(newnode, entry, entry->next);	//ע�����������ڵ�
}
//��Ϊǰһ�����ǰ����
void list_add_before(struct list_node *newnode, struct list_node *entry)
{
	list_add(newnode, entry->prev, entry);	//ע�����������ڵ�
}
//��˫������ɾ��һ���ڵ�,���������ڵ��м���һ��Ҫɾ���Ľڵ�
void list_del(struct list_node *prev, struct list_node *next)
{
	prev->next = next;
	next->prev = prev;
}
void list_del_node(struct list_node *entry)
{
	list_del(entry->prev, entry->next);	//ע�����������ڵ�
	entry->next = LIST_POISON1;		//ɾ���Ľڵ�ǰ��ָ����Ч��ַ��
    entry->prev = LIST_POISON2;
}
//�ж������Ƿ�Ϊ��
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
//����ƴ��
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
