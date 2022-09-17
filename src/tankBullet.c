/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : tankBullet.c
  Author       : xiamj
  Created      : 2022/9/12
  Description  : 

  History      :
  1.Date        : 2022/9/12
    Author      : xiamj
    Modification: Created file
    tank的武器
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


/*----------------------------------------------------------------------------------*/
/*locals                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*funs                                                                              */
/*----------------------------------------------------------------------------------*/

/*************************************************************************************
* 函数名称: tankBullet_move 
* 函数说明: 武器在屏幕上移动
* 输入参数: block_t * pTank
*           void *arg
*           void *timerNode
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int tankBullet_move(block_t * pTankBullet, int isMaster,void *timerNode)
{
    int x_dst = 0, y_dst = 0;
    if(isMaster){
        x_dst = pTankBullet->x_coor + 1;
    }else{
        x_dst = pTankBullet->x_coor - 1;
    }
    y_dst = pTankBullet->y_coor;
    
    return block_move(pTankBullet, x_dst, y_dst);
}


/*************************************************************************************
* 函数名称: tankBullet_died 
* 函数说明: 武器消失的条件
* 输入参数: block_t * pTank
*           void *timerNode
* 输出参数: 无
* 返 回 值: TRUE:条件成立 FLSE 条件不成立
* 备     注: 无
*************************************************************************************/
int tankBullet_died(block_t * pTankBullet,void *timerNode)
{
    int ret = FALSE;


    //判断是否删除快
    if( (pTankBullet->x_coor == 0) ||                           //到达左边界
        (pTankBullet->x_coor+pTankBullet->x_len >=SCREEN_X_LEN))//到达右边界
    {
        if(pTankBullet->privateData){
            free(pTankBullet->privateData);
        }
        timer_eventRm(&pTankBullet->timerNode[0]);
        screen_listRmBlock(pTankBullet);
        ret = TRUE;
    }
    return ret;
}


/*************************************************************************************
* 函数名称: tankBullet_Creat 
* 函数说明: tank武器创建
* 输入参数: int type：武器类型
*           int x 初始化坐标
*           int y
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int tankBullet_Creat(block_t * pTank, void* isMaster, KEY_ID KYE_ID)
{
    int x = 0, y = 0, type = 0, speed = 0;
    block_t * ptankBullet = block_creat(4, 4);  //x轴4个字符，y轴4个字符
    tankArg_t * ptankArg = pTank->privateData;
    tankArg_t * ptankBulletArg = NULL;
    int * pBulletCnt = NULL;

    type  = KYE_ID;
    //检查子弹量是否充足
    if(isMaster){
        switch(type){            
            case KEY_K:
                pBulletCnt = &ptankArg->bulletCnt[BULLET_TYPE2_K];
                break;
            case KEY_L:
                pBulletCnt = &ptankArg->bulletCnt[BULLET_TYPE3_L];
                break;
            default:
                pBulletCnt = &ptankArg->bulletCnt[BULLET_DEFAULT];
        }
        if(*pBulletCnt < 1){
            return ERROR;
        }

        *pBulletCnt -= 1;
    }
    
    ptankBulletArg = malloc(sizeof(tankArg_t));
    memcpy(ptankBulletArg, ptankArg, sizeof(tankArg_t));  //子弹的参数继承tank的参数
    ptankBullet->privateData = ptankBulletArg;    
    ptankBullet->crashVal = 100;   
    
    speed = ptankArg->armSpeed;
    if(isMaster){
        ptankBullet->blockType = BLOCK_TANK_BULLET;
        x = pTank->x_coor + 2;
    }else{
        type = -1;
        ptankBullet->blockType = BLOCK_TANK_BULLET_FOE;
        x = pTank->x_coor - 1;
    }
    
    y = pTank->y_coor + pTank->y_len/2;
    
    switch(type){
        case KEY_J:
            ptankArg->attackVal = 10;
            block_draw(ptankBullet,0,0,"*");
            y -= 1;
            break;
        case KEY_K:
            ptankArg->attackVal = 40;
            block_draw(ptankBullet,0,0,"=");
            block_draw(ptankBullet,0,1,"=");
            block_draw(ptankBullet,0,2,"=");
            y -= 2;
            break;
        case KEY_L:
            ptankArg->attackVal = 100;
            block_draw(ptankBullet,0,0,"####");
            block_draw(ptankBullet,0,1,"####");
            block_draw(ptankBullet,0,2,"####");
            y -= 2;
            break;
        default:
            ptankArg->attackVal = 1;
            block_draw(ptankBullet,0,0,"-");
    }
    block_move(ptankBullet, x, y);

    screen_listAddBlock(GET_SCREEN_PTR(), ptankBullet);
    TIMER_EVNET_ADD(&ptankBullet->timerNode[0],speed,tankBullet_move,ptankBullet,(void*)isMaster,tankBullet_died,ptankBullet);
}


#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

