/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : timer.c
  Author       : xiamj
  Created      : 2022/9/9
  Description  : 

  History      :
  1.Date        : 2022/9/9
    Author      : xiamj
    Modification: Created file
    实现一些定时相关的内容，注意：这里的timer更多的是软件层次的，并不是硬件的
*************************************************************************************/
#ifdef __cplussplus
extern "C"{
#endif /*end ifdef __cplussplus*/

#include "prjCommon.h"
#include "timer.h"
#include "list.h"

/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------*/
/*extern                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*globals                                                                           */
/*----------------------------------------------------------------------------------*/
timerNode_t g_mainNode = {0};


/*----------------------------------------------------------------------------------*/
/*locals                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*funs                                                                              */
/*----------------------------------------------------------------------------------*/

/*************************************************************************************
* 函数名称: timer_init 
* 函数说明: 初始化
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int timer_init(void)
{
    static int initFlag = FALSE;
    timerNode_t * pMainNode = &g_mainNode;

    if(initFlag == FALSE){
        memset(pMainNode, 0, sizeof(timerNode_t));
        
        INIT_LIST_HEAD(&pMainNode->node);

        pMainNode->addTime = cpu_timebase_get();
        initFlag = TRUE;
    }
    return OK;
}

int timer_eventAddCheck(timerNode_t * pNode)
{
    struct list_head_linux * pPos = NULL;
    struct list_head_linux * pPosTmp = NULL;
    timerNode_t * pTNode = 0;
    timerNode_t * pMainNode = &g_mainNode;
    
    list_for_each_safe(pPos,pPosTmp, &pMainNode->node){
        pTNode = container_of(pPos, timerNode_t, node);
        if(pNode == pTNode){
            PERROR("链表重复添加");
            return ERROR;
        }
    }

    return OK;
}


/*************************************************************************************
* 函数名称: timer_nodeInList 
* 函数说明: 判断某个节点是否在列表内
* 输入参数: timerNode_t * pNode
* 输出参数: 无
* 返 回 值: 0表示没有，1表示有
* 备     注: 无
*************************************************************************************/
int timer_nodeInList(timerNode_t * pNode)
{
    struct list_head_linux * pPos = NULL;
    struct list_head_linux * pPosTmp = NULL;
    timerNode_t * pTNode = 0;
    timerNode_t * pMainNode = &g_mainNode;
    
    list_for_each_safe(pPos,pPosTmp, &pMainNode->node){
        pTNode = container_of(pPos, timerNode_t, node);
        if(pNode == pTNode){
//          PERROR("链表重复添加");
            return 1;
        }
    }

    return 0;
}

/*************************************************************************************
* 函数名称: timer_evetRegister 
* 函数说明: 注册一个时间时间节点
* 输入参数: timerNode_t * pNode
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int timer_eventAdd(timerNode_t * pNode)
{
    timerNode_t * pMainNode = &g_mainNode;

    timer_init();
    argcheck(pNode==NULL, "pNode==NULL", ERROR);

    //检查传入的参数
    argcheck(pNode->cycleMs == 0 , "pNode->cycleMs == 0", ERROR);
    argcheck(pNode->callFun == NULL , "pNode->callFun == NULL", ERROR);
//  argcheck(pNode->rmFun == NULL , "pNode->rmFun == NULL", ERROR);

    if(timer_nodeInList(pNode) > 0){
        return ERROR;
    }
    pNode->addTime = cpu_timebase_diff_us(pMainNode->addTime);
    pNode->lastRunTime = pNode->addTime;
    list_add(&pNode->node, &pMainNode->node);
    return OK;
}

/*************************************************************************************
* 函数名称: timer_eventRm 
* 函数说明: 时间节点删除
* 输入参数: timerNode_t * pNode
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int timer_eventRm(timerNode_t * pNode)
{
    argcheck(pNode==NULL, "pNode==NULL", ERROR);
    if(timer_nodeInList(pNode) > 0){
       list_del(&pNode->node);
    }
    
    return OK;
}

/*************************************************************************************
* 函数名称: timer_process 
* 函数说明: ...
* 输入参数: timerNode_t * pMainNode
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int timer_process(void)
{
    struct list_head_linux * pPos = NULL;
    struct list_head_linux * pPosTmp = NULL;
    timerNode_t * pTNode = 0;
    timerNode_t * pMainNode = &g_mainNode;

    list_for_each_safe(pPos,pPosTmp, &pMainNode->node){
        pTNode = container_of(pPos, timerNode_t, node);

        //判断时间是否到指定周期
        if(cpu_timebase_diff_ms(pTNode->lastRunTime) > pTNode->cycleMs){

                        //判断是否要移除节点
            if(pTNode->rmFun){
                if(pTNode->rmFun(pTNode->rmArg, pTNode) == TRUE){
                    timer_eventRm(pTNode);
                    continue;
                }
            }  
        
//          printf("cycleUs:%dus diff_us:%ld\n",pTNode->cycleMs, cpu_timebase_diff_ms(pTNode->lastRunTime));
            if(pTNode->callFun){
                pTNode->callFun(pTNode->arg1, pTNode->arg2,pTNode);
            }

            //更新节点私有信息
            pTNode->lastRunTime = cpu_timebase_get();
            pTNode->RunCnt++;
                     
        }

    }
}

int timer_testRmFun(int arg1, void * tmp)
{
    timerNode_t * pTNode = (timerNode_t *)tmp;
    if(pTNode->RunCnt > arg1){
        printf("cycleUs:%dus remove.\n",pTNode->cycleMs);
        return TRUE;
    }

    return FALSE;
}


int timer_testCallFun(int arg1, int arg2, void * tmp)
{
    timerNode_t * pTNode = (timerNode_t *)tmp;
    printf("cycleUs:%dms RunCnt:%d.\n",pTNode->cycleMs,pTNode->RunCnt);
}

/*************************************************************************************
* 函数名称: timer_test 
* 函数说明: ...
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int timer_test(void)
{
    timerNode_t  Node[2] = {{0}};
    timer_init();
    TIMER_EVNET_ADD(&Node[0], 200,timer_testCallFun,NULL,NULL,timer_testRmFun,5);
    TIMER_EVNET_ADD(&Node[1], 300,timer_testCallFun,NULL,NULL,timer_testRmFun,6);

    while(1)
    {
        timer_process();
    }
}
#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

