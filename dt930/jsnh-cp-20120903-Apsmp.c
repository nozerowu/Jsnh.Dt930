
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
void collectCK(void);
void Initial(void);
void setting(void);

/*----------------------*
** Global Variable     **
*-----------------------*/

UB fgoods[20],fqxdz[20],fchuku[20];
UB allcode[1000][16];
H  Lenth,KeyLen,COM  =  COM0;
ER  fn,sumcount  =  0,a  =  0;

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
   disp_set.font_md = FONT6_SET;
   dat_system( SYSD_FNC_WRITE, SYSD_DSP, &disp_set );

   for( x = 0,y = 1,cnt = 0; cnt < 5; y += 2,cnt++ )
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
        if(( fup = open(fgoods, O_RDONLY))== E_LOWERR )
    	{
	   	/*lcd_cls();*/
		Dsp_str("文件不存在...",1,0);
		/*Wait2(15L);*/
		close(fup);

    	} else
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
               if(( fn = open(fgoods, O_CREAT | O_RDWR))== E_LOWERR )
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
		return; /* No Files */
    	}

 }


void setting()
{
   H code;
   lcd_cls();
   Dsp_str("PC端通讯口:",1,0);
   Dsp_str("1:COM1",3,0);
   Dsp_str("2:COM2",5,0);
   while(1){
     code=getkey();
     if(code=='1'||code=='2') break;
   }
   if(code=='1') COM=COM0;
   else
     COM=COM1;
}

/*采集数据*/
void collect(void)
 {
   UB numbs[8],customer[40],strbuff[40],barcode[40],total[10],neednub[40];
   ER ret,ineednub,icount,k,m,turn;
   H code;

   while(1)
   {
        lcd_cls();
        icount  =  0;
        turn  =  0;

        memset(total,0x0,sizeof(total));
        sprintf(total,"%d",sumcount);
        Dsp_str("单号:",1,0);
        Dsp_str("个数: 0",7,0);
        Dsp_str("合计",7,11);
        Dsp_str(total,7,18);


        memset(strbuff,0x0,sizeof(strbuff));
        ret=read_str(3,0,0,strbuff,18);
        if(ret==E_KEY_CLR) break;
        strtake(customer,strbuff,0,strlen(strbuff));

        lcd_cls();
        Dsp_str("输入要扫描的件数",1,0);

        memset(strbuff,0x0,sizeof(strbuff));
        ret=read_str(3,0,0,strbuff,40);
        if(ret==E_KEY_CLR) break;
        strtake(neednub,strbuff,0,strlen(strbuff));
        ineednub  =  atoi(neednub);


        while(1)
        {
            if (ineednub  ==  0)
            break;
            turn =0;
            lcd_cls();
            memset(numbs,0x0,sizeof(numbs));
            sprintf(numbs,"%d",j);
            memset(total,0x0,sizeof(total));
            sprintf(total,"%d",sumcount);
            Dsp_str("单号:",1,0);
            Dsp_str(customer,1,7);

            Dsp_str("个数:",7,0);
            Dsp_str(numbs,7,6);
            Dsp_str("合计:",7,9);
            Dsp_str(total,7,15);

            memset(strbuff,0x0,sizeof(strbuff));
            ret  =  read_str(3,0,0,strbuff,40);
            if(ret  ==  E_KEY_CLR) break;
            strtake(barcode,strbuff,0,strlen(strbuff));

            if (a>0)
            {
                m  =  a;
                while(m  >  0)
                {
                     if (strcmp(allcode[m-1],barcode) == 0)
                     {
                        lcd_cls();
                        Dsp_str("注意：有同号！",1,0);
                        Dsp_str("继续扫描！：",3,0);
                        lcd_el(1);
                        s_sound(3370,12);
                        lcd_el(0);
                        lcd_el(1);
                        lcd_el(0);
                        lcd_el(1);
                        lcd_el(0);
                        turn  =  1;
                        break;
                     }
                     m--;
                     continue;
                }
            }

            if (turn  == 1) continue;

            if (strlen(strbuff)>3)
            {
                icount++;
                sumcount++;
                ineednub--;
                strtake(allcode[a],strbuff,0,strlen(strbuff));
                a++;

                write(fn,customer,strlen(customer));
                k  =  strlen(customer);
                while(k<32)
                {
                    write(fn," ",1);
                    k++;
                }
                write(fn,barcode,strlen(barcode));
                write(fn,"\x0d\x0a",2);
                if (ineednub == 0)
                {
                    lcd_cls();
                    Dsp_str("扫描完毕",1,0);
                    Dsp_str("按任意键扫下一家：",3,0);
                    s_sound(3370,12);
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




