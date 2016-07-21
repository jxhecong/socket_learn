/********************************************
�����Ķ��������Ĭ���������extern�ģ�
����̬����ֻ�������������ļ����пɼ������ܱ������ļ����á�
inline�������ں������壬���ں���������inline�������ã�
inline����ĺ���ֻ�ڱ����ļ��ɼ���Ӧ����ͷ�ļ��С�
********************************************/
#ifndef _LIST_HC_H
#define _LIST_HC_H

//ͨ����֪�ĳ�Աmember�ĵ�ַptr��ȡ�ýṹ��type���׵�ַ
//#define list_entry(ptr, type, member) \
//    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
//��������ɾ�����������ã��統ǰ����pos��ɾ����pos = pos->next����

#define list_for_each(pos, head) \
    for (pos = head->next; pos != head; pos = pos->next)
//������������ɾ���ڵ����ʱʹ��
#define list_for_each_safe(pos, n, head) \
    for (pos = head->next, n = pos->next; pos != head; \
        pos = n, n = pos->next)

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

//˫����Ķ���
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
