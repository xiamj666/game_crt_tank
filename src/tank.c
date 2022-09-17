/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : tank.c
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
#define FOE_MAX     30
#define BLOCK_TANK_X_LEN        4
#define BLOCK_TANK_Y_LEN        4
/*----------------------------------------------------------------------------------*/
/*extern                                                                            */
/*----------------------------------------------------------------------------------*/
typedef struct{
    int tankFoeNum;         //�з�tank����
    int score;              //�÷�
    time_t lastCrashTime;   //�ϴ���ײ������ʱ��
    int gameHardLeve;       //��Ϸ�Ѷȵȼ���ÿ20��+1
    screen_t * pScr;
    block_t *pTank;
}tankGameArg_t;
#define GET_GAME_SCORE()        (gTankGameArg.score)        //��ȡ��Ϸ����
#define GET_GAME_HARD_LEVE()    (gTankGameArg.gameHardLeve) //��ȡ��Ϸ�Ѷ�
/*----------------------------------------------------------------------------------*/
/*globals                                                                           */
/*----------------------------------------------------------------------------------*/
tankGameArg_t gTankGameArg = {0};


/*----------------------------------------------------------------------------------*/
/*locals                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*funs                                                                              */
/*----------------------------------------------------------------------------------*/
int tank_move(block_t * pTank, void *arg,KEY_ID keyID);
int tank_RandMove(block_t * pTank, void *arg,void *timerNode);


/*************************************************************************************
* ��������: tank_creat 
* ����˵��: ����һ��tank��������ʼ��������
* �������: int x
*           int y
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
block_t * tank_creat(int x, int y, int isMaster)
{
    block_t * pTank = block_creat(BLOCK_TANK_Y_LEN, BLOCK_TANK_X_LEN);  //x��4���ַ���y��4���ַ�
    tankArg_t * pTankArg = (tankArg_t *)malloc(sizeof(tankArg_t));

    memset(pTankArg, 0, sizeof(pTankArg));
    pTankArg->armType = 1;
    pTankArg->healthVal = TANK_HEALTH_MAX;
    pTank->privateData = pTankArg;
    pTank->crashVal = 1;
    pTank->crashCall = (crashCallFun)tank_crashCall;
    if(isMaster){
        pTank->blockType = BLOCK_TANK;
        pTankArg->armSpeed = 10;
        pTankArg->isMaster = TRUE;

        //Ĭ�ϵ���
        pTankArg->bulletCnt[BULLET_DEFAULT] =  99;
        pTankArg->bulletCnt[BULLET_TYPE2_K] =  BULLET_TYPE2_K_MAX;
        pTankArg->bulletCnt[BULLET_TYPE3_L] =  BULLET_TYPE3_L_MAX;
        pTankArg->healthSelfHealing = 1;
        
        block_draw(pTank,0,0,"### ");
        block_draw(pTank,0,1,"  ##");
        block_draw(pTank,0,2,"### ");
        block_draw(pTank,0,3,tank_getHealthStr(pTank));
        block_move(pTank, x, y);
        console_evetRegister(KEY_A, (evntCall)tank_move, pTank, NULL);
        console_evetRegister(KEY_S, (evntCall)tank_move, pTank, NULL);
        console_evetRegister(KEY_W, (evntCall)tank_move, pTank, NULL);
        console_evetRegister(KEY_D, (evntCall)tank_move, pTank, NULL);
        
        console_evetRegister(KEY_J, (evntCall)tankBullet_Creat, pTank, (void*)TRUE);
        console_evetRegister(KEY_K, (evntCall)tankBullet_Creat, pTank, (void*)TRUE);
        console_evetRegister(KEY_L, (evntCall)tankBullet_Creat, pTank, (void*)TRUE);
    }else{
#if 1      
        int gameLeve = GET_GAME_HARD_LEVE();
        pTankArg->armSpeed  =     100 - (rand() % 4+gameLeve)*5;       //�ӵ��ƶ��ٶ�
        pTankArg->moveSpeed =     1000-  (rand() % 4+gameLeve)*200;     //�л��ƶ��ٶ�
        pTankArg->armlaunchSpeed =2000 - (rand() % 4+gameLeve)*100;     //�ӵ������ٶ�

        if(pTankArg->armSpeed < 1) { pTankArg->armSpeed = 1; }
        if(pTankArg->moveSpeed < 200){ pTankArg->moveSpeed = 200; }
        if(pTankArg->armlaunchSpeed < 200){ pTankArg->armlaunchSpeed = 200;}
#else
        pTankArg->armSpeed  = 1000;                         //�ӵ��ƶ��ٶ�
        pTankArg->moveSpeed = 2000;                         //�л��ƶ��ٶ�
        pTankArg->armlaunchSpeed = 3000;                    //�ӵ������ٶ�
#endif
        pTankArg->isMaster = FALSE;
        pTank->blockType = BLOCK_TANK_FOE;
        block_draw(pTank,0,0," ###");
        block_draw(pTank,0,1,"##  ");
        block_draw(pTank,0,2," ###");
        block_draw(pTank,0,3,tank_getHealthStr(pTank));
        block_move(pTank, x, y);

        //�ƶ��ٶȣ�ռ�õл�tank���͵�0~1 timer node
        TIMER_EVNET_ADD(&pTank->timerNode[0],pTankArg->moveSpeed,tank_RandMove,pTank,NULL,NULL,NULL);
        TIMER_EVNET_ADD(&pTank->timerNode[1],pTankArg->armlaunchSpeed,
                    tankBullet_Creat,pTank,(void*)FALSE,tankBullet_died,pTank);
    }
    return pTank;
}

/*************************************************************************************
* ��������: tank_destroy 
* ����˵��: ����һ��tank
* �������: block_t * pTank
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_destroy(block_t * pTank)
{
    argcheck(pTank == NULL, "pTank == NULL", ERROR);
    
    timer_eventRm(&pTank->timerNode[0]);
    timer_eventRm(&pTank->timerNode[1]);
    screen_listRmBlock(pTank);
    if(pTank->privateData){
        free(pTank->privateData);
    }
    return block_destroy(pTank);
}

/*************************************************************************************
* ��������: tank_getHealthStr 
* ����˵��: ��ȡtank����ֵ���ַ���
* �������: block_t * pTank
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
char * tank_getHealthStr(block_t * pTank)
{
    static char healthMsg[64] = {0};
    tankArg_t * pTankArg = NULL;
    argcheck(pTank == NULL, "pTank == NULL", "ERR");
    int healthVal = 0;
    
    memset(healthMsg, 0, sizeof(healthMsg));
    pTankArg = pTank->privateData;

    if(pTankArg){
        healthVal = pTankArg->healthVal;
        
        if(healthVal > 99) healthVal = 99;
        if(healthVal < 0) healthVal = 0;
        sprintf(healthMsg,"H:%2d",healthVal);
    }else{
        sprintf(healthMsg,"H:--");
    }
    if(strlen(healthMsg) > BLOCK_TANK_X_LEN){
        PERROR("�ַ�����.");
        return "ERR";
    }
//  printf("strlen healthMsg:%ld.\n",strlen(healthMsg));
    return healthMsg;
}

/*************************************************************************************
* ��������: tank_flushHealthMsg 
* ����˵��: ˢ��tank����ֵ��Ϣ
* �������: block_t * pTank
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_flushHealthMsg(block_t * pTank)
{
    block_draw(pTank,pTank->x_coor,pTank->y_coor+3,tank_getHealthStr(pTank));
}
/*************************************************************************************
* ��������: tank_move 
* ����˵��: �ƶ�tank
* �������: block_t * pTank
*           int x_dst
*           int y_dst
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_move(block_t * pTank, void *arg,KEY_ID keyID)
{
    int x_dst = 0, y_dst = 0;
    x_dst = pTank->x_coor;
    y_dst = pTank->y_coor;
    switch(keyID){
        case KEY_W:
            y_dst = pTank->y_coor - 1;
            break;
        case KEY_S:
            y_dst = pTank->y_coor + 1;
            break;
        case KEY_A:
            x_dst = pTank->x_coor - 1;
            break;
        case KEY_D:
            x_dst = pTank->x_coor + 1;
            break;
    }
//  PERROR("==%d===\n",1);
    return block_move(pTank, x_dst, y_dst);
}

/*************************************************************************************
* ��������: tank_RandMove 
* ����˵��: tank����ƶ�
* �������: block_t * pTank
*           void *arg
*           KEY_ID keyID
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_RandMove(block_t * pTank, void *arg,void *timerNode)
{
    int x_dst = 0, y_dst = 0;
    x_dst = pTank->x_coor;
    y_dst = pTank->y_coor;
    KEY_ID keyID = rand() % 5;

    //��ǰ���Ķ�һЩ
    if(keyID >= KEY_D){
        keyID = KEY_A;
    }
    switch(keyID){
        case KEY_W:
            y_dst = pTank->y_coor - 1;
            break;
        case KEY_S:
            y_dst = pTank->y_coor + 1;
            break;
        case KEY_A:
            x_dst = pTank->x_coor - 1;
            break;
        case KEY_D:
            x_dst = pTank->x_coor + 1;
            break;
    }
    return block_move(pTank, x_dst, y_dst);
}

/*************************************************************************************
* ��������: tank_crashCall 
* ����˵��: tank�鷢����ײ�Ļص�
* �������: block_t * pTank
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_crashCall(block_t * pTank, block_t * pPullet)
{
    
    tankGameArg_t * pGame = &gTankGameArg;
    tankArg_t * ptankArg = pTank->privateData;
    tankArg_t * pPulletArg = pPullet->privateData;

    argcheck(pTank==NULL, "pTank==NULL", ERROR);
    argcheck(pPullet==NULL, "pPullet==NULL", ERROR);
   
    //�з��ӵ������л�,�ҷ��ӵ������Լ�
    if(pTank->blockType + pPullet->blockType == BLOCK_TANK_BULLET_FOE + BLOCK_TANK_FOE||
        pTank->blockType + pPullet->blockType == BLOCK_TANK_BULLET + BLOCK_TANK){
        return OK;
    }  

    //����N ms���ظ��е��޵У������ӵ��ܼ���ʱ������
    if(ptankArg && ptankArg->isMaster){
        if(cpu_timebase_diff_ms(pGame->lastCrashTime) > 85){
            pGame->lastCrashTime = cpu_timebase_get();
        }else{
            return OK;  //���ʱ��С��N ms���������˺�
        }

    }

    //������ֵ
    if(ptankArg && pPulletArg)
    {
        //fix�л�������Ϊ1
        if(pPullet->blockType == BLOCK_TANK_BULLET_FOE){
            pPulletArg->attackVal = 1;
        }
        
        ptankArg->healthVal -= pPulletArg->attackVal;
        tank_flushHealthMsg(pTank);
        
        if(ptankArg->healthVal < 0){ 
            
            //game over�ж�
            if(ptankArg && ptankArg->isMaster){
#if 0           //�����޵�   ��TODO ���Ըĳ�������ķ�ʽ   
                return OK;
#else
//              PERROR("healthVal:%d %d type:%d",
//                  ptankArg->healthVal, pPulletArg->attackVal,pPullet->blockType);
                screen_gameOverPrint(GET_SCREEN_PTR());
#endif 
            }
            pGame->score++; //�÷�++
            pGame->gameHardLeve = pGame->score/10;
            tank_destroy(pTank);
        }
    }


    return OK;
}

/*************************************************************************************
* ��������: tank_creatFoeTank 
* ����˵��: �����л�����
* �������: block_t * pTank
*           void *arg
*           void *timerNode
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_creatFoeTank(tankGameArg_t * pGame , void * arg, void *timerNode)
{
    block_t * pTanks[FOE_MAX] = {0};
    int i = 0, j = 0, xPost = 0, yPost = 0, foeNumTmp = 0;

    foeNumTmp = screen_numOfBlockType(pGame->pScr, BLOCK_TANK_FOE);
    foeNumTmp = pGame->tankFoeNum-foeNumTmp;
    foeNumTmp += GET_GAME_HARD_LEVE();
    
    for(i=0; i<foeNumTmp; i++){
        xPost = (pGame->pScr->x_len - 30) + (rand() % 5) * 5; 
        yPost = (rand() % 10)*4;
        pTanks[i] = tank_creat(xPost,yPost,0);
        screen_listAddBlock(pGame->pScr, pTanks[i]);
    }
    return OK;
}


/*************************************************************************************
* ��������: tank_selfHealing 
* ����˵��: tank ���һ�Ѫ
* �������: block_t * pTank
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_selfHealing(block_t * pTank,void * arg, void *timerNode)
{
    tankArg_t * ptankArg = pTank->privateData;
    if(ptankArg->healthVal < TANK_HEALTH_MAX){
        
        ptankArg->healthVal += ptankArg->healthSelfHealing;

        if(ptankArg->healthVal >= TANK_HEALTH_MAX){
            ptankArg->healthVal = TANK_HEALTH_MAX;
        }
    }
    if(ptankArg->healthVal < 50){
        ptankArg->healthSelfHealing = 3;
    }else{
        ptankArg->healthSelfHealing = 1;
    }
    tank_flushHealthMsg(pTank);
    return OK;
}

/*************************************************************************************
* ��������: tank_bulletAutoFill 
* ����˵��: �����Զ����
* �������: block_t * pTank
*           void * arg
*           void *timerNode
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_bulletAutoFill(block_t * pTank,void * arg, void *timerNode)
{
    static int runCnt = 0;
    
    tankArg_t * ptankArg = pTank->privateData;
    runCnt++;
    
    ptankArg->bulletCnt[BULLET_DEFAULT] = BULLET_DEFAULT_MAX;
    ptankArg->bulletCnt[BULLET_TYPE2_K] += 5;
    if(runCnt % 1 == 0){
        ptankArg->bulletCnt[BULLET_TYPE3_L] += 1;
    }

    //�߽��ж�
    if(ptankArg->bulletCnt[BULLET_TYPE2_K] > BULLET_TYPE2_K_MAX)
        ptankArg->bulletCnt[BULLET_TYPE2_K] = BULLET_TYPE2_K_MAX;
    if(ptankArg->bulletCnt[BULLET_TYPE3_L] > BULLET_TYPE3_L_MAX)
        ptankArg->bulletCnt[BULLET_TYPE3_L] = BULLET_TYPE3_L_MAX;
}

/*************************************************************************************
* ��������: tank_gameStatusPrint 
* ����˵��: ��Ϸ״̬��ʾ
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
void tank_gameStatusPrint(void)
{
    block_t * pTank = gTankGameArg.pTank;
    tankArg_t * ptankArg = pTank->privateData;
    
    printf("״̬:  �����ָ�����:%d/s ����: j:%2d k:%2d l:%2d.  ",
        ptankArg->healthSelfHealing,
        ptankArg->bulletCnt[BULLET_DEFAULT],
        ptankArg->bulletCnt[BULLET_TYPE2_K],
        ptankArg->bulletCnt[BULLET_TYPE3_L]);
    printf("����:%d ��Ϸ�Ѷ�:%d �л�����:%d.\n",GET_GAME_SCORE(),
        GET_GAME_HARD_LEVE(),
        screen_numOfBlockType(GET_SCREEN_PTR(), BLOCK_TANK_FOE));
    
}


/*************************************************************************************
* ��������: tank_gameStart 
* ����˵��: tank��Ϸ��ʼ
* �������: screen_t * pScr
*           int foeNum �� �л�����
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int tank_gameStart(screen_t * pScr, int foeNum)
{
    block_t * pTank = NULL;
    block_t * pTanks[FOE_MAX] = {0};
    int i = 0, j = 0, xPost = 0, yPost = 0;
    tankGameArg_t * pGame = &gTankGameArg;
    
    argcheck(foeNum>FOE_MAX, "foeNum>FOE_MAX", ERROR);
    argcheck(pScr==NULL, "pScr==NULL", ERROR);
    
    pTank=tank_creat(0,0,1);
    screen_listAddBlock(pScr, pTank);

    pGame->tankFoeNum = foeNum;
    pGame->pScr = pScr;
    pGame->pTank = pTank;
    pScr->gameStatPrint = tank_gameStatusPrint;

    //�����Դ����з�tank
    TIMER_EVNET_ADD(&pTank->timerNode[0],1000,tank_creatFoeTank,pGame,NULL,NULL,NULL);
    //���������һ�Ѫ
    TIMER_EVNET_ADD(&pTank->timerNode[1],1000, tank_selfHealing,pTank,NULL,NULL,NULL);

    //�����ӵ����
    TIMER_EVNET_ADD(&pTank->timerNode[2],1000, tank_bulletAutoFill,pTank,NULL,NULL,NULL);
    return OK;
}



#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

