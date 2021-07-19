#ifndef _QUEUE_H_
#define _QUEUE_H_

/*
*队列结构体定义
*/
typedef struct _Queue
{
  unsigned int front:14;
  unsigned int rear:14;
  unsigned int total:14;
  unsigned int flag:1;
  unsigned char *element;
}Queue;

#define DECLARE_QUEUE(name, elemBuf, size)            \
        Queue name = {                                \
            0,			/*front*/             \
            0,			/*rear*/              \
            size,		/*total*/             \
            0,			/*flag*/              \
            elemBuf,	        /*element*/           \
        }

#define InitQueue(pQ, elemBuf, size)    \
do                                      \
{                                       \
  (pQ)->front = 0;                      \
  (pQ)->rear = 0;                       \
  (pQ)->total = (unsigned int)(size);	\
  (pQ)->flag = 0;                       \
  (pQ)->element = elemBuf;              \
}while(0)

/*清空标号位置参数*/
#define FlushQueue(pQ)      \
do                          \
{                           \
  (pQ)->front = 0;          \
  (pQ)->rear = 0;           \
  (pQ)->flag = 0;           \
}while(0)


bool Enqueue(Queue *pQ, unsigned char e);

bool Dequeue(Queue *pQ, unsigned char *pe);

bool QueueIsFull(Queue *pQ);

bool QueueIsEmpty(Queue *pQ);

#endif /*_QUEUE_H_*/

