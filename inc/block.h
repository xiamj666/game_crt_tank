/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : block.h
  Author       : xiamj
  Created      : 2022/9/8
  Description  : 

  History      :
  1.Date        : 2022/9/8
    Author      : xiamj
    Modification: Created file

*************************************************************************************/
#ifndef __BLOCK_H__
#define __BLOCK_H__
#ifdef __cplussplus
extern "C"{
#endif /*end ifdef __cplussplus*/

#include "list.h"
#include "timer.h"
/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/
#define BLOCK_MAX_X_LEN     200
#define BLOCK_MAX_Y_LEN     200
#define BLOCK_MAX_TIME_NUM  4           //block�ڵ���������ʱ�ڵ������һ���ڵ������һ����ʱ
typedef enum{
    BLOCK_DEFAULT           = 0x00,
    BLOCK_TANK              = 0x01,        //tank�ڵ�
    BLOCK_TANK_FOE          = 0x02,        //�з�tank�ڵ�
    BLOCK_TANK_BULLET       = 0x10,        //tank �ӵ��ڵ�
    BLOCK_TANK_BULLET_FOE   = 0x20,        //�з�tank �ӵ��ڵ�
    BLOCK_MAX,
}BLOCK_TYPE;

/*
    block_self: ��ǰblock�Ĳ���
    block_other����һ��block�Ĳ���
*/
typedef int (*crashCallFun)(void * block_self, void * block_other);

typedef struct{

    int x_coor;             //����λ��x�� coordinates
    int y_coor;             //����λ��y��
    
    int x_len;              //�������x�᳤��
    int y_len;              //�������y�᳤��
    void * privateData;     //˽�б���
    struct list_head_linux list;

    BLOCK_TYPE blockType;   //������
    timerNode_t timerNode[BLOCK_MAX_TIME_NUM];  //��ʱ�ڵ�
    //��ײ����
    int crashVal;   //��ײֵ���������ص�ʱ��ֵС�Ļ����crashCall,
                    //�ο�screen_BlockCrashCheck
    crashCallFun crashCall;
    
    char data[BLOCK_MAX_Y_LEN][BLOCK_MAX_X_LEN]; //����������ݣ��ո��0Ϊ͸��
}block_t;

#define TYPE_BULLET         (BLOCK_TANK_BULLET_FOE+BLOCK_TANK_BULLET)   //������Ϊ�ӵ�
#define BLOCK_IS_TANK_BULLET(pBlock) (pBlock->blockType & TYPE_BULLET  ? TRUE : FALSE)
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
block_t * block_creat(int xLen, int yLen);
int block_destroy(block_t * pBlk);
int block_draw(block_t * pBlk,int x, int y, char * str);
int block_move(block_t * pBlk, int x_dst, int y_dst);

#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

#endif /*end ifndef __BLOCK_H__*/
