/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : block.c
  Author       : xiamj
  Created      : 2022/9/8
  Description  : 

  History      :
  1.Date        : 2022/9/8
    Author      : xiamj
    Modification: Created file

*************************************************************************************/
#ifdef __cplussplus
extern "C"{
#endif /*end ifdef __cplussplus*/

#include "block.h"
#include "prjCommon.h"
/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/


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

/*************************************************************************************
* ��������: block_open 
* ����˵��: ��һ����
* �������: int xLen, ������Ĵ�С
*           int yLen
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
block_t * block_creat(int xLen, int yLen)
{
    block_t * pBlk = (block_t *)malloc(sizeof(block_t));
    int i = 0;
    
    argcheck(pBlk==NULL, "pBlk==NULL", NULL);
    argcheck(xLen > BLOCK_MAX_X_LEN, "xLen > MAX_X_LEN", NULL);
    argcheck(yLen > BLOCK_MAX_Y_LEN, "yLen > MAX_X_LEN", NULL);
    
    //���Ĭ������
    memset(pBlk, 0, sizeof(block_t));
    pBlk->x_len = xLen;
    pBlk->y_len = yLen;
    pBlk->x_coor = 0;
    pBlk->y_coor = 0;

    INIT_LIST_HEAD(&pBlk->list);

    //��ʼ����ʱ�ڵ�
    for(i=0; i<BLOCK_MAX_TIME_NUM; i++){
        INIT_LIST_HEAD(&pBlk->timerNode[i].node);
    }
    
    return pBlk;
}


/*************************************************************************************
* ��������: block_close 
* ����˵��: �ر�һ����
* �������: block_t * pBlk
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int block_destroy(block_t * pBlk)
{
    argcheck(pBlk==NULL, "pBlk==NULL", ERROR);
    free(pBlk);
    return OK;
}

/*************************************************************************************
* ��������: screen_print 
* ����˵��: ����ƣ���һ������������ַ�
* �������: block_t * pBlk
*           int x
*           int y
*           char * str
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int block_draw(block_t * pBlk,int x, int y, char * str)
{
    int strLen = strlen(str);

    argcheck(y > pBlk->y_len+pBlk->y_coor, "y > pScr->y_len", ERROR);
    argcheck(x+strLen > pBlk->x_len+pBlk->x_coor, "x+strLen > pScr->x_len", ERROR);

    memcpy(&pBlk->data[y][0]+x, str, strLen);
    return OK;

}

/*************************************************************************************
* ��������: block_move 
* ����˵��: ���ƶ�
* �������: block_t * pBlk
*           int x_dst �ƶ�����λ��
*           int y_dst
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: 0,0�����������Ͻ�
*************************************************************************************/
int block_move(block_t * pBlk, int x_dst, int y_dst)
{
    int i = 0, len = 0;
    char * src = NULL, *dst = NULL;
    char buff[BLOCK_MAX_Y_LEN][BLOCK_MAX_X_LEN];;

    memcpy(&buff[0], &pBlk->data[0], sizeof(pBlk->data));
    memset(&pBlk->data[0], 0, sizeof(pBlk->data));

    //�߽��ж�
    argcheck(pBlk==NULL, "pBlk==NULL", ERROR);
    if(x_dst+pBlk->x_len > SCREEN_X_LEN) x_dst = SCREEN_X_LEN-pBlk->x_len;
    if(y_dst+pBlk->y_len > SCREEN_Y_LEN) y_dst = SCREEN_Y_LEN-pBlk->y_len;
    if(x_dst < 0) x_dst = 0;
    if(y_dst < 0) y_dst = 0;

    //�ƶ�
    len = pBlk->x_len;
    for(i=0; i<pBlk->y_len; i++){ 
        src = &buff[pBlk->y_coor+i][pBlk->x_coor];
        dst = &pBlk->data[y_dst+i][x_dst];
        memcpy(dst, src, len);
    }

    //��������λ��
    pBlk->y_coor = y_dst;
    pBlk->x_coor = x_dst;

    return OK;
}

#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

