/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : timer.h
  Author       : xiamj
  Created      : 2022/9/9
  Description  : 

  History      :
  1.Date        : 2022/9/9
    Author      : xiamj
    Modification: Created file

*************************************************************************************/
#ifndef __TIMER_H__
#define __TIMER_H__
#ifdef __cplussplus
extern "C"{
#endif /*end ifdef __cplussplus*/

#include "typedef.h"

/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/
typedef void (*timerCallFun)(void * arg1, void * arg2,void *timerNode);

/*ɾ������*/
typedef int (*timerRmCondition)(void*rmArg,void *timerNode);


typedef struct{
    UINT32  cycleMs;        //����
    /*��ʱ�ص�����*/
    timerCallFun callFun;   //���ڵ���ʱ�ص�����
    void * arg1;
    void * arg2;

    /*ɾ����ʱ�ص�������,��rmFun����TRUEʱ��ɾ���ýڵ�*/
    timerRmCondition rmFun;
    void * rmArg;

    //˽�б�����timer_evetRegister���Զ����
    time_t  addTime;        //���ʱ��
    time_t  lastRunTime;    //�ϴ�ִ�иýڵ��ʱ��
    UINT32  RunCnt;         //ִ�еĴ���
    struct list_head_linux node;   
}timerNode_t;




/*----------------------------------------------------------------------------------*/
/*extern                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*globals                                                                           */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*locals                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*funs                                                                              */
/*----------------------------------------------------------------------------------*/
int timer_init(void);
int timer_eventAdd(timerNode_t * pNode);
int timer_eventRm(timerNode_t * pNode);
int timer_process(void);
int timer_test(void);

#define TIMER_EVNET_ADD(pNode,_cycleMs,_callFun,_arg1,_arg2,_rmFun,_rmArg) do{ \
    (pNode)->cycleMs = _cycleMs;                    \
    (pNode)->callFun = (timerCallFun)_callFun;      \
    (pNode)->arg1 = (void*)_arg1;                   \
    (pNode)->arg2 = (void*)_arg2;                   \
    (pNode)->rmFun = (timerRmCondition)_rmFun;      \
    (pNode)->rmArg = (void*)_rmArg;                 \
    timer_eventAdd(pNode);                          \
}while(0);




#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

#endif /*end ifndef __TIMER_H__*/
