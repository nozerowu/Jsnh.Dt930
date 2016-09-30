#include <stdio.h>
#include <string.h>
#include <math.h>
#include "itron.h"
#include "cmndef.h"
#include "bios1mac.h"
#include "clce800.h" 
#include "k_point2.h"

void Wait2(UW);
void MenuOpening( void );
void MainMenu(void);
void FNC_Init( void );   /* 1996.11.12 casio */
void Work(void);
void Submenu(short attr);
void Scan(short attr);
void WriteEnd(void);
int getkey(void);
void UploadFile( void );
void ReadDataFile(void);
void ClearData(void);
void submenu_cfwzcj(void);

void ReadDataFile(void)
{
		char r[100],r1[100];
		H fmode;
		H fhnd;
		lcd_cls();
    if (( fhnd = open("data.txt", O_RDWR  ))== E_LOWERR )
    {
        fmode=O_CREAT|O_RDWR;
        if (( fhnd = open("data.txt", fmode ))== E_LOWERR )
        {
            
            lcd_csr_put( 1, 0 );
            cputstr(LCD_ATTR_NORMAL,"文件打开错误! ");
            getkey();
            return;
        }
    }
		read(fhnd,r,90);
		if(strlen(r)>0)
		{
		    lcd_cls();
        lcd_csr_put( 1, 0 );
        cputstr(LCD_ATTR_NORMAL,"data.txt");
        lcd_csr_put( 3, 0 );
        cputstr(LCD_ATTR_NORMAL,r);
		}
		else
		{
		    lcd_cls();
        lcd_csr_put( 1, 0 );
        cputstr(LCD_ATTR_NORMAL,"没有东东");
		}
		close(fhnd);
		getkey();
		lcd_cls();
		
    if (( fhnd = open("dulp1.txt", O_RDWR  ))== E_LOWERR )
    {
        fmode=O_CREAT|O_RDWR;
        if (( fhnd = open("dulp1.txt", fmode ))== E_LOWERR )
        {

            lcd_csr_put( 1, 0 );
            cputstr(LCD_ATTR_NORMAL,"文件打开错误! ");
            getkey();
            return;
        }
    }
		read(fhnd,r1,90);
		if(strlen(r1)>0)
		{
		    lcd_cls();
		        lcd_csr_put( 1, 0 );
		        cputstr(LCD_ATTR_NORMAL,"dulp1.txt");
        lcd_csr_put( 3, 0 );
        cputstr(LCD_ATTR_NORMAL,r1);
		}
		else
		{
		    lcd_cls();
        lcd_csr_put( 1, 0 );
        cputstr(LCD_ATTR_NORMAL,"没有东东");
		}
		close(fhnd);
		getkey();
}

/******************************************************************************
**     Routine name: apstart                                                **
**     Detail      : Start routine                                           **
**     Parameter   : None                                                    **
**     Return value: None                                                    **
**     Author      : Fuji Software Inc.                                      **
******************************************************************************/
void ap_start( void )
{
    Wait2( 5L );
    s_sound( 2250, 5 );
    Wait2( 5L );
    s_sound( 2250, 5 );
    FNC_Init();
    lcd_cls();               		/*  screen clear 1996.11.26.casio */
    obr_change( 0 );            	/*  obr_change( OBR_ALL )   */
    while (1)
    {
        MenuOpening();              	/*  display opening message 2009 orifound */
        getkey();
        MainMenu();
    }
    exit( 0 );                      /*  application end */
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
    KEYFORM		kcode;

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
**     Routine name: MenuOpening                                            **
**     Detail      : opening message search and display                      **
**     Parameter   : none                                                    **
**     Return value: none                                                    **
**     Author      : Fuji Software Inc.                                      **
******************************************************************************/
void MenuOpening( void )
{
    short   cnt, x, y;
    DAT_DSP_STR	disp_set;
    dat_system( SYSD_FNC_READ, SYSD_DSP, &disp_set );
    disp_set.font_md = FONT6_SET;
    dat_system( SYSD_FNC_WRITE, SYSD_DSP, &disp_set );
    lcd_cls();
    lcd_csr_put( 1, 0 );
    cputstr(LCD_ATTR_WIDTH,"欢迎使用");
    lcd_csr_put( 3, 0 );
    cputstr(LCD_ATTR_NORMAL,"工商企业档案");
    lcd_csr_put( 5, 0 );
    cputstr(LCD_ATTR_NORMAL,"条形码采集系统 V1.0");
    lcd_csr_put( 9, 8 );
    cputstr(LCD_ATTR_NORMAL,"2009 ORIFOUND");
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
    ER ercd;
    UW p_flg;

    ercd = s_settimer2( FL_TM2_INT_ID, FL_TM2_INT_ITU0, tmcnt );
    wai_flg( &p_flg, FL_TM2_INT_ID, FL_TM2_INT_ITU0, TWF_ANDW | TWF_CLR );
    s_timerend2( ercd );
}

int getkey(void)
{
    KEY_INP	kinp;
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

void MainMenu()
{
    short x,y,cnt,code;
    int i;
		H fhnd;
		
    while (1)
    {
        key_clear();
        lcd_cls();
        lcd_csr_put( 1, 0 );
        cputstr(LCD_ATTR_WIDTH,"主功能菜单");
        lcd_csr_put( 3, 0 );
        cputstr(LCD_ATTR_NORMAL,"1:存放位置采集");
        lcd_csr_put( 5, 0 );
        cputstr(LCD_ATTR_NORMAL,"2:上传采集数据");

        lcd_csr_put( 9, 0 );
        cputstr(LCD_ATTR_NORMAL,"        请按键选择...");

        code=getkey();
        if (code==0x0c)
            break;
        if (code >= '1' && code <= '9')
            code -= 0x30;
        switch (code)
        {
        case 1:
            submenu_cfwzcj();
            break;
        case 2:
						UploadFile();
						break;
				/*
				case 3:
						ReadDataFile();
            break;
        case 4:
						ClearData();
        		break;
        */
        default:
            break;
        }
    }
}
/******************************************************************************
**     Routine name: submenu_cfwzcj                                          **
**     Detail      : 子菜单																	                 **
**     Parameter   : tmcnt   : timer count                                   **
**     Return value: none                                                    **
**     Author      : Fuji Software Inc.                                      **
******************************************************************************/
void submenu_cfwzcj(void)
{
    short code;

		while(1)
		{
       	key_clear();
        lcd_cls();
        lcd_csr_put( 1, 0 );
        cputstr(LCD_ATTR_WIDTH,"存放位置采集");
        lcd_csr_put( 3, 0 );
        cputstr(LCD_ATTR_NORMAL,"1:开始采集");
        lcd_csr_put( 5, 0 );
        cputstr(LCD_ATTR_NORMAL,"2:清除数据");

        lcd_csr_put( 9, 0 );
        cputstr(LCD_ATTR_NORMAL,"        请按键选择...");

        code=getkey();
        if (code==0x0c)
            break;
        if (code >= '1' && code <= '9')
            code -= 0x30;
        switch (code)
        {
		        case 1:
		            Scan(1);
		            break;
		        case 2:
								ClearData();
								break;
		        default:
		            break;
        }
		}
}

/******************************************************************************
**     Routine name: ClearData                                               **
**     Detail      :清除数据																	               **
**     Parameter   : tmcnt   : timer count                                   **
**     Return value: none                                                    **
**     Author      : Fuji Software Inc.                                      **
******************************************************************************/
void ClearData()
{
		H fhnd;
		short i;
		short flag=0;
		
		key_clear();
		lcd_cls();
    lcd_csr_put( 1, 0 );
    cputstr(LCD_ATTR_NORMAL,"您确定要删除数据吗?");
    lcd_csr_put( 5, 0 );
    cputstr(LCD_ATTR_NORMAL,"[ENT是 CLR否]        ");
    lcd_csr_put( 9, 0 );
    cputstr(LCD_ATTR_NORMAL,"        请按键选择...");
    
    while (((i=getkey())!=0x0c)&&(i!=0x0d));
    if (i==0x0d)
    {   
		    key_clear();
		    lcd_cls();

				fil_remove("data.txt");
				fil_remove("dulp1.txt");

		    if (( fhnd = open("data.txt", O_RDWR  ))== E_LOWERR )
		    {
		        flag++;
		    }
		    if (( fhnd = open("dulp1.txt", O_RDWR  ))== E_LOWERR )
		    {
		        flag++;
		    }
		    
		    if(flag==2)
		    {
				    lcd_csr_put( 1, 0 );
				    cputstr(LCD_ATTR_NORMAL,"删除成功!");
		    }
		    else
	    	{
				    lcd_csr_put( 1, 0 );
				    cputstr(LCD_ATTR_NORMAL,"删除失败!");
	    	}
		    
		    lcd_csr_put( 9, 0 );
		    cputstr(LCD_ATTR_NORMAL,"按任意键继续...");
				getkey();

    }
}

/******************************************************************************
**     Routine name: UploadFile                                              **
**     Detail      : specified second wait routine 2 (31.25ms)               **
**     Parameter   : none									                                   **
**     Return value: none                                                    **
**     Author      : liming						                                       **
******************************************************************************/
void UploadFile( void )
{
		int ret;
		H fhnd;

    if (( fhnd = open("data.txt", O_RDWR  ))== E_LOWERR )
    {
        lcd_cls();
        lcd_csr_put( 1, 0 );
        cputstr(LCD_ATTR_NORMAL,"无采集数据! ");
        getkey();
        return;
    }
    close(fhnd);

    lcd_cls();
    lcd_csr_put( 1, 0 );
    cputstr(LCD_ATTR_NORMAL,"将采集器放在通讯座上");
    lcd_csr_put( 3, 0 );
    cputstr(LCD_ATTR_NORMAL,"按任意键...");
    getkey();
    
    lcd_cls();

    sendfile("data.txt");
    getkey();
}
/******************************************************************************
**     Routine name: Scan                                                    **
**     Detail      : 扫描条码     														               **
**     Parameter   : none									                                   **
**     Return value: none                                                    **
**     Author      : liming						                                       **
******************************************************************************/
void Scan(short attr)
{
		int ctxScanFlag=0;		/*档案层条码扫描标记＝０表示未扫描，＝１表示已经扫描，值在dacBarcode中*/
		char dacBarcode[9];		/* 档案层条码 */
    H   fmode,fhnd;
    ER ret;
    char fullbar[12],bar[12],now_s[4],filename[10],buffer[12],temp[12];											/*39码,层8位　盒10位*/
    int num,i,j;
    int flag=0;

    W fp,s,n,x,y;
		
		memset(now_s,0x0,4);
    memset(filename,0x0,10);
    memset(temp,0x0,11);
    memset(bar,0x0,11);
    memcpy(filename,"dulp1.txt",9);
		
    Wait2( 5L );
    s_sound( 2250, 5 );
    Wait2( 5L );
    s_sound( 2250, 5 );

    while (1)
    {
        lcd_cls();
        lcd_csr_put( 1, 0 );
        cputstr(LCD_ATTR_WIDTH,"存放位置采集");
        
        if(ctxScanFlag==0)
        {
        		lcd_csr_put( 9, 0 );
        		cputstr(LCD_ATTR_NORMAL,"请先扫描档案层条码... ");
        }
        else
        {
        		sprintf(now_s,"%4d",n);
        		lcd_csr_put( 3, 0 );
        		cputstr(LCD_ATTR_NORMAL,dacBarcode);
        		lcd_csr_put( 3, 12 );
		        cputstr(LCD_ATTR_NORMAL,"已采:");
		        lcd_csr_put( 3, 17 );
		        cputstr(LCD_ATTR_NORMAL,now_s);
		    		lcd_csr_put( 7, 0 );
		    		cputstr(LCD_ATTR_NORMAL,bar);
        		lcd_csr_put( 9, 0 );
        		cputstr(LCD_ATTR_NORMAL,"请扫描盒或层条码... ");
        }	



        /* 采集一个条码　 */
        i=read_str(20,1,1,bar,10);

        if (i==E_KEY_CLR)
        {
            lcd_cls();
		        lcd_csr_put( 1, 0 );
		        cputstr(LCD_ATTR_WIDTH,"存放位置采集");
            lcd_csr_put( 5, 0 );
            cputstr(LCD_ATTR_NORMAL,"真的要退出吗?        ");
            lcd_csr_put( 9, 0 );
            cputstr(LCD_ATTR_NORMAL,"[ENT是 CLR否]        ");
            while (((i=getkey())!=0x0c)&&(i!=0x0d));
            if (i==0x0d)
            {
            		/*
                if (flag==0)
                    WriteEnd();
                */
                return;
            }
            else
                continue;
        }

        if (strlen(bar)!=8 && strlen(bar)!=9 && strlen(bar)!=10)
            continue;
        else
        {
        		if(strlen(bar)==8 || strlen(bar)==9)
        		{
        				/*  层条码,不用查重 */
        				ctxScanFlag=1;
        				n=0;
        				memset(dacBarcode,0x0,9);
        				bar[8]=0x0;
        				bar[9]=0x0;
        				bar[10]=0x0;
        				for(i=0;i<8;i++)
        				{
        						dacBarcode[i]=bar[i];
        				}
								/*  保存层条码 */
				        if (( fhnd = open("data.txt", O_RDWR  ))== E_LOWERR )
				        {
				            fmode=O_CREAT|O_RDWR;
				            if (( fhnd = open("data.txt", fmode ))== E_LOWERR )
				            {
				                lcd_cls();
				                lcd_csr_put( 1, 0 );
				                cputstr(LCD_ATTR_NORMAL,"文件打开错误! ");
				                getkey();
				                return;
				            }
				        }
		            lseek(fhnd,0L,2);
		            write(fhnd,bar,8);
		            lseek(fhnd,0L,2);
		            write(fhnd,";",1);
		            close(fhnd);
		            /*  保存条码结束 */
        		}
        		else
        		{
        				/*如果没有扫描层条码，则要先扫描层条码　*/
        				if(ctxScanFlag==0)
        						continue;
        						
		        		/*　档案盒条码查重　*/
				        if (( fhnd = open(filename, O_RDWR  ))== E_LOWERR )
				        {
				            fmode=O_CREAT|O_RDWR;
				            if (( fhnd = open(filename, fmode ))== E_LOWERR )
				            {
				                lcd_cls();
				                lcd_csr_put( 1, 0 );
				                cputstr(LCD_ATTR_NORMAL,"文件打开错误! ");
				                getkey();
				                return;
				            }
				        }
				        memset(fullbar,0x0,12);
				        fullbar[0]=bar[0];
				        fullbar[1]=bar[1];
				        fullbar[2]=bar[2];
				        fullbar[3]=bar[3];
				        fullbar[4]=bar[4];
				        fullbar[5]=bar[5];
				        fullbar[6]=bar[6];
				        fullbar[7]=bar[7];
				        fullbar[8]=bar[8];
				        fullbar[9]=bar[9];
				        fullbar[10]=';';
				        ret=dat_F_Search(filename,0,11,0,11,fullbar,buffer,&fp);
				        if (ret==E_OK)
				        {
				            Wait2( 5L );
				            s_sound( 2250, 5 );
				            
				            lcd_csr_put( 7, 0 );
				            cputstr(LCD_ATTR_NORMAL,"重复扫描!           ");
				            lcd_csr_put( 9, 0 );
				            cputstr(LCD_ATTR_NORMAL,"按任意键继续...        ");
				            close(fhnd);
				            getkey();
				            continue;
				        }
				        else
				        {
				        	  lseek(fhnd,0L,2);
				            write(fhnd,bar,10);
				            lseek(fhnd,0L,2);
		            		write(fhnd,";",1);            
				            close(fhnd);
				            
						        n++;
										/*  保存盒条码 */
						        if (( fhnd = open("data.txt", O_RDWR  ))== E_LOWERR )
						        {
						            fmode=O_CREAT|O_RDWR;
						            if (( fhnd = open("data.txt", fmode ))== E_LOWERR )
						            {
						                lcd_cls();
						                lcd_csr_put( 1, 0 );
						                cputstr(LCD_ATTR_NORMAL,"文件打开错误! ");
						                getkey();
						                return;
						            }
						        }
				            lseek(fhnd,0L,2);
				            write(fhnd,bar,10);
				            lseek(fhnd,0L,2);
				            write(fhnd,";",1);
				            close(fhnd);
				            /*  保存条码结束 */
				        }

						}
				}
    }
}

void WriteEnd(void)
{
    H   fmode,fhnd;
    ER ret;
    char cr[3];
    cr[0]=0x0d;
    cr[1]=0x0a;
    cr[2]=0x0;

    if (( fhnd = open("data.txt", O_RDWR  ))== E_LOWERR )
    {
        lcd_cls();
        lcd_csr_put( 1, 0 );
        cputstr(LCD_ATTR_NORMAL,"文件打开错误! ");
        getkey();
        return;
    }
    lseek(fhnd,0L,2);
    write(fhnd,cr,2);
    close(fhnd);
}
