/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : screen.h
  Author       : xiamj
  Created      : 2022/9/8
  Description  : 

  History      :
  1.Date        : 2022/9/8
    Author      : xiamj
    Modification: Created file

*************************************************************************************/
#ifndef __SCREEN_H__
#define __SCREEN_H__
#ifdef __cplussplus
extern "C"{
#endif /*end ifdef __cplussplus*/

#include "list.h"
#include "block.h"
#include "timer.h"
#include "typedef.h"
/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/
#define SCREEN_MAX_X_LEN    200
#define SCREEN_MAX_Y_LEN    200

#define SCREEN_X_LEN        (g_pScree->x_len)
#define SCREEN_Y_LEN        (g_pScree->y_len)
#define GET_SCREEN_PTR()    (g_pScree)

#define ESC		            "\033"
#define ESC_CLEAR           ESC"[H"ESC"[J"
#define CLEAR_2_EOL         ESC"[K"         /* The escape code to clear to the end of line */
#define HOME		        ESC"[H"


typedef void (*gameStatusPirnt)(void);

typedef struct{
    int x_len;              //屏幕的x轴长度
    int y_len;              //屏幕的y轴长度
    int fpsVal;             //FPS值
    int gameIsOver;
    gameStatusPirnt        gameStatPrint;
    struct list_head_linux blockList;  //块链表
    timerNode_t timerNode;  //定时节点
    int flushTime;          //刷新时间间隔，2针屏幕刷新间隔(ms)
    char data[SCREEN_MAX_Y_LEN][SCREEN_MAX_X_LEN]; //保存屏幕的数据
}screen_t;


/*----------------------------------------------------------------------------------*/
/*extern                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*globals                                                                           */
/*----------------------------------------------------------------------------------*/
screen_t * g_pScree;


/*----------------------------------------------------------------------------------*/
/*locals                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*funs                                                                              */
/*----------------------------------------------------------------------------------*/

/*************************************************************************************
* 函数名称: screen_cursor_move 
* 函数说明: 移动光标到指定位置
* 输入参数: line
*           col
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
#define screen_cursor_move(line,col) do{       \
	printf(ESC"[%u;%uH", line, col);    \
}while(0);



screen_t * screen_init(int xLen, int yLen);
int screen_print(screen_t * pScr,int x, int y, char * str);
int screen_show(screen_t * pScr);
int screen_clear(screen_t * pScr, int isCrtCler);
int screen_flush(screen_t * pScr);
int screen_process(screen_t * pScr);
int screen_listAddBlock(screen_t * pScr, block_t * pBlk);
int screen_listRmBlock(block_t * pBlk);
int screen_BlockNum(screen_t * pScr);
int screen_numOfBlockType(screen_t * pScr, BLOCK_TYPE type);
int screen_gameOverPrint(screen_t * pScr);

int scree_test(void);





#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

#endif /*end ifndef __SCREEN_H__*/
