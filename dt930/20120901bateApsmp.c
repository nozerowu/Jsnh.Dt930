
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
H  Lenth,KeyLen,COM=COM0;
ER  fn,sumcount=0;

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
          {  close(fup);
             lcd_cls();
             sumcount  =  0;
             Dsp_str("删除?",1,0);
             Dsp_str("1.删除",3,0);
             Dsp_str("2.保留",5,0);
             code=getkey();
             switch(code)
             {
              case '1':
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
   UB datim[20],scount[8],head[40],bph[40],lsh[40],ygh[10],ErrStr[6],tpstr[40],hgnr[43];
   UB qxdm[7],TopStr[4],qx_h4w[6],qxnr[30];
   H  w=8,g=8,o=8;       /*  workshop,group,operator的最大位数   */
   UB barcode[25],record[25],data[130],barcodes[100],ch[30];
   H  codelen=24,lmt=6;  /*  codelen--成品码的最大长度； lmt--缺陷码的长度*/
   W  lno,lno2;
   ER fm,ret,Status,i,i1,j,k,fn2,fn3,flag1;

   struct          tim_tabl        tim_dat;
   struct          day_tabl        day_dat;
   ER                      retcd;
   UB                      buff[8],rqsj1[20];
   H code;


   Status=0;
   i=0;
   flag1=1;

   while(1)
   {
        lcd_cls();
        j=0;

        memset(ygh,0x0,sizeof(ygh));
        sprintf(ygh,"%d",sumcount);
        Dsp_str("单号:",1,0);
        Dsp_str("个数: 0",7,0);
        Dsp_str("合计",7,11);
        Dsp_str(ygh,7,18);


        memset(bph,0x0,sizeof(bph));
        ret=read_str(3,0,0,bph,18);
        if(ret==E_KEY_CLR) break;
        strtake(head,bph,0,strlen(bph));
        j=0;

        while(1)
        {
            lcd_cls();

            memset(scount,0x0,sizeof(scount));
            sprintf(scount,"%d",j);
            memset(ygh,0x0,sizeof(ygh));
            sprintf(ygh,"%d",sumcount);
            Dsp_str("单号:",1,0);
            Dsp_str(head,1,7);

            Dsp_str("个数:",7,0);
            Dsp_str(scount,7,6);
            Dsp_str("合计:",7,9);
            Dsp_str(ygh,7,15);

            memset(bph,0x0,sizeof(bph));
            ret=read_str(3,0,0,bph,40);
            if(ret==E_KEY_CLR) break;
            strtake(lsh,bph,0,strlen(bph));
            if (strlen(bph)>3)
            {
                j++;
                sumcount++;

                write(fn,head,strlen(head));
                k  =  strlen(head);
                while(k<32)
                {
                    write(fn," ",1);
                    k++;
                }
                write(fn,lsh,strlen(lsh));
                write(fn,"\x0d\x0a",2);
                continue;
            }
            else
            continue;

        }
   }



 }




