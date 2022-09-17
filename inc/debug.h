/************************************************************************************

  Copyright (c) 2022,  ...
  All rights reserved.
*************************************************************************************
  File Name    : debug.h
  Author       : xiamj
  Created      : 2022/9/8
  Description  : 

  History      :
  1.Date        : 2022/9/8
    Author      : xiamj
    Modification: Created file

*************************************************************************************/
#ifndef __DEBUG_H__
#define __DEBUG_H__
#ifdef __cplussplus
extern "C"{
#endif /*end ifdef __cplussplus*/


/*----------------------------------------------------------------------------------*/
/*defines                                                                           */
/*----------------------------------------------------------------------------------*/
/*带颜色的调试*/
#define BULE                "\033[34m"
#define FMT_NONE            "\033[m"
#define LIGHT_RED           "\033[1;31m"

#define COLOR_NONE          "\e[0m"
#define BLACK               "\e[0;30m"
#define L_BLACK             "\e[1;30m"
#define RED                 "\e[0;31m"
#define L_RED               "\e[1;31m"
#define GREEN               "\e[0;32m"
#define L_GREEN             "\e[1;32m"
#define BROWN               "\e[0;33m"
#define YELLOW              "\e[1;33m"
#define BLUE                "\e[0;34m"
#define L_BLUE              "\e[1;34m"
#define PURPLE              "\e[0;35m"
#define L_PURPLE            "\e[1;35m"
#define CYAN                "\e[0;36m"
#define L_CYAN              "\e[1;36m"  
#define GRAY                "\e[0;37m"
#define WHITE               "\e[1;37m"

#define BOLD                "\e[1m"
#define UNDERLINE           "\e[4m"
#define BLINK               "\e[5m"
#define REVERSE             "\e[7m"
#define HIDE                "\e[8m"
#define CLEAR               "\e[2J"                //请屏幕
#define CLRLINE             "\r\e[K"               //or "\e[1K\r"

#undef CTRL
#define CTRL(c)             ((c) & (UINT8)~0x60)

/*----------------------------------------------------------------------------------*/
/*extern                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*globals                                                                           */
/*----------------------------------------------------------------------------------*/
extern unsigned int g_errCnt;

/*----------------------------------------------------------------------------------*/
/*locals                                                                            */
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/*funs                                                                              */
/*----------------------------------------------------------------------------------*/
#define PDGB(fmt,args...) do{ \
    printf("["WHITE"DGB"FMT_NONE"]:<%s(%d)> "fmt"\n",__func__,__LINE__,##args);\
}while(0)

#define PWARNING(fmt, args...)do {\
    printf("["YELLOW"WAR"FMT_NONE"]:<%s(%d)> "fmt"\n", __func__, __LINE__, ##args); \
}while(0)

#define PERROR(fmt,args...) do{ \
    printf("["LIGHT_RED"ERR"FMT_NONE"%d]:<%s(%d)> "fmt"\n",\
        g_errCnt++,__func__,__LINE__,##args);\
    while(1);\
}while(0)

#define PERROR_HANG(fmt,args...) do{ \
    printf("["LIGHT_RED"ERR"FMT_NONE"%d]:<%s(%d)> "fmt"\n",\
        g_errCnt++,__func__,__LINE__,##args);\
    while(1);\
}while(0)


/*************************************************************************************
* 函数名称: argcheck 
* 函数说明: 参数检查
* 输入参数: isFalse
*           retVal
* 输出参数: 无
* 返 回 值: 成功返回0，失败返回非0
* 备     注: 无
*************************************************************************************/
#define RETURN_VOID             //返回值为void时使用这个
#define argcheck(isTrue,printMsg,retVal) do{    \
    if(isTrue){                              \
        PERROR_HANG("argCheck error:%s.",printMsg);   \
        return retVal;                           \
    }                                            \
}while(0);




#ifdef __cplussplus
}
#endif /*end ifdef __cplussplus*/

#endif /*end ifndef __DEBUG_H__*/
