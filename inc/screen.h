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
    int x_len;              //��Ļ��x�᳤��
    int y_len;              //��Ļ��y�᳤��
    int fpsVal;             //FPSֵ
    int gameIsOver;
    gameStatusPirnt        gameStatPrint;
    struct list_head_linux blockList;  //������
    timerNode_t timerNode;  //��ʱ�ڵ�
    int flushTime;          //ˢ��ʱ������2����Ļˢ�¼��(ms)
    char data[SCREEN_MAX_Y_LEN][SCREEN_MAX_X_LEN]; //������Ļ������
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
* ��������: screen_cursor_move 
* ����˵��: �ƶ���굽ָ��λ��
* �������: line
*           col
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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
