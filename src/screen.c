/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : screen.c
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
screen_t * g_pScree = NULL;
int BlockCrashCheck = 0;


/*----------------------------------------------------------------------------------*/
/*locals                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*funs                                                                              */
/*----------------------------------------------------------------------------------*/


/*************************************************************************************
* ��������: screen_open 
* ����˵��: ��һ����Ļ
* �������: int xLen ��Ļ��x����
*           int yLen y����
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
screen_t * screen_init(int xLen, int yLen)
{
    screen_t * pScr = (screen_t *)malloc(sizeof(screen_t));
    
    argcheck(pScr==NULL, "pScr==NULL", NULL);
    argcheck(xLen > SCREEN_MAX_X_LEN, "xLen > MAX_X_LEN", NULL);
    argcheck(yLen > SCREEN_MAX_Y_LEN, "yLen > MAX_X_LEN", NULL);

    memset(pScr, 0, sizeof(pScr));
    pScr->x_len = xLen;
    pScr->y_len = yLen;

    pScr->flushTime = 20;               //ˢ��ʱ��20ms
    INIT_LIST_HEAD(&pScr->blockList);
    //���Ĭ������
    int i = 0,j = 0;
    for(j=0; j<pScr->y_len; j++){
        for(i=0; i<pScr->x_len; i++){
            pScr->data[j][i] = ' ';
        }
    }
    g_pScree = pScr;

    //console_evetRegister(KEY_ANY, (evntCall)screen_show, pScr,NULL);
    console_evetRegister(KEY_ESC, (evntCall)screen_flush, pScr,NULL);

    TIMER_EVNET_ADD(&pScr->timerNode, 20,screen_process,pScr,NULL,NULL,NULL);
    return pScr;
}

/*************************************************************************************
* ��������: screen_close 
* ����˵��: �ر���Ļ
* �������: screen_t *pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_close(screen_t *pScr)
{
    free(pScr);
}

/*************************************************************************************
* ��������: screen_print 
* ����˵��: ��Ļ��ӡ
* �������: int x
*           int y
*           int len
*           char * str
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_printStr(screen_t * pScr,int x, int y, char * str)
{
    int strLen = strlen(str);

    argcheck(y > pScr->y_len, "y > pScr->y_len", ERROR);
    argcheck(x+strLen > pScr->x_len, "x+strLen > pScr->x_len", ERROR);

    memcpy(&pScr->data[y][0]+x, str, strLen);
    return OK;

}

/*************************************************************************************
* ��������: screen_printBlock 
* ����˵��: ����Ļ�д�ӡ��
* �������: screen_t * pScr
*           block_t * pBlk
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_printBlock(screen_t * pScr, block_t * pBlk)
{
    int x = 0, y = 0;
    argcheck(pScr==NULL, "pScr==NULL", ERROR);
    argcheck(pBlk==NULL, "pBlk==NULL", ERROR);

    for(y=0; y<pScr->y_len; y++){
        for(x=0; x<pScr->x_len; x++){
            if(pBlk->data[y][x] != 0){  //ֻҪ�ǲ�Ϊ0�Ϳ�����ȥ
                pScr->data[y][x] = pBlk->data[y][x];
            }
        }
    }
    return OK;
}


/*************************************************************************************
* ��������: screen_clear 
* ����˵��: ����Ļ
* �������: screen_t * pScr
            isCrtCler : �Ƿ�ˢ��crt������
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_clear(screen_t * pScr, int isCrtCler)
{
    int i = 0,j = 0;
    if(isCrtCler){
        printf(ESC_CLEAR);
    }
    for(j=0; j<pScr->y_len; j++){
        for(i=0; i<pScr->x_len; i++){
            pScr->data[j][i] = ' ';
        }
    }    
}

/*************************************************************************************
* ��������: screen_gameStatusShow 
* ����˵��: ��Ϸ״̬��ʾ
* �������: screen_t * pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
void screen_gameStatPrint(screen_t * pScr)
{
    if(pScr->gameStatPrint){
        pScr->gameStatPrint();
    }
}

/*************************************************************************************
* ��������: screen_statusShow 
* ����˵��: �����Ͻ���ʾ״̬��Ϣ
* �������: screen_t * pScr
*           int fpsVal
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
void screen_statusShow(screen_t * pScr, int fpsVal)
{
    char buff[32] = {0};
    int xPos = 0;
    sprintf(buff,"FPS:%d blockNum:%3d.",fpsVal,screen_BlockNum(pScr));
    
    xPos = pScr->x_len-strlen(buff);
            
    screen_printStr(pScr,xPos,0,buff);
}

/*************************************************************************************
* ��������: screen_show 
* ����˵��: ��ʾ��Ļ
* �������: screen_t * pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_show(screen_t * pScr)
{
    int i = 0,j = 0;
    static time_t timeVal = 0;
    static int  fpsCnt = 0;
    argcheck(pScr==NULL, "pScr==NULL", ERROR);
        
    //��ӡ����
    printf("��");
    for(i=0; i<pScr->x_len; i++){
        printf("-");
    }
    printf("��\n");

    //��ӡ����
    for(j=0; j<pScr->y_len; j++){
        printf("|%s|\n",pScr->data[j]);
    }
    
    //��ӡĩ��
    printf("��");
    for(i=0; i<pScr->x_len; i++){
        printf("-");
    }
    printf("��\n");
    fflush(stdout);

    //FPSͳ��
    fpsCnt++;
    if(cpu_timebase_diff_ms(timeVal)>500){
        timeVal = cpu_timebase_get();
        pScr->fpsVal = fpsCnt;
        fpsCnt = 0;
    }
    
    screen_gameStatPrint(pScr);
    screen_cursor_move(0,0);        //�������ڳ�ʼ����λ����
    screen_clear(pScr,FALSE);
    
    return OK;
}

/*************************************************************************************
* ��������: screen_flush 
* ����˵��: ��Ļˢ��
* �������: screen_t * pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_flush(screen_t * pScr)
{
    screen_clear(pScr,TRUE);
    screen_process(pScr);
}


/*************************************************************************************
* ��������: screen_perfTest 
* ����˵��: ��Ļ���ܲ���
* �������: screen_t * pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_perfTest(screen_t * pScr)
{
    int cnt = 0, xPos = 0;
    time_t timeVal = 0;
    char buff[32] = {0};
    
    screen_clear(pScr,FALSE);
    timeVal = cpu_timebase_get();
    while(1){
        cnt++;        
        
        if(cpu_timebase_diff_ms(timeVal)>1000){
            screen_clear(pScr,FALSE);
        
            sprintf(buff,"��Ļ��С:%dX%d.",pScr->x_len,pScr->y_len);
            xPos = strlen(buff) < pScr->x_len ? 
                    (pScr->x_len-strlen(buff))/2 : 4;
            screen_printStr(pScr,xPos,pScr->y_len/2-1,buff);
            
            memset(buff, 0, sizeof(buff));            
            sprintf(buff,"��Ļˢ����:%d��/s.",cnt);
            xPos = strlen(buff) < pScr->x_len ? 
                    (pScr->x_len-strlen(buff))/2 : 4;
            screen_printStr(pScr,xPos,pScr->y_len/2,buff);

            memset(buff, 0, sizeof(buff));
            sprintf(buff,"��ӡ�ٶ�:%dk/s.",cnt*pScr->y_len*pScr->x_len/1024);
            xPos = strlen(buff) < pScr->x_len ? 
                    (pScr->x_len-strlen(buff))/2 : 4;
            screen_printStr(pScr,xPos,pScr->y_len/2+1,buff);
            
            timeVal = cpu_timebase_get();            
            cnt=0;
        }
        screen_show(pScr);
    }

}


/*************************************************************************************
* ��������: screen_gameOverPrint 
* ����˵��: ��Ϸ������ӡ
* �������: screen_t * pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_gameOverPrint(screen_t * pScr)
{
    int cnt = 0, xPos = 0;
    time_t timeVal = 0;
    char buff[32] = {0};
    
    screen_clear(pScr,FALSE);
    timeVal = cpu_timebase_get();
    while(1){
        cnt++;        
//      if(cpu_timebase_diff_ms(timeVal)>1000){
//          screen_clear(pScr,FALSE);
        
            sprintf(buff,"GAME OVER.");
            xPos = strlen(buff) < pScr->x_len ? 
                    (pScr->x_len-strlen(buff))/2 : 4;
            screen_printStr(pScr,xPos,pScr->y_len/2-1,buff);
            
            timeVal = cpu_timebase_get();            
            cnt=0;
//      }
        screen_show(pScr);
    }

}


/*************************************************************************************
* ��������: scree_listAddBlock 
* ����˵��: ������ӵ���Ļ�б���
* �������: screen_t * pScr
*           block_t * pBlk
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_listAddBlock(screen_t * pScr, block_t * pBlk)
{
    list_add(&pBlk->list, &pScr->blockList);
}

/*************************************************************************************
* ��������: screen_listRmBlock 
* ����˵��: �������Ļ�б���ɾ��
* �������: block_t * pBlk
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_listRmBlock(block_t * pBlk)
{
    list_del(&pBlk->list);
}

/*************************************************************************************
* ��������: screen_listNum 
* ����˵��: ��ȡ��Ļ�е�block����
* �������: screen_t * pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_BlockNum(screen_t * pScr)
{
    struct list_head_linux * pos = NULL;
    block_t * pBlk = NULL;
    int num = 0;
    list_for_each(pos, &pScr->blockList){
        num++;
    
    }
    return num;
}

/*************************************************************************************
* ��������: screen_numOfBlockType 
* ����˵��: ��ȡ����Ļ�п����͵�����
* �������: screen_t * pScr
*           BLOCK_TYPE type
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_numOfBlockType(screen_t * pScr, BLOCK_TYPE type)
{
    struct list_head_linux * pos = NULL;
    block_t * pBlk = NULL;
    int num = 0;
    list_for_each(pos, &pScr->blockList){
        pBlk = container_of(pos, block_t, list);
        if(pBlk->blockType == type){
            num++;
        }
    }
    return num;

}


/*************************************************************************************
* ��������: screen_crashCheck 
* ����˵��: ����ײ���
* �������: screen_t * pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_BlockCrashCheck(screen_t * pScr, block_t * pBlock)
{
    struct list_head_linux * pos = NULL;
    block_t * pBlk[4] = {0};
    block_t * pBlkTmp = NULL;
    int pBlkTmpCnt = 0;

    int xIndex = 0, yIndex = 0;
    for(xIndex=pBlock->x_coor; xIndex<(pBlock->x_len+pBlock->x_coor); xIndex++){
        for(yIndex=pBlock->y_coor; yIndex<(pBlock->y_len+pBlock->y_coor); yIndex++){
            pBlkTmpCnt = 0;
            list_for_each(pos, &pScr->blockList){
                if(pBlkTmpCnt >= 4)
                    break;
                pBlkTmp = container_of(pos, block_t, list);
                if(!(pBlkTmp->data[yIndex][xIndex] == 0 || pBlkTmp->data[yIndex][xIndex] == ' ')){
                    pBlk[pBlkTmpCnt] = pBlkTmp;
                    pBlkTmpCnt++;
                }

            }

            //��ͬһ����������2�����ϵĿ��غ�
            if(pBlkTmpCnt >= 2){
#if 1
                //PERROR("x:%d y:%d crashVal:%d %d.\n",xIndex,yIndex, pBlk[0]->crashVal , pBlk[1]->crashVal);
                if(pBlk[0]->crashVal == pBlk[1]->crashVal){
                    //���Ӳֵ��ͬ����ִ����ײ�ص�
                    if(pBlk[0]->crashCall){
                        pBlk[0]->crashCall(pBlk[0], pBlk[1]);
                    }
                    if(pBlk[1]->crashCall){
                        pBlk[1]->crashCall(pBlk[1], pBlk[0]);
                    }
                }else{
                    //���Ӳֵ����ͬ����ִ��ӲֵС����ײ�ص�
                    if(pBlk[0]->crashVal > pBlk[1]->crashVal) {
                        if(pBlk[1]->crashCall){
                            pBlk[1]->crashCall(pBlk[1],pBlk[0]);
                        }
                    }
                    else{
                        if(pBlk[0]->crashCall){
                            pBlk[0]->crashCall(pBlk[0],pBlk[1]);
                        }
                    }
                }            
#endif 
                return pBlkTmpCnt; //ֻ����һ����ײ
            }
            
        }
    }

    return pBlkTmpCnt;
}

/*************************************************************************************
* ��������: scree_process 
* ����˵��: ...
* �������: screen_t * pScr
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int screen_process(screen_t * pScr)
{
    struct list_head_linux * pos = NULL;
    block_t * pBlk = NULL;

    //�������ϵĿ��ӡ����Ļ�ϣ�Ȼ����ʾ
    list_for_each(pos, &pScr->blockList){
        pBlk = container_of(pos, block_t, list);
        screen_printBlock(pScr, pBlk);

        //������Ϊ�ӵ����͵Ĵ�����ײ
        if(BLOCK_IS_TANK_BULLET(pBlk)){
            BlockCrashCheck = screen_BlockCrashCheck(pScr, pBlk);
        }

    }
    screen_statusShow(pScr, pScr->fpsVal);
    screen_show(pScr);
}

int scree_test(void)
{
    int i = 0, j = 0, xPost = 0, yPost = 0;
    time_t timeVal = 0;
    screen_t * pScr = screen_init(150,40);
    block_t * pTank = tank_creat(0,0,1);
    block_t * pTanks[10] = {0};

    screen_listAddBlock(pScr, pTank);

    for(i=0; i<10; i++){
        xPost = rand() % 140;
        yPost = rand() % 40;
        pTanks[i] = tank_creat(xPost,yPost,0);
        screen_listAddBlock(pScr, pTanks[i]);
    }
    timeVal = cpu_timebase_get();
    while(1){
        if(cpu_timebase_diff_ms(timeVal)>pScr->flushTime){
            //screen_clear(pScr,FALSE);
            timeVal = cpu_timebase_get();
            #if 0
            screen_printBlock(pScr, pTank);
            screen_printBlock(pScr, pTank2); 
            screen_show(pScr);
            #else
            screen_process(pScr);
            #endif
        }
        console_process();
    }
    
    screen_close(pScr);
}

#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

