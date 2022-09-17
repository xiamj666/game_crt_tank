/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : console.c
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
#include <sys/ioctl.h>
#include <termios.h>


/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/
#define STD_IN      0

typedef struct{
    KEY_ID  keyID;
    evntCall callFun;
    void *   callArg1;
    void *   callArg2;
}key_evet_arg_t;

typedef struct{
    KEY_ID keyID;
    int keyValNum;      //描述keyVal的有效个数
    char keyVal[4];
}key_map_t;
/*----------------------------------------------------------------------------------*/
/*extern                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*globals                                                                           */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*locals                                                                            */
/*----------------------------------------------------------------------------------*/

static key_map_t keymap[] = {
    {KEY_A,     1,  {0x61}},
    {KEY_S,     1,  {0x73}},
    {KEY_W,     1,  {0x77}},
    {KEY_D,     1,  {0x64}},

    {KEY_J,     1,  {0x6a}},
    {KEY_K,     1,  {0x6b}},
    {KEY_L,     1,  {0x6c}},
    {KEY_I,     1,  {0x69}},
    {KEY_O,     1,  {0x6f}},

    {KEY_UP,    3,  {0x1b,0x5b,0x41}},
    {KEY_DOWN,  3,  {0x1b,0x5b,0x42}},
    {KEY_LEFT,  3,  {0x1b,0x5b,0x43}},
    {KEY_RIGHT, 3,  {0x1b,0x5b,0x44}},

    {KEY_ESC,       1,  {0x1b}},
    {KEY_ENTER,     1,  {0xa}},
    {KEY_BACKAPSE,  1,  {0xa}},
    {KEY_SPACE,     1,  {0x20}},

    {KEY_ANY,       1,  {0}},
};
static key_evet_arg_t keyEvntArg[KEY_MAX] = {0};

/*----------------------------------------------------------------------------------*/
/*funs                                                                              */
/*----------------------------------------------------------------------------------*/


/*************************************************************************************
* 函数名称: console_init 
* 函数说明: 初始化控制台
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int console_init(void)
{
    static int initFlag = ERROR;
    if(initFlag == ERROR){
        //设置及时从终端获取字符串，关闭回显
        struct termios termios_opt;
        tcgetattr(STD_IN, &termios_opt);
        termios_opt.c_lflag &= ~ICANON;     //关闭回车才能读数据
        termios_opt.c_lflag &= ~ECHO;       //关闭回显
        tcsetattr(STD_IN,TCSANOW,&termios_opt);

        memset(&keyEvntArg, 0, sizeof(keyEvntArg));
        initFlag = OK;
    }
    return OK;
}

/*************************************************************************************
* 函数名称: console_getVal 
* 函数说明: 从控制台获取1个字符
* 输入参数: char * outVal 输出参数
*           int timeOutUs 超时时间
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int console_getVal(char * outVal, int timeOutUs)
{
    int val = 0;
    time_t timeOut = 0;
    timeOut = cpu_timebase_get();
 
    do{
        if(ioctl(STD_IN,FIONREAD,&val)<0){
            perror("ioctl");
            return ERROR;
        }
        if(val>0){
            break;
        }
        if(cpu_timebase_diff_ms(timeOut) > timeOutUs){
            return ERROR;
        }
    }while(1);
    
    if(val>0){
        read(STD_IN,outVal,1);
    }

    return OK;
}

/*************************************************************************************
* 函数名称: console_getVals 
* 函数说明: 获取多个字符
* 输入参数: char * outVal：输出参数
* 输出参数: 无
* 返 回 值: 成功返回获取的个数，失败返回-1
* 备     注: 无
*************************************************************************************/
int console_getVals(char * outVal)
{
    int i = 0, num = 0;
    char val = 0;
    while(1){
        if(console_getVal(&val,0) < 0){
            break;
        }
//      printf("%x.\n",val);
        outVal[num] = val;
        num++;
    }

    return num;
}

/*************************************************************************************
* 函数名称: console_keyMapCheck 
* 函数说明: 键盘映射检查
* 输入参数: KEY_ID keVal
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
static int console_keyMapCheck(KEY_ID keVal)
{
    int i = 0;
    int keyMapNum = sizeof(keymap)/sizeof(keymap[0]);
    for(i=0; i<keyMapNum; i++){
        if(keVal == keymap[i].keyID){
            return OK;
        }

    }

    return ERROR;
}

/*************************************************************************************
* 函数名称: console_evetRegister 
* 函数说明: 控制台事件注册
* 输入参数: KEY_ID keVal
*           evntCall callfun
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int console_evetRegister(KEY_ID keVal, evntCall callfun, void * callArg1,void * callArg2)
{
    console_init();
    if(console_keyMapCheck(keVal) <0){
        PERROR("key val:%d invaid.\n", keVal);
    }

    argcheck(callfun==NULL, "callfun==NULL", ERROR);

    keyEvntArg[keVal].callFun = callfun;
    keyEvntArg[keVal].callArg1 = callArg1;
    keyEvntArg[keVal].callArg2 = callArg2;
    return OK;
}


/*************************************************************************************
* 函数名称: console_process 
* 函数说明: 控制台处理
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int console_process(void)
{
    char keyVal[32] = {0};
    int i = 0, keyValNum = 0, keyId = 0;
    int keyMapNum = sizeof(keymap)/sizeof(keymap[0]);
    key_evet_arg_t * keyArg = NULL;
    
    if( (keyValNum=console_getVals(keyVal))  <= 0){
        return OK;
    }
    //printf("keyValNum:%d :%x %x %x %x\n",keyValNum, keyVal[0],keyVal[1],keyVal[2],keyVal[3]);
    //查找键值回调参数，先通过键值找键ID，然后再找键参数
    for(i=0; i<keyMapNum; i++){
        if(memcmp(keyVal, keymap[i].keyVal, keyValNum) == 0){
            keyId = keymap[i].keyID;
            keyArg = &keyEvntArg[keyId];
        }
    }
    //击中回调
    if(keyArg && keyArg->callFun ){
        keyArg->callFun(keyArg->callArg1, keyArg->callArg2,keyId);
    }

    //任意键回调
    keyArg = &keyEvntArg[KEY_ANY];
    if(keyArg && keyArg->callFun ){
        keyArg->callFun(keyArg->callArg1, keyArg->callArg2,keyId);
    }
}

int console_defaultevntCall(void * arg1, void * arg2,KEY_ID keyId)
{
    printf("key id:%d.\n",keyId);
}

/*************************************************************************************
* 函数名称: console_test 
* 函数说明: 输入测试
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
int console_test(void)
{
    char val = 0;
    console_init();

    console_evetRegister(KEY_A, console_defaultevntCall, NULL,NULL);
    console_evetRegister(KEY_S, console_defaultevntCall, NULL,NULL);
    console_evetRegister(KEY_W, console_defaultevntCall, NULL,NULL);
    console_evetRegister(KEY_D, console_defaultevntCall, NULL,NULL);
    
    while(1){
#if 0
        if(console_getVal(&val,0) == OK){
            printf("val:%c 0x%x.\n",val, val);
        }
#else
        console_process();
#endif
    }

}
#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

