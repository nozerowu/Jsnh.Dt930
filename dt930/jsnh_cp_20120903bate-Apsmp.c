
/*-----------------------*
**   Header File        **
*-----------------------*/
#include <stdio.h>
#include <string.h>
#include "itron.h"
#include "cmndef.h"
#include "bios1mac.h"

/*----------------------*
**  Prototype define   **
*----------------------*/
void MenuOpening( void );
void work(void);
void collect(void);
/*      void collectCK(void);       */
void Initial(void);

/*----------------------*
** Global Variable     **
*-----------------------*/

UB fgoods[20];
UB allcode[1000][16];
H  Lenth,  KeyLen,  COM  =  COM0;
ER  fn,  sumcount  =  0,a  =  0;

extern  H COM;

/******************************************************************************
**     Routine name: apstart                                                **
**     Detail      : Start routine                                           **
**     Parameter   : None                                                    **
**     Return value: None                                                    **
**     Author      : Fuji Software Inc.                                      **
******************************************************************************/
void ap_start( void )
{
    Wait2( 5L );                /* 等时 --casio      */
    s_sound( 2250, 5 );         /* 发声 --casio      */
    Wait2( 5L );
    s_sound( 2250, 5 );
    FNC_Init();                 /*  功能键设置       */
    lcd_cls();                  /*  清屏 --casio     */
    MenuOpening();              /*  显示开机信息--clce */
    Wait2( 20L );
    obr_change(0);              /*  设置可读条码的种类 */
    work();                     /*  主要工作函数       */
    exit(0);                    /*  应用程序结束       */
}

/******************************************************************************
**     Routine name: MenuOpening                                            **
**     Detail      : opening message search and display                      **
**     Parameter   : none                                                    **
**     Return value: none                                                    **
**     Author      : Fuji Software Inc.                                      **
******************************************************************************/
void MenuOpening( void )
{
   unsigned char    msg[][20]={
                    "  南昌仓库   ",
                    "  2012-09 ",
                    " 条码采集系统  ",
                    "  by QQ 3921342",
                    "北京金色农华"};
   short          cnt, x, y;
   DAT_DSP_STR    disp_set;

   /*－－－－－设置显示字体为12点阵－－－－－*/
   dat_system( SYSD_FNC_READ, SYSD_DSP, &disp_set );
   disp_set.font_md  =  FONT6_SET;
   dat_system( SYSD_FNC_WRITE, SYSD_DSP, &disp_set );

   for( x  =  0,y  =  1,cnt  =  0; cnt  <  5; y  +=  2,cnt++  )
   Dsp_str(msg[cnt],y , x);   /*--在屏幕坐标(y,x)处显示字符串msg[cnt]--*/
}

/*选择菜单*/
void work(void)
 {
    H  x,  y,  cnt,  code;
    ER  fup;
    unsigned char msg[][25]={"南昌仓库条码系统",
                             " 1:数据采集",
                             " 2:数据上传",
                             " 3:退出"
                           };

    Initial();        /*----- 初始化 ------*/
    for(  ;  ;)
    {

     lcd_cls();
     for( x  =  0,y  =  1,cnt  =  0; cnt  <  4; y  +=  2,cnt++ )
     Dsp_str(msg[cnt],  y,  x);
     code  =  getkey();   /*----  等待击键  -----*/

     switch (code)
      {
       case '1':
            collect();              	/*--- 数据采集 ----*/
            break;
       case '2':
        close(fn);
        sendfile(fgoods);    	 /*-- 上传文件与下载文件    ----*/
        if(( fup  =  open(fgoods, O_RDONLY))  ==   E_LOWERR )
    	{
	   	/*lcd_cls();*/
		Dsp_str("文件不存在...",1,0);
		/*Wait2(15L);*/
		close(fup);

    	}
    	else
        {
            close(fup);
            lcd_cls();
            sumcount  =  0;
            a  =  0;
            memset(allcode,0x0,sizeof(allcode));

            Dsp_str("删除?",1,0);
            Dsp_str("1.删除",3,0);
            Dsp_str("2.保留",5,0);
            code=getkey();
            switch(code)
            {
            case '1':
                close(fn);
                dat_fdel(fgoods); /*上传结束后删除该文件*/

                break;
            default:
                break;
            }
        }
                /*上传结束后删除该文件*/
        if(( fn  =  open(fgoods, O_CREAT | O_RDWR))  ==  E_LOWERR )
    	{
            lcd_cls();
            Dsp_str("File Open Error!",0,1);
            Wait2(15L);
            close(fn);
            return; /* No Files */
    	}

            break;
        case '3':
            break;
        default:
            break;
       }
    }
 }


void Initial(void)
  {
   Lenth  =  55;
   KeyLen  =  5;
   strcpy(fgoods,"wenjian.txt");

     if(( fn = open(fgoods, O_CREAT | O_RDWR))== E_LOWERR )
    	{
            lcd_cls();
            Dsp_str("File Open Error!",0,1);
            Wait2(15L);
            close(fn);
            return;                 /* No Files */
    	}

 }


/*采集数据*/

/*  *********************************************


    dh_heji：单号件数  danhao：单号     buff：   临时
    tiaoma :  条形码   heji  : 总件数   neednub: 打算扫描数量


  ***********************************************  */
void collect(void)
 {
   UB dh_heji[8],danhao[40],buff[40],tiaoma[40],heji[10],neednub[40];
   ER ret,  i,  j,  k,  m,  turn;
   H code;

   while(1)
   {
        lcd_cls();
        j  =  0;
        turn  =  0;

        memset(heji,  0x0,  sizeof(heji));
        sprintf(heji,  "%d",  sumcount);
        Dsp_str("单号:",  1,  0);
        Dsp_str("个数: 0",  7,  0);
        Dsp_str("合计",  7,  11);
        Dsp_str(heji,  7,  18);


        memset(buff,  0x0,  sizeof(buff));
        ret=read_str(3,  0,  0,  buff,  18);
        if(ret  ==  E_KEY_CLR)  break;
        strtake(danhao,   buff,  0,  strlen(buff));

        lcd_cls();
        Dsp_str("输入要扫描的件数",  1,  0);

        memset(buff,  0x0,  sizeof(buff));
        ret=read_str(3,  0,  0,  buff,  40);
        if(ret  ==  E_KEY_CLR)  break;
        strtake(neednub,  buff,  0,  strlen(buff));
        i  =  atoi(  neednub);


        while(1)
        {
            if (i  ==  0)
            break;

            lcd_cls();
            memset(dh_heji,  0x0,  sizeof(dh_heji));
            sprintf(dh_heji,  "%d",  j);
            memset(heji,  0x0,  sizeof(heji));
            sprintf(heji,  "%d",  sumcount);

            Dsp_str("单号:",  1,  0);
            Dsp_str(danhao,  1,  7);

            if (j  >  1)
            {   if (turn  == 1)
                {
                    Dsp_str("上一条码同号！",  7,  0);
                }
                else
                {
                    Dsp_str("上一条码:",  7,  0);
                    Dsp_str(tiaoma,  7,  11);
                }

            }

            Dsp_str("个数:",  9,  0);
            Dsp_str(dh_heji,  9,  6);
            Dsp_str("合计:",  9,  9);
            Dsp_str(heji,  9,  15);

            memset(buff,  0x0,  sizeof(  buff));
            ret  =  read_str(3,  0,  0,  buff,   40);
            if(ret  ==  E_KEY_CLR) break;
            strtake(tiaoma,  buff,  0,  strlen(buff));

            turn  =  0;

            if (a  >  0)
            {
                m  =  a;
                while(m  >  0)
                {
                     if (strcmp(allcode[m-1],  tiaoma)  ==  0)
                     {
                        lcd_cls();
                        Dsp_str("注意：有同号！",  1,  0);
                        Dsp_str("按任意键继续：",  3,  0);
                        lcd_el(1);
                        s_sound(500,  20);
                        lcd_el(0);
                        lcd_el(1);
                        turn  =  1;
                        break;
                     }
                     m--;
                     continue;
                }
            }

            if (turn  ==  1) continue;

            if (strlen(buff)  >  3)
            {
                j++;
                sumcount++;
                i--;
                strtake(allcode[a],  buff,  0,  strlen(buff));
                a++;

                write(fn,  danhao,  strlen(danhao));
                k  =  strlen(danhao);
                while(k  <  32)
                {
                    write(fn,  " ",  1);
                    k++;
                }
                write(fn,  tiaoma,  strlen(tiaoma));
                write(fn,  "\x0d\x0a",  2);
                if (i  ==  0)
                {
                    lcd_cls();
                    Dsp_str("扫描完毕",  1,  0);
                    Dsp_str("按任意键扫下一家：",  3,  0);
                    s_sound(3370,  12);
                    lcd_el(1);
                    code=getkey();
                    switch(code)
                    {
                        default:
                        lcd_el(0);
                        break;
                    }


            }
                continue;
            }
            else
            continue;


        }
   }



 }




