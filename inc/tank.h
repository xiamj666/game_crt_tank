/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : tank.h
  Author       : xiamj
  Created      : 2022/9/8
  Description  : 

  History      :
  1.Date        : 2022/9/8
    Author      : xiamj
    Modification: Created file

*************************************************************************************/
#ifndef __TANK_H__
#define __TANK_H__
#ifdef __cplussplus
extern "C"{
#endif /*end ifdef __cplussplus*/

#include "typedef.h"
#include "console.h"
#include "timer.h"
/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/

typedef enum{
    BULLET_DEFAULT = 0,       //*
    BULLET_TYPE2_K,           //=
    BULLET_TYPE3_L,           //#
    BULLET_TYPP_MAX,
}BULLET_TYPE;

typedef struct{
    int armlaunchSpeed;     //�����ķ����ٶ�
    int armSpeed;           //�������ƶ��ٶ�
    int armType;            //����������
    int healthVal;          //����ֵ  0~99
    int healthSelfHealing;  //���һָ�ֵ,ÿs�ָ�
    
    int attackVal;          //������0~99
    int moveSpeed;          //�ƶ��ٶ�
    int isMaster;
    int bulletCnt[BULLET_TYPP_MAX];
    screen_t * pScr;        //��Ļ����ָ��
}tankArg_t;
#define TANK_HEALTH_MAX         99

#define BULLET_DEFAULT_MAX      99
#define BULLET_TYPE2_K_MAX      10
#define BULLET_TYPE3_L_MAX      5
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
block_t * tank_creat(int x, int y, int isMaster);

int tank_destroy(block_t * pTank);
int tank_gameStart(screen_t * pScr, int foeNum);
int tank_crashCall(block_t * pTank, block_t * pOtherTank);
char * tank_getHealthStr(block_t * pTank);




int tankBullet_Creat(block_t * pTank, void* isMaster, KEY_ID KYE_ID);
int tankBullet_died(block_t * pTank,void *timerNode);




#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

#endif /*end ifndef __TANK_H__*/
