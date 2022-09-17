/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : console.h
  Author       : xiamj
  Created      : 2022/9/8
  Description  : 

  History      :
  1.Date        : 2022/9/8
    Author      : xiamj
    Modification: Created file

*************************************************************************************/
#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#ifdef __cplussplus
extern "C"{
#endif /*end ifdef __cplussplus*/


/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/
typedef enum{
    KEY_A = 0,
    KEY_W,
    KEY_S,
    KEY_D,
    KEY_B,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_I,
    KEY_O,
    KEY_ESC,
    KEY_ENTER,
    KEY_BACKAPSE,
    KEY_SPACE,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,   //×ó
    KEY_RIGHT,  //ÓÒ
    KEY_ANY,    //ÈÎÒâ¼ü
    KEY_MAX,
}KEY_ID;

typedef int (*evntCall)(void * arg1,void*arg2,KEY_ID KYE_ID);

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

int console_init(void);
int console_getVal(char * outVal, int timeOutUs);
int console_getVals(char * outVal);
int console_process(void);
int console_evetRegister(KEY_ID keVal, evntCall callfun, void * callArg1,void * callArg2);

int console_test(void);


#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

#endif /*end ifndef __CONSOLE_H__*/
