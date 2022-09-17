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
* 函数名称: screen_open 
* 函数说明: 打开一个屏幕
* 输入参数: int xLen 屏幕的x长度
*           int yLen y长度
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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

    pScr->flushTime = 20;               //刷新时间20ms
    INIT_LIST_HEAD(&pScr->blockList);
    //填充默认数据
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
* 函数名称: screen_close 
* 函数说明: 关闭屏幕
* 输入参数: screen_t *pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int screen_close(screen_t *pScr)
{
    free(pScr);
}

/*************************************************************************************
* 函数名称: screen_print 
* 函数说明: 屏幕打印
* 输入参数: int x
*           int y
*           int len
*           char * str
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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
* 函数名称: screen_printBlock 
* 函数说明: 在屏幕中打印块
* 输入参数: screen_t * pScr
*           block_t * pBlk
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int screen_printBlock(screen_t * pScr, block_t * pBlk)
{
    int x = 0, y = 0;
    argcheck(pScr==NULL, "pScr==NULL", ERROR);
    argcheck(pBlk==NULL, "pBlk==NULL", ERROR);

    for(y=0; y<pScr->y_len; y++){
        for(x=0; x<pScr->x_len; x++){
            if(pBlk->data[y][x] != 0){  //只要是不为0就拷贝过去
                pScr->data[y][x] = pBlk->data[y][x];
            }
        }
    }
    return OK;
}


/*************************************************************************************
* 函数名称: screen_clear 
* 函数说明: 清屏幕
* 输入参数: screen_t * pScr
            isCrtCler : 是否刷新crt的内容
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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
* 函数名称: screen_gameStatusShow 
* 函数说明: 游戏状态显示
* 输入参数: screen_t * pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
void screen_gameStatPrint(screen_t * pScr)
{
    if(pScr->gameStatPrint){
        pScr->gameStatPrint();
    }
}

/*************************************************************************************
* 函数名称: screen_statusShow 
* 函数说明: 在右上角显示状态信息
* 输入参数: screen_t * pScr
*           int fpsVal
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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
* 函数名称: screen_show 
* 函数说明: 显示屏幕
* 输入参数: screen_t * pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int screen_show(screen_t * pScr)
{
    int i = 0,j = 0;
    static time_t timeVal = 0;
    static int  fpsCnt = 0;
    argcheck(pScr==NULL, "pScr==NULL", ERROR);
        
    //打印首行
    printf("┌");
    for(i=0; i<pScr->x_len; i++){
        printf("-");
    }
    printf("┐\n");

    //打印数据
    for(j=0; j<pScr->y_len; j++){
        printf("|%s|\n",pScr->data[j]);
    }
    
    //打印末行
    printf("└");
    for(i=0; i<pScr->x_len; i++){
        printf("-");
    }
    printf("┘\n");
    fflush(stdout);

    //FPS统计
    fpsCnt++;
    if(cpu_timebase_diff_ms(timeVal)>500){
        timeVal = cpu_timebase_get();
        pScr->fpsVal = fpsCnt;
        fpsCnt = 0;
    }
    
    screen_gameStatPrint(pScr);
    screen_cursor_move(0,0);        //将光标放在初始化的位置上
    screen_clear(pScr,FALSE);
    
    return OK;
}

/*************************************************************************************
* 函数名称: screen_flush 
* 函数说明: 屏幕刷新
* 输入参数: screen_t * pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int screen_flush(screen_t * pScr)
{
    screen_clear(pScr,TRUE);
    screen_process(pScr);
}


/*************************************************************************************
* 函数名称: screen_perfTest 
* 函数说明: 屏幕性能测试
* 输入参数: screen_t * pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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
        
            sprintf(buff,"屏幕大小:%dX%d.",pScr->x_len,pScr->y_len);
            xPos = strlen(buff) < pScr->x_len ? 
                    (pScr->x_len-strlen(buff))/2 : 4;
            screen_printStr(pScr,xPos,pScr->y_len/2-1,buff);
            
            memset(buff, 0, sizeof(buff));            
            sprintf(buff,"屏幕刷新率:%d次/s.",cnt);
            xPos = strlen(buff) < pScr->x_len ? 
                    (pScr->x_len-strlen(buff))/2 : 4;
            screen_printStr(pScr,xPos,pScr->y_len/2,buff);

            memset(buff, 0, sizeof(buff));
            sprintf(buff,"打印速度:%dk/s.",cnt*pScr->y_len*pScr->x_len/1024);
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
* 函数名称: screen_gameOverPrint 
* 函数说明: 游戏结束打印
* 输入参数: screen_t * pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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
* 函数名称: scree_listAddBlock 
* 函数说明: 将块添加到屏幕列表中
* 输入参数: screen_t * pScr
*           block_t * pBlk
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int screen_listAddBlock(screen_t * pScr, block_t * pBlk)
{
    list_add(&pBlk->list, &pScr->blockList);
}

/*************************************************************************************
* 函数名称: screen_listRmBlock 
* 函数说明: 将块从屏幕列表中删除
* 输入参数: block_t * pBlk
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int screen_listRmBlock(block_t * pBlk)
{
    list_del(&pBlk->list);
}

/*************************************************************************************
* 函数名称: screen_listNum 
* 函数说明: 获取屏幕中的block个数
* 输入参数: screen_t * pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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
* 函数名称: screen_numOfBlockType 
* 函数说明: 获取在屏幕中块类型的数量
* 输入参数: screen_t * pScr
*           BLOCK_TYPE type
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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
* 函数名称: screen_crashCheck 
* 函数说明: 快碰撞检测
* 输入参数: screen_t * pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
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

            //在同一个坐标上有2个以上的快重合
            if(pBlkTmpCnt >= 2){
#if 1
                //PERROR("x:%d y:%d crashVal:%d %d.\n",xIndex,yIndex, pBlk[0]->crashVal , pBlk[1]->crashVal);
                if(pBlk[0]->crashVal == pBlk[1]->crashVal){
                    //如果硬值相同，都执行碰撞回调
                    if(pBlk[0]->crashCall){
                        pBlk[0]->crashCall(pBlk[0], pBlk[1]);
                    }
                    if(pBlk[1]->crashCall){
                        pBlk[1]->crashCall(pBlk[1], pBlk[0]);
                    }
                }else{
                    //如果硬值不相同，则执行硬值小的碰撞回调
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
                return pBlkTmpCnt; //只处理一次碰撞
            }
            
        }
    }

    return pBlkTmpCnt;
}

/*************************************************************************************
* 函数名称: scree_process 
* 函数说明: ...
* 输入参数: screen_t * pScr
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int screen_process(screen_t * pScr)
{
    struct list_head_linux * pos = NULL;
    block_t * pBlk = NULL;

    //将链表上的快打印到屏幕上，然后显示
    list_for_each(pos, &pScr->blockList){
        pBlk = container_of(pos, block_t, list);
        screen_printBlock(pScr, pBlk);

        //块类型为子弹类型的处理碰撞
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

