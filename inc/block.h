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
#define BLOCK_MAX_TIME_NUM  4           //block节点做多允许定时节点个数，一个节点允许挂一个定时
typedef enum{
    BLOCK_DEFAULT           = 0x00,
    BLOCK_TANK              = 0x01,        //tank节点
    BLOCK_TANK_FOE          = 0x02,        //敌方tank节点
    BLOCK_TANK_BULLET       = 0x10,        //tank 子弹节点
    BLOCK_TANK_BULLET_FOE   = 0x20,        //敌方tank 子弹节点
    BLOCK_MAX,
}BLOCK_TYPE;

/*
    block_self: 当前block的参数
    block_other：另一个block的参数
*/
typedef int (*crashCallFun)(void * block_self, void * block_other);

typedef struct{

    int x_coor;             //坐标位置x轴 coordinates
    int y_coor;             //坐标位置y轴
    
    int x_len;              //描述块的x轴长度
    int y_len;              //描述块的y轴长度
    void * privateData;     //私有变量
    struct list_head_linux list;

    BLOCK_TYPE blockType;   //块类型
    timerNode_t timerNode[BLOCK_MAX_TIME_NUM];  //定时节点
    //碰撞处理
    int crashVal;   //碰撞值，两个快重叠时，值小的会调用crashCall,
                    //参考screen_BlockCrashCheck
    crashCallFun crashCall;
    
    char data[BLOCK_MAX_Y_LEN][BLOCK_MAX_X_LEN]; //描述块的内容，空格和0为透明
}block_t;

#define TYPE_BULLET         (BLOCK_TANK_BULLET_FOE+BLOCK_TANK_BULLET)   //块类型为子弹
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
