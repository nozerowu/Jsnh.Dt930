#include <stdio.h>
#include <string.h>
#include "itron.h"
#include "cmndef.h"
#include "bios1mac.h"

void Wait2(UW);
void MainMenu(void);
void FNC_Init( void );   /* 1996.11.12 casio */
int getkey(void);
void Work(void);
void clear(void);



unsigned char oper[7];
extern UB oper[7];


void ap_start( void )
{
    Wait2( 5L );
    s_sound( 2250, 5 );
    Wait2( 5L );
    s_sound( 2250, 5 );
    FNC_Init();
    lcd_cls();               /*  screen clear 1996.11.26.casio */
    obr_change( 0 );            /*  obr_change( OBR_ALL )   */
    while(1)
    {

        MainMenu();

    }
    exit( 0 );                      /*  application end */
}



void MainMenu(void)
{

    unsigned char msg[][22]={"欢迎使用资产采集系统",
                             "",
                             "by onepc 153785587",
                             "",
                             "1.采集 2.上传 3.删除"};
   short x,y,cnt,code;
   int i;
   H fhnd;

    while (1)
    {
         key_clear();
         lcd_cls();
         for( x=0,y=1,cnt=0; cnt<5; y+=3,cnt++ )
            {
                {
                   lcd_csr_put( y, x );
                   cputstr(LCD_ATTR_NORMAL,msg[cnt]);
                }
             }
        /**/
     code=getkey();
     if(code==0x0c)
         break;
     if(code >= '1' && code <= '3')
        code -= 0x30;
        switch(code)
        {
            case 1:
                Work();
                break;
            case 2:
                if(( fhnd = open("data.txt", O_RDWR ))== E_LOWERR )
                {
                    lcd_cls();
                    lcd_csr_put( 1, 0 );
                    cputstr(LCD_ATTR_NORMAL,"无采集数据! ");
                    getkey();
                    break;
                }
                close(fhnd);
                lcd_cls();
                lcd_csr_put( 1, 0 );
                cputstr(LCD_ATTR_NORMAL,"将采集器放在通讯座上");
                lcd_csr_put( 5, 5 );
                cputstr(LCD_ATTR_NORMAL,"按任意键...");
                getkey();
                sendfile();
                /*lcd_cls();
                lcd_csr_put( 5, 5 );
                cputstr(LCD_ATTR_NORMAL,"按任意键删除文件");
                getkey();
                clear();*/
                break;
            case 3:
                lcd_cls();
                /*cputstr(LCD_ATTR_NORMAL,"删除[ENT] 取消[CLR]");*/
                clear();
                break;
        }



    }


}

/******************************************************************************
**     Routine name: FNC_Initialize                                          **
**     Detail      : function key initialize set (return default)            **
**     Parameter   : None                                                    **
**     Return value: None                                                    **
**     Author      : casio                                                   **
******************************************************************************/
void FNC_Init( void )
{
        KEYFORM        kcode;

    kcode.attr = 0x0;
    kcode.code = 0x2d;
        key_fnc( FNC_SET, FNC_1, &kcode);
    kcode.attr = 0x0;
    kcode.code = 0x1d;
        key_fnc( FNC_SET, FNC_2, &kcode);
    kcode.attr = 0x0;
    kcode.code = 0x1c;
        key_fnc( FNC_SET, FNC_3, &kcode);
    kcode.attr = 0x0;
    kcode.code = 0x10;
        key_fnc( FNC_SET, FNC_4, &kcode);
    kcode.attr = 0x0;
    kcode.code = 0x20;
        key_fnc( FNC_SET, FNC_5, &kcode);
    kcode.attr = 0xff;
    kcode.code = 0x0;
        key_fnc( FNC_SET, FNC_6, &kcode);
    kcode.attr = 0xff;
    kcode.code = 0x1;
        key_fnc( FNC_SET, FNC_7, &kcode);
    kcode.attr = 0xff;
    kcode.code = 0x02;
        key_fnc( FNC_SET, FNC_8, &kcode);

}



/******************************************************************************
**     Routine name: Wait2                                                   **
**     Detail      : specified second wait routine 2 (31.25ms)               **
**     Parameter   : tmcnt   : timer count                                   **
**     Return value: none                                                    **
**     Author      : Fuji Software Inc.                                      **
******************************************************************************/
void Wait2( UW   tmcnt )
{
ER      ercd;
UW      p_**;

    ercd = s_settimer2( FL_TM2_INT_ID, FL_TM2_INT_ITU0, tmcnt );
    wai_**( &p_**, FL_TM2_INT_ID, FL_TM2_INT_ITU0, TWF_ANDW | TWF_CLR );
    s_timerend2( ercd );
}

int getkey(void)
 {
   KEY_INP    kinp;
   int code;

    kinp.ext = KEY_OBR_EXT;
    kinp.echo = ECHO_OFF;
    kinp.font_size = LCD_ANK_LIGHT;
    kinp.type = LCD_ATTR_NORMAL;
    kinp.column_pos =0;
    kinp.line_pos = 2;
        code=key_read(&kinp);
        return (code);
  }


  void Work(void)
  {
    H fmode,fhnd;
    ER ret;
    char dnname[10],dnbar[32],disbar[32],cr[3],allbind[100],splitdh[2],dissize[3]; /*filename[11],*/
    int i,j;
    /*W fp;*/
    cr[0]=0x0d;
    cr[1]=0x0a;
    cr[2]=0x0;
    splitdh[0]=0x2c;
    splitdh[1]=0x0;
    /*memset(filename,0x0,11);*/
    /*memcpy(filename,"data.txt",10);*/

      while(1)
      {
        lcd_cls();
        lcd_csr_put( 3, 0 );
        cputstr(LCD_ATTR_NORMAL,"请扫描员工号六位:");
        memset(oper,0x0,7);
        i=read_str(0,5,0,oper,7);
        if(i==1)
        continue;
        if(i==E_KEY_CLR)
            return;
        if((strlen(oper)==0)||(strlen(oper)>6))
        continue;
        if(strlen(oper)<6)
        {
            for(i=strlen(oper);i<6;i++)
            {
                oper[i]=0x20;
            }
        }

             while(1)
              {
                lcd_cls();
                lcd_csr_put( 3, 0 );
                memset(dnbar,0x0,32);
                cputstr(LCD_ATTR_NORMAL,"条码:");
                i=read_str(0,5,0,dnbar,32);
                if(i==1)
                continue;
                if (strlen(dnbar)==0)
                {
                    continue;
                }
                 memset(allbind,0x0,100);
                    memcpy(allbind,oper);
                    strcat(allbind,dnname);

                    if((fhnd=open("data.txt",O_CREAT | O_RDWR | O_APPEND ))==E_LOWERR)
                    {
                            lcd_cls();
                            lcd_csr_put( 1, 0 );
                            cputstr(LCD_ATTR_NORMAL,"文件打开错误!");
                                         return;
                    }
                    lseek(fhnd,0L,2);
                    write(fhnd,allbind,99);
                    write(fhnd,cr,2);
                    Wait2( 5L );
                    close(fhnd);

                if(i==E_KEY_CLR)
                break; /*扫描成功则跳出循环*/

              }



                 /*    if(j==E_KEY_CLR)
                     {
                        break;
                     }else
                    {
                     continue;
                     }*/




      /**/
      }
  }


void clear(void)
{
    int k;
    lcd_csr_put( 7,0);
    cputstr(LCD_ATTR_NORMAL,"删除[ENT] 取消[CLR]");
    k=getkey();
    if(k==0x0d)
    {
        /*memset(oper,0x0,7);*/
        dat_fdel("data.txt");
        dat_fdel("galan.txt");
    }
    return;
}
/*
void Submenu(short attr)
{
    short code;
    while(1)
    {
        lcd_cls();
        Scan(attr);
        break;
    }
}

void Scan(short attr)
{
    H fmode,fhnd;
    ER ret;
    char dnname[10],dnbar[32],disbar[32],cr[3],filename[11];
    int num,i,j;
    W fp;
    cr[0]=0x0d;
    cr[1]=0x0a;
    cr[2]=0x0;
    memset(filename,0x0,11);
    memset(filename,"galanz.txt",10);

    input computer name
    while(1)
    {
        lcd_cls();
        lcd_csr_put( 3, 0 );
        cputstr(LCD_ATTR_NORMAL,"请输电脑名称(不能超过9位):");
        memset(dnname,0x0,10););
        i=read_str(0,5,2,dnname,10);
        if(i==E_KEY_CLR)
            break;
        if((strlen(dnname)==0)||(strlen(dnname)>9))
        continue;
    }
}
*/

