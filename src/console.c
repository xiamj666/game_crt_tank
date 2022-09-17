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
    int keyValNum;      //����keyVal����Ч����
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
* ��������: console_init 
* ����˵��: ��ʼ������̨
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
*************************************************************************************/
int console_init(void)
{
    static int initFlag = ERROR;
    if(initFlag == ERROR){
        //���ü�ʱ���ն˻�ȡ�ַ������رջ���
        struct termios termios_opt;
        tcgetattr(STD_IN, &termios_opt);
        termios_opt.c_lflag &= ~ICANON;     //�رջس����ܶ�����
        termios_opt.c_lflag &= ~ECHO;       //�رջ���
        tcsetattr(STD_IN,TCSANOW,&termios_opt);

        memset(&keyEvntArg, 0, sizeof(keyEvntArg));
        initFlag = OK;
    }
    return OK;
}

/*************************************************************************************
* ��������: console_getVal 
* ����˵��: �ӿ���̨��ȡ1���ַ�
* �������: char * outVal �������
*           int timeOutUs ��ʱʱ��
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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
* ��������: console_getVals 
* ����˵��: ��ȡ����ַ�
* �������: char * outVal���������
* �������: ��
* �� �� ֵ: �ɹ����ػ�ȡ�ĸ�����ʧ�ܷ���-1
* ��     ע: ��
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
* ��������: console_keyMapCheck 
* ����˵��: ����ӳ����
* �������: KEY_ID keVal
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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
* ��������: console_evetRegister 
* ����˵��: ����̨�¼�ע��
* �������: KEY_ID keVal
*           evntCall callfun
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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
* ��������: console_process 
* ����˵��: ����̨����
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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
    //���Ҽ�ֵ�ص���������ͨ����ֵ�Ҽ�ID��Ȼ�����Ҽ�����
    for(i=0; i<keyMapNum; i++){
        if(memcmp(keyVal, keymap[i].keyVal, keyValNum) == 0){
            keyId = keymap[i].keyID;
            keyArg = &keyEvntArg[keyId];
        }
    }
    //���лص�
    if(keyArg && keyArg->callFun ){
        keyArg->callFun(keyArg->callArg1, keyArg->callArg2,keyId);
    }

    //������ص�
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
* ��������: console_test 
* ����˵��: �������
* �������: ��
* �� �� ֵ: �ɹ�����0��ʧ�ܷ��ط�0
* ��     ע: ��
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

