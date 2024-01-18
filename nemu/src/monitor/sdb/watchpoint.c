/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "sdb.h"

#define NR_WP 32



#define LIST_FOREACH(head, next, current, TYPE) \
  TYPE *_node = NULL;                           \
  TYPE *current = NULL;                         \
  for (current = _node = head->next; _node != NULL; current = _node = _node->next)

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
// 空闲的意思是不使用，而不是不使能

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
void show_wp_pool()
{
  printf("Show watch points \n");
  // for (int i = 0; i < NR_WP; i++)
  // {
  //   printf("NO: %4d next: %4d pw: 0x%-16lx \n", wp_pool[i].NO, wp_pool[i].next->NO, wp_pool[i].address);
  // }
  printf("%8s %8s %4s %30s\n", "Num", "Type", "Enb", "What");
  WP *node = head;
  while (node->next != NULL)
  {
    printf("%8d %8s %4s %30s\n", node->NO, "hw", "y", node->expr);
  }
}

// 从尾部添加
WP *wp_add(WP *head_node, WP *node)
{
  int count = 0;
  LIST_FOREACH(head_node, next, current, WP)
  {
    count++;
    if (count > NR_WP - 1)
    {
      Log("All watch points are in use");
      assert(0);
    }
    if (current->next == NULL)
    {
      current->next = node;
      node->next = NULL;
      node->NO = count;
      break;
    }
  }
  return node;
}

// 从尾部删除, 返回删除的节点
WP *wp_delete(WP *head_node)
{
  if (head_node->next == NULL)
  {
    printf("You are delete the last node");
    return head_node;
  }
  LIST_FOREACH(head_node, next, current, WP)
  {
    if (current->next != NULL)
    {
      WP *node = current->next;
      if (node->next == NULL)
      {
        current->next = NULL;
        node->NO = 0;
        node->expr = NULL;
        return node;
      }
    }
  }
  assert(0);
  return NULL;
}

// 寻找对应值的节点
WP *wp_find_no(WP *head_node, int no)
{
  WP *node = head_node;
  while (node->NO != no)
  {
    node = node->next;
  }
  return node;
}

// 返回的是删除后的链表
WP *wp_del_no(WP *head_node, WP *node)
{
  WP *_node = head_node;
  // 第一个
  if (head_node->NO == node->NO)
  {
    _node = node->next;
    node->next = NULL;
    return _node;
  }
  WP *pre = head_node;
  WP *cur = pre->next;
  while (1)
  {
    if (cur->NO == node->NO)
    {
      if (cur->next != NULL)
      {
        pre->next = cur->next;
      }
      node->next = NULL;
      node->NO = 0;
      return head_node;
    }
    if (cur->next == NULL)
    {
      Log("Find all but no find");
      return head_node;
    }
    pre = pre->next;
    cur = cur->next;
  }
  return head_node;
}

// 排序
WP *wp_order(WP *head_node)
{
  int count = 0;
  WP *node = head_node;
  while (1)
  {
    count += 1;
    node->NO = count;
    if (node->next == NULL)
    {
      break;
    }
    node = node->next;
  }
  node->NO = count + 1;
  return head_node;
}

// 计数
int wp_count(WP *head_node)
{
  int count = 0;
  WP *node = head_node;
  while (node->next != NULL)
  {
    count += 1;
    node = node->next;
  }
  return count + 1;
}

// 从空闲的那儿拿一个出来，添加到使用中的列表
WP *new_wp(char *str)
{
  WP *node = wp_delete(free_);
  node->next = NULL;
  node->expr = str;
  if (head == NULL)
  {
    head = node;
    node->NO = 1;
    return head;
  }
  else
  {
    wp_add(head, node);
    return head;
  }
  return head;
}

// 就是从使用中的一个拿出来放到空闲中的
void free_wp(int no)
{
  WP *node = wp_find_no(head, no);
  head = wp_del_no(head, node);
  wp_order(head);

  node->NO = 0;
  node->expr = NULL;
  node->next = NULL;
  wp_add(free_, node);
}
