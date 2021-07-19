/*
*疑问:队列算法--业务模式的多重化
*1.数据采取循环存储
*2.最大值溢出标志
*应用场景分析:
*/
#include "type.h"
#include "queue.h"

bool Enqueue(Queue *pQ, unsigned char e)
{          
	unsigned int rear;
	unsigned int front;

	rear = pQ->rear;
	front = pQ->front;

	if ((front != rear) || (!pQ->flag))            
	{
		pQ->element[rear++] = e;
		if (rear >= pQ->total)
		{
			rear = 0;
		}  
		if (front == rear)
		{
			pQ->flag = 1;
		}
		pQ->rear = rear;
		return TRUE;                          
	}                              
	else                                
	{  
		return FALSE;                          
	}
}

bool Dequeue(Queue *pQ, unsigned char *pe)
{  
	unsigned int rear;
	unsigned int front;

	rear = pQ->rear;
	front = pQ->front;

	if ((front != rear) || (pQ->flag))            
	{                                  
		*pe = pQ->element[front++];                  
		if (front >= pQ->total)  
		{    
			front = 0;
		}
		if (pQ->flag)                    
		{  
			pQ->flag = 0;  
		}
		pQ->front = front;
		return TRUE;  
	}        
	else
	{
		return FALSE;
	}
}

bool QueueIsFull(Queue *pQ)
{
  return  ((pQ->front == pQ->rear) && (pQ->flag));
}

bool QueueIsEmpty(Queue *pQ)
{
  return  ((pQ->front == pQ->rear) && (!pQ->flag));
}
