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
    ʵ��һЩ��ʱ��ص����ݣ�ע�⣺�����timer������������εģ�������Ӳ����
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
* ��������: timer_init 
* ����˵��: ��ʼ��
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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
            PERROR("�����ظ����");
            return ERROR;
        }
    }

    return OK;
}


/*************************************************************************************
* ��������: timer_nodeInList 
* ����˵��: �ж�ĳ���ڵ��Ƿ����б���
* �������: timerNode_t * pNode
* �������: ��
* �� �� ֵ: 0��ʾû�У�1��ʾ��
* ��     ע: ��
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
//          PERROR("�����ظ����");
            return 1;
        }
    }

    return 0;
}

/*************************************************************************************
* ��������: timer_evetRegister 
* ����˵��: ע��һ��ʱ��ʱ��ڵ�
* �������: timerNode_t * pNode
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int timer_eventAdd(timerNode_t * pNode)
{
    timerNode_t * pMainNode = &g_mainNode;

    timer_init();
    argcheck(pNode==NULL, "pNode==NULL", ERROR);

    //��鴫��Ĳ���
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
* ��������: timer_eventRm 
* ����˵��: ʱ��ڵ�ɾ��
* �������: timerNode_t * pNode
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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
* ��������: timer_process 
* ����˵��: ...
* �������: timerNode_t * pMainNode
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int timer_process(void)
{
    struct list_head_linux * pPos = NULL;
    struct list_head_linux * pPosTmp = NULL;
    timerNode_t * pTNode = 0;
    timerNode_t * pMainNode = &g_mainNode;

    list_for_each_safe(pPos,pPosTmp, &pMainNode->node){
        pTNode = container_of(pPos, timerNode_t, node);

        //�ж�ʱ���Ƿ�ָ������
        if(cpu_timebase_diff_ms(pTNode->lastRunTime) > pTNode->cycleMs){

                        //�ж��Ƿ�Ҫ�Ƴ��ڵ�
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

            //���½ڵ�˽����Ϣ
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
* ��������: timer_test 
* ����˵��: ...
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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

