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

/*删除条件*/
typedef int (*timerRmCondition)(void*rmArg,void *timerNode);


typedef struct{
    UINT32  cycleMs;        //周期
    /*定时回调函数*/
    timerCallFun callFun;   //周期到来时回调函数
    void * arg1;
    void * arg2;

    /*删除定时回调的条件,当rmFun返回TRUE时，删除该节点*/
    timerRmCondition rmFun;
    void * rmArg;

    //私有变量，timer_evetRegister会自动填充
    time_t  addTime;        //添加时间
    time_t  lastRunTime;    //上次执行该节点的时间
    UINT32  RunCnt;         //执行的次数
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
