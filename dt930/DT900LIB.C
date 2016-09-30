
/***********************************************************************/
/*                                                                     */
/*  PROGRA              : CLCE LIB PROGRAM                             */
/*  SYSTEM              : DT-700 VER1.00                               */
/*  FILE                : ClceLib.C                                    */
/*  VERSION             : 1.00                                         */
/*  AUTHOR              : YangDongQing, CLCE                           */
/*  DATE OF GET         : 98/07/01                                     */
/*  DATE OF UPDATE      :                                              */
/*  FUNCTION            :                                              */
/*  HISTORY             :                                              */
/*                                                                     */
/*                                                                     */
/***********************************************************************/
#include 	    "cu_multi.h"
#include        "stdio.h"
#include        "itron.h"
#include        "CMNDEF.H"
#include        "bios1mac.h"



void   CR_Check( B *msg, short len );
void   obr_change(int);
int    getkey(void);
int    getkey2(int,int);
void   Dsp_str( B *,H ,H);
void   Dsp_str2( B *,H ,H);
void   Dsp_str3( B *,H ,H);
void   Dsp_str4( B *,H ,H);
void   alert(int);
char   get_ch(void);
void   FNC_Init( void );   /* 1996.11.12 casio */
void   FNC_Update( void );
void   Wait2(UW);
void   datetimeset(void );
void   datetimeget(UB *datetime);
void   dayhourget(UB *dayhour);
void   IntPrint(int x,int y,B *format,int tp);
void   strsub (B *sdata,B *fint,B *fdecimal);
void   strtake(B *strsub, B *str, H sos, H len);
void   strcmb(B *str, B *strsub, H sos);
void   addspace(B *str,H Keylen);
void   zeroadd(B *str,H len, H KeyLen);
void   addfloat_str(B *str,B *str1,B *str2);
void   str_cut(B *str,B *substr,B *mode);
void   char_change(B *string,char charold,char charnew);
ER     Order_Search(ER fd,W start_adr,H fieldsize,H keypos,H keylen,B *code,B *sdata,W *fpos);

extern H COM;

/**************************************************************************
      ER ercd = dat_F_Search( B *filename , W start_adr , H fieldsize ,
                        H keypos , H keylen , UB *code , UB *sdata,
                        W *fpos );

****************************************************************************/



/******************************************************************************
**     Routine name: read_str                                                **
**     Detail      : 根据指定的模式读入字符串                                **
**     Parameter   : int x,int y:  读入字符的回显位置                        **
**                   int mode   :  指定读入的模式aal of barocde type         **
**                                 0: All; 1:scan only; 2:key only           **
**                   UB *str    :  字符串用来存放读入的字符串                **
**                                 输入的时候可以放入缺省内容                **
**                   int len    :  读取字符串的长度                          **
**     Return value:  E_OK : OK;   E_KEY_CLR: clr       2:Error                         **
**     Last Date   :   1998/07/01                                            **
**     Author      : CLCE YangDongQing                                       **
******************************************************************************/
ER read_str(int y,int x,int mode,UB *str,int lenth)
{
	/*** OBR DATA BUFFER ************/
	UB     obr_data[48];

	KEY_INPS     key_str;
	KEY_INP	     key_chr;
	ER ret;
        W  Leng;
        UB Lcnt,Len;
        UW Code;
        H csr_x,csr_y,csr_type;
        int i;

        /*** BAR CODE STORE AREA INITIALIZE ***/
        for( i=0; i<48; i++ )
        {
                obr_data[i] = 0x00;
        }

	/*
	for(i=0;i<lenth;i++)
	{
		str[i]=0x31;
	}
	str[0]=0x0;
	*/


        /*** KEY INITIALIZE ROUTINE ***/
        key_str.ext = KEY_OBR_EXT | KEY_CLR_EXT | KEY_INT_EXT;
        key_str.echo = ECHO_ON;
        key_str.font_size = LCD_ANK_STANDARD;
        key_str.type = LCD_ATTR_NORMAL;
        key_str.len=lenth;  /* input lenth */
        key_str.column_pos = x;
        key_str.line_pos = y;
	key_str.column_len=0;

	key_chr.ext = KEY_OBR_EXT;
	key_chr.echo = ECHO_OFF;
	key_chr.font_size = LCD_ANK_LIGHT;
	key_chr.type = LCD_ATTR_NORMAL;
	key_chr.column_pos = x;
	key_chr.line_pos = y;

	lcd_csr_get(&csr_x,&csr_y,&csr_type);
	lcd_csr_set(LCD_CSR_UNDER);
	/*OBR_chgbuf(OBR_STOFF);*/
	OBR_chgbuf(OBR_BUFOBR);
	key_clear();

	switch(mode) /* 判断工作模式 */
	{
	case 0:   /* BOR and key */
		ret = OBR_open( 0x00000000 );	/* OBR OPEN	  */
		OBR_flush();
		ret = key_string(&key_str,str);
		if(ret==E_OK)
		{
			break;
		}
		if(ret==E_KEY_OBR)
		{
			ret = OBR_stat( &Leng, &Lcnt );
			ret = OBR_gets( &obr_data[0], &Code, &Len ); /*  读条码 */
			obr_data[Len]=0x00;
			CR_Check(obr_data,Len);
			for(i=0;i<lenth;i++)
			{
				if(obr_data[i]==0x00) break;
				str[i]=obr_data[i];
			};
			str[i]=0x00;
			ret=E_OK;
			break;
		}
		if(ret==E_KEY_CLR)
		{
			break;
		}
		ret=2;
		break;
	case 1:   /* OBR only  */
		ret = OBR_open( 0x00000000 );	/* OBR OPEN	  */
		OBR_flush();
		while(1)
		{
			ret=key_read(&key_chr);
			if(E_KEY_OBR==ret) break;
			if(0xc==ret) break;
		}
		if(ret==0xc)
		{
			ret=E_KEY_CLR;
			break;
		}
		ret = OBR_stat( &Leng, &Lcnt );
		ret = OBR_gets( &obr_data[0], &Code, &Len ); /*  读条码 */
		obr_data[Len]=0x00;
		CR_Check(obr_data,Len);
		for(i=0;i<lenth;i++)
		{
			if(obr_data[i]==0x00) break;
			str[i]=obr_data[i];
		};
		str[i]=0x00;
		ret=E_OK;
		break;
	case 2:  /* key only */
	        key_str.ext = KEY_NON_EXT | KEY_CLR_EXT;
	        ret=key_string(&key_str,str);
		break;
	default:  /* error */
		ret=2;
		break;
	}

	lcd_csr_set(csr_type);
	OBR_chgbuf(OBR_BUFOBR);
	OBR_close();
        Dsp_str(str,y,x);
	return ret;
}


/********************************************************************************
** 函数名 : read_str1()
** 功能   : 阅读条码,当mode==0或1时,按下<F5>(可根据情形另行设置)键后
**          的返回值是"E_KEY_INT",因此可用来实现"E_KEY_INT"类似的功能.
********************************************************************************/
int read_str1(int y,int x,int mode,UB *str,int lenth)
{
        /*** OBR DATA BUFFER ************/
        UB     obr_data[48];

        KEY_INPS     key_str;
        KEY_INP      key_chr;
        ER ret;
        W  Leng;
        UB Lcnt,Len;
        UW Code;
        H csr_x,csr_y,csr_type;
        int i;


   ER          err;
   UW          ptn;
   ID          fid;
   fid = FL_FK_INT_ID;
   ptn = FL_FK_INT_FNC8;
   err = key_fnc_mode(FNC_MODE_SET,FNC_8,&fid,&ptn);


        /*** BAR CODE STORE AREA INITIALIZE ***/
        for( i=0; i<48; i++ )
        {
                obr_data[i] = 0x00;
        }

        /*** KEY INITIALIZE ROUTINE ***/
        key_str.ext = KEY_OBR_EXT | KEY_CLR_EXT | KEY_INT_EXT; /*设置可识别返回值?*/
        key_str.echo = ECHO_ON;    /*回显,即输入时显示当前输入*/
        key_str.font_size = LCD_ANK_STANDARD;
        key_str.type = LCD_ATTR_NORMAL;
        key_str.len=lenth;  /* input lenth */
        key_str.column_pos = x;
        key_str.line_pos = y;
        key_str.column_len=0;

        key_chr.ext = KEY_OBR_EXT | KEY_INT_EXT;
        key_chr.echo = ECHO_OFF;
        key_chr.font_size = LCD_ANK_LIGHT;
        key_chr.type = LCD_ATTR_NORMAL;
        key_chr.column_pos = x;
        key_chr.line_pos = y;

        lcd_csr_get(&csr_x,&csr_y,&csr_type);
        lcd_csr_set(LCD_CSR_UNDER);
        /*OBR_chgbuf(OBR_STOFF);*/
        OBR_chgbuf(OBR_BUFOBR);
        key_clear();

        switch(mode) /* 判断工作模式 */
        {
        case 0:   /* BOR and key */
                ret = OBR_open( 0x00000000 );   /* OBR OPEN       */
                OBR_flush();
                ret = key_string(&key_str,str);
                if(ret==E_OK)
                {
                        ret=0;
                        break;
                }
                if(ret==E_KEY_OBR)
                {
                        ret = OBR_stat( &Leng, &Lcnt );
                        ret = OBR_gets( &obr_data[0], &Code, &Len ); /*  读条码 */
                        obr_data[Len]=0x00;
                        CR_Check(obr_data,Len);
                        for(i=0;i<lenth;i++)
                        {
                                if(obr_data[i]==0x00) break;
                                str[i]=obr_data[i];
                        };
                        str[i]=0x00;
                        ret=0;
                        break;
                }
                if(ret==E_KEY_CLR)
                {
                        break;
                }
                if(ret==E_KEY_INT) break;
                ret=2;
                break;
        case 1:   /* OBR only  */
                ret = OBR_open( 0x00000000 );   /* OBR OPEN       */
                OBR_flush();
                while(1)
                {
                        ret=key_read(&key_chr);
                        if(ret==E_KEY_INT)break;
                        if(E_KEY_OBR==ret) break;
                        if(0xc==ret) break;
                }
                if(ret==0xc)
                {
                        ret=E_KEY_CLR;
                        break;
                }

                if(ret==E_KEY_INT)
                {
                  ret=2;
                  break;
                }
                else
                {
                	ret = OBR_stat( &Leng, &Lcnt );
                	ret = OBR_gets( &obr_data[0], &Code, &Len ); /*  读条码 */
                	obr_data[Len]=0x00;
                	CR_Check(obr_data,Len);
                	for(i=0;i<lenth;i++)
                	{
	                        if(obr_data[i]==0x00) break;
                        	str[i]=obr_data[i];
                	};
                	str[i]=0x00;
                	ret=0;
                	break;
                }
        case 2:  /* key only */
                key_str.ext = KEY_NON_EXT | KEY_CLR_EXT | KEY_INT_EXT;
                ret=key_string(&key_str,str);
                if(ret==E_KEY_INT) break;
                break;
        default:  /* error */
                ret=2;
                break;
        }

   fid = FL_FK_INT_ID;
   ptn = FL_FK_INT_FNC8;
   err = key_fnc_mode(FNC_MODE_CLR,FNC_8,&fid,&ptn);
        lcd_csr_set(csr_type);
        OBR_chgbuf(OBR_BUFOBR);
        OBR_close();
        return ret;
}

/******************************************************************************
**     Routine name: cputstr                                                 **
**     Detail      : display Chinese string                                  **
**     Parameter   :   int attrib :     Display attrib                       **
**                     B *msg    :     Display message                      **
**     Return value:   0 : OK                                                **
**     Last Date   :   1998/07/01                                            **
**     Author      : CLCE YangDongQing                                       **
******************************************************************************/
int cputstr(int attrib,unsigned char *string)
{
     	int i;
     	unsigned char strtemp[67],ss0,ss1;

	for(i=0;i<66;i+=2)
	{
		if(string[i]==0)
		{
			break;
		}
		else if(string[i+1]==0)
		{
			strtemp[i]=string[i];
			i++;
			break;
		};

		if(string[i]<0x80)
		{
			strtemp[i]=string[i];
			if(string[i+1]<0x80)
			{
				strtemp[i+1]=string[i+1];
				continue;
			}
			else
				i++;
		}

		if(string[i]==0xa3)
		{
			strtemp[i]=0xe8;
			strtemp[i+1]=string[i+1]-0xa1+0x40+95;
		}
		else
		{
			ss0=string[i]-0xb0;
			ss1=string[i+1]-0xa1+0x40;

			if (ss0<8)
			{
				strtemp[i]=(ss0/2)+0x81;
				if(ss0%2)
				{
					strtemp[i+1]=ss1+95;
				}
				else
				{
					if(ss1>0x7e)
						ss1++;
					strtemp[i+1]=ss1;
				}
				/* continue;*/

			}
			else if(ss0<0x37)
			{
				strtemp[i]=((ss0+1)/2)+0x84;
				if((ss0+1)%2)
				{
					strtemp[i+1]=ss1+95;
				}
				else
				{
					if(ss1>0x7e)
						ss1++;
					strtemp[i+1]=ss1;
				}
			}
			else
			{
				ss0=ss0-0x37;
				strtemp[i]=(ss0/2)+0xe0;
				if(ss0%2)
				{
					strtemp[i+1]=ss1+95;
				}
				else
				{
					if(ss1>0x7e)
						ss1++;
					strtemp[i+1]=ss1;
				}
			} /* else */
		} /* else if */

	}  /* end of for */

	strtemp[i]=0;

	lcd_usrfont(0,"B:\\font6sk.fon");
	lcd_string(LCD_ANK_STANDARD,attrib,(B*)strtemp,LCD_LF_ON);

	return(0);
}

/******************************************************************************
**     Routine name: obr_change                                              **
**     Detail      : set the barcode type                                    **
**     Parameter   : int incode :     the serial of barocde type             **
**     Return value: 0 : OK                                                  **
**     Last Date   :   1998/07/01                                            **
**     Author      : CLCE YangDongQing                                       **
******************************************************************************/
void obr_change(int incode)
{
	/*** OBR USING MODE TABLE ***********************/
	M_TBL   obr_mode;

        /*** OBR_MODE ****/
        obr_mode.Code = OBR_ALL;
        obr_mode.Cd39[0] = 0xff;
        obr_mode.Cd39[1] = 2;
        obr_mode.Cd39[2] = 38;
        obr_mode.Cd39[3] = 1;
        obr_mode.Cd39[4] = 1;
        obr_mode.Cd39[5] = 1;
        obr_mode.Nw7[0] = 0xff;
        obr_mode.Nw7[1] = 2;
        obr_mode.Nw7[2] = 38;
        obr_mode.Nw7[3] = 0;
        obr_mode.Nw7[4] = 0;
        obr_mode.Nw7[5] = 0;
        obr_mode.Wpcea[0] = 0xff;
        obr_mode.Wpcea[1] = 10;
        obr_mode.Wpcea[2] = 18;
        obr_mode.Wpcea[3] = 0;
        obr_mode.Wpcea[4] = 1;
        obr_mode.Wpcea[5] = 1;
        obr_mode.Wpce[0] = 0xff;
        obr_mode.Wpce[1] = 8;
        obr_mode.Wpce[2] = 13;
        obr_mode.Wpce[3] = 0;
        obr_mode.Wpce[4] = 1;
        obr_mode.Wpce[5] = 1;
        obr_mode.Upcea[0] = 0xff;
        obr_mode.Upcea[1] = 9;
        obr_mode.Upcea[2] = 12;
        obr_mode.Upcea[3] = 0;
        obr_mode.Upcea[4] = 1;
        obr_mode.Upcea[5] = 1;
        obr_mode.Upce[0] = 0xff;
        obr_mode.Upce[1] = 7;
        obr_mode.Upce[2] = 7;
        obr_mode.Upce[3] = 1;
        obr_mode.Upce[4] = 1;
        obr_mode.Upce[5] = 1;
        obr_mode.Idsf[0] = 0xff;
        obr_mode.Idsf[1] = 2;
        obr_mode.Idsf[2] = 40;
        obr_mode.Idsf[3] = 0;
        obr_mode.Idsf[4] = 1;
/*        obr_mode.Idsf[5] = 1; */
        obr_mode.Idsf[5] = 0;

        obr_mode.Itrf[0] = 0xff;
        obr_mode.Itrf[1] = 4;
        obr_mode.Itrf[2] = 40;
        obr_mode.Itrf[3] = 0;
        obr_mode.Itrf[4] = 0;
/*        obr_mode.Itrf[5] = 1; */
        obr_mode.Itrf[5] = 0;


        obr_mode.Cd93[0] = 0xff;
        obr_mode.Cd93[1] = 1;
        obr_mode.Cd93[2] = 40;
        obr_mode.Cd93[3] = 0;
        obr_mode.Cd93[4] = 1;
        obr_mode.Cd93[5] = 0;
        obr_mode.Cd128[0] = 0xff;
        obr_mode.Cd128[1] = 1;
        obr_mode.Cd128[2] = 46;
        obr_mode.Cd128[3] = 0;
        obr_mode.Cd128[4] = 1;
        obr_mode.Cd128[5] = 0;
        obr_mode.Msi[0] = 0xff;
        obr_mode.Msi[1] = 1;
        obr_mode.Msi[2] = 40;
        obr_mode.Msi[3] = 0;
        obr_mode.Msi[4] = 1;
        obr_mode.Msi[5] = 1;
        obr_mode.Type = 1;
        obr_mode.Gain = 0;
        obr_mode.Buzc = 1;
        obr_mode.Ledc = 1;
        obr_mode.Bufc = 0;
        obr_mode.Endc = 0;
        obr_mode.Mode = 0;

	switch( incode )
	{
	case 0:
		obr_mode.Code = OBR_ALL;
		obr_mode.Cd39[1] = 2;
		obr_mode.Cd39[2] = 38;
		obr_mode.Cd39[4] = 0;
		obr_mode.Cd39[5] = 1;
		obr_mode.Nw7[1] = 2;
		obr_mode.Nw7[2] = 38;
		obr_mode.Itrf[1] = 4;
		obr_mode.Itrf[2] = 40;
		break;
	case 1:
		obr_mode.Code = OBR_CD39;
		obr_mode.Cd39[1] = 1;
		obr_mode.Cd39[2] = 38;
		obr_mode.Cd39[4] = 0;
		obr_mode.Cd39[5] = 1;
		break;
	case 2:
		obr_mode.Code = OBR_NW_7;
		obr_mode.Nw7[1] = 1;
		obr_mode.Nw7[2] = 38;
		break;
	case 3:
		obr_mode.Code = OBR_ITF;
		obr_mode.Itrf[1] = 2;
		obr_mode.Itrf[2] = 40;

		break;
	default:
		return;
	}

	OBR_modewt( &obr_mode );
}


/******************************************************************************
**     Routine name: CR_Check                                                **
**     Detail      : CR code is existed then clear                           **
**     Parameter   : B *msg       : pointer to character string                **
**                 : short len     : character string length                    **
**     Return value: none                                                    **
**     Last Date   :   1998/07/01                                            **
**     Author      : CLCE YangDongQing                                       **
******************************************************************************/
void CR_Check( B *msg, short len )
{
    	short   i;

    	for( i = 0; i < len; i++ )
    	{
        	if( msg[i] == 0x0d || msg[i] == '\0' )
        	{
            		msg[i] = '\0';
            		break;
	        }
    	}
    	for( ; i < len; i++ )
        	msg[i] = '\0';
}
/******************************************************************************
**     Routine name: Wait                                                    **
**     Detail      : specified second wait routine                           **
**     Parameter   : tmcnt   : timer count                                   **
**     Return value: none                                                    **
**     Author      : Fuji Software Inc.                                      **
******************************************************************************/
void Wait( UW   tmcnt )
{
ER      ercd;
UW      p_flg;

    ercd = s_settimer( FL_TM1_INT_ID, FL_TM1_INT_RTC0, tmcnt );
    wai_flg( &p_flg, FL_TM1_INT_ID, FL_TM1_INT_RTC0, TWF_ANDW | TWF_CLR );
    s_timerend( ercd );
}


/*void IntPrint(int x,int y,B *format,int tp)
{
	B temp[48];

	sprintf(temp,format,tp);

	lcd_csr_put(x,y);
	cputstr(LCD_ATTR_NORMAL,(B*)temp);
}

*/

int FileCreat(void)
{
B   fname[12],fbuff[11];
    B   fid;
    H   fmode,fhnd,fret,wkcnt;
    UH  flen;
    ER  retcd;
    ID  fi;
    UW  ptn;
    KEY_INP         pkey_inp;
    DIR_TBL     file_tbl;

    fi = FL_FK_INT_ID;
    ptn = FL_FK_INT_FNC1;
    retcd = key_fnc_mode(FNC_MODE_SET,FNC_1,&fi,&ptn);

    pkey_inp.ext  = KEY_NON_EXT | KEY_INT_EXT;
    pkey_inp.echo = ECHO_OFF;
    pkey_inp.font_size = LCD_ANK_STANDARD;
    pkey_inp.type = LCD_ATTR_NORMAL;
    pkey_inp.column_pos = 3;
    pkey_inp.line_pos   = 3;


        /*  write data set  */
     memset( fname,'\0',12);
     memset( fbuff,'\0',11);

     strcpy( fname,"stock   pmk");

     lcd_cls();
     /*  open    */
        fmode = O_CREAT | O_WRONLY | O_TRUNC;
        fhnd = open(fname, fmode);
        if (fhnd == E_LOWERR){
            lcd_string( LCD_ANK_LIGHT, LCD_ATTR_NORMAL,(B*)"file  open  error" , LCD_LF_OFF);
            key_read( &pkey_inp );
            return(1);
        }


     /*  close   */
        fret = close(fhnd);
        if (fret == E_LOWERR){
            lcd_string( LCD_ANK_LIGHT, LCD_ATTR_NORMAL,(B*)"file  close  error" , LCD_LF_OFF);
            key_read( &pkey_inp );
            fret = close(fhnd);
            return(1);
        }

    lcd_cls();
    /*  dat_fdir    */
    fid = DAT_FILE_TOP;

    retcd = dat_fdir(fid,&file_tbl);
        if (retcd == E_OK) {}

       else{
            lcd_csr_put( 0, 1);
            lcd_string( LCD_ANK_LIGHT, LCD_ATTR_NORMAL,(B*)"File Creat ERR" , LCD_LF_OFF);
            key_read( &pkey_inp );
            return(1);
             }

    fi = FL_FK_INT_ID;
    ptn = FL_FK_INT_FNC1;
    retcd = key_fnc_mode(FNC_MODE_CLR,FNC_1,&fi,&ptn);

    return(1);
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

void FNC_Update( void )
{
        KEYFORM		kcode;



        kcode.attr = 0x0;
	kcode.code = 0x1;
		key_fnc( FNC_SET, FNC_6, &kcode);
	kcode.attr = 0x0;
	kcode.code = 0x2;
		key_fnc( FNC_SET, FNC_7, &kcode);
	kcode.attr = 0x0;
	kcode.code = 0x3;
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
UW      p_flg;

    ercd = s_settimer2( FL_TM2_INT_ID, FL_TM2_INT_ITU0, tmcnt );
    wai_flg( &p_flg, FL_TM2_INT_ID, FL_TM2_INT_ITU0, TWF_ANDW | TWF_CLR );
    s_timerend2( ercd );
}


int getkey2(int y,int x)
 {
   KEY_INP	kinp;
   int code;
   H csr_x,csr_y,csr_type;

        lcd_csr_get(&csr_x,&csr_y,&csr_type);
	lcd_csr_set(LCD_CSR_UNDER);
   	kinp.ext = KEY_OBR_EXT;
	kinp.echo = ECHO_ON;
	kinp.font_size = LCD_ANK_STANDARD;
	kinp.type = LCD_ATTR_NORMAL;
	kinp.column_pos = x;
	kinp.line_pos = y;
        code=key_read(&kinp);
        lcd_csr_set(csr_type);
        return (code);
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

char get_ch(void)
{
	UW key = 0;
	KEY_INP	key_chr; /* key_read*/

	key_chr.ext = KEY_INT_EXT | KEY_CLR_EXT;
	key_chr.echo = ECHO_OFF;
	key_chr.font_size = LCD_ANK_STANDARD;
	key_chr.type = LCD_ATTR_NORMAL;
	key_chr.column_pos = 0;
	key_chr.line_pos = 2;

	key_clear();
	key = key_read(&key_chr);
	key_clear();
	return((char)key);
}

ER dat_F_Read(B *filename,W pos,B *sdata,H len)
{
	int fd,err;

	fd=open(filename,O_RDWR);
	if(fd==E_LOWERR)  return E_PRM;

	err=lseek(fd,pos,0);

	if(err==E_LOWERR)
	{
		close(fd);
		return(1);
	}

	err=read(fd,sdata,len);

	if(err!=len)
	{
		close(fd);
		return(1);
	}

	close(fd);
	return (0);
}



ER dat_F_Write(B *filename,H mode,W pos,B *sdata,H len)
{
	int fd,err;

	fd=open(filename,O_RDWR);
	if(fd==E_LOWERR)  return E_PRM;

	if(mode==0)
	{
		err=lseek(fd,pos,0);
	}
	else if(mode==1)
	{
		err==lseek(fd,0L,2);
	}
	else
	{
		close(fd);
		return E_PRM;
	}

	if(err==E_LOWERR)
	{
		close(fd);
		return E_PRM;
	}

	err=write(fd,sdata,len);
	if(err==E_LOWERR)
	{
		close(fd);
		return(3); /* other err */
	}

	close(fd);
	return E_OK;
}



void dateset(void)
  {
        struct          day_tabl  day_dat;
        ER              retcd,len;
        B               buff[5];
        KEY_INPS        pkey_inps;

        retcd = s_dateget(&day_dat);
        lcd_csr_put( 3, 1 );
        memset(buff,0x00,sizeof(buff));
        sprintf(buff,"%4d",day_dat.year);
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_NORMAL,(B*)buff , LCD_LF_OFF );
        /*      set     month   */
        lcd_csr_put( 3, 5 );
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_NORMAL,(B*)"/" , LCD_LF_OFF );
        lcd_csr_put( 3, 6 );
        memset(buff,0x00,sizeof(buff));
        sprintf(buff,"%2d",day_dat.month);
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_NORMAL,(B*)buff , LCD_LF_OFF );

        lcd_csr_put( 3, 5 );
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_NORMAL,(B*)"/" , LCD_LF_OFF );

        /*      set day */
        lcd_csr_put( 3, 8 );
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_NORMAL,(B*)"/" , LCD_LF_OFF );
        lcd_csr_put( 3, 9 );
        memset(buff,0x00,sizeof(buff));
        sprintf(buff,"%2d",day_dat.day);
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_NORMAL,(B*)buff , LCD_LF_OFF );
        lcd_csr_put( 3, 8 );
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_NORMAL,(B*)"/" , LCD_LF_OFF );

        /*      input   */
        pkey_inps.ext = KEY_NON_EXT;
        pkey_inps.echo = ECHO_ON;
        pkey_inps.font_size = LCD_ANK_STANDARD;
        pkey_inps.type = LCD_ATTR_REVERS;
        pkey_inps.clr_type = KEY_NUM_CLR_ON;

        /*      input   year    */
        memset(buff,0x00,sizeof(buff));
        pkey_inps.len = 4;
        pkey_inps.column_pos = 1;
        pkey_inps.line_pos = 5;
        retcd = key_num( &pkey_inps, &buff);
        len=strlen(buff);
        if (len==0)
          {
           lcd_csr_put( 5, 1 );
           sprintf(buff,"%4d",day_dat.year);
           lcd_string( LCD_ANK_STANDARD, LCD_ATTR_REVERS,(B*)buff , LCD_LF_OFF );
            goto lab1;
           }
        day_dat.year = atol(buff);

lab1:        /*      input month     */
        memset(buff,0x00,sizeof(buff));
        pkey_inps.len = 2;
        pkey_inps.column_pos = 6;
        pkey_inps.line_pos = 5;
        retcd = key_num( &pkey_inps, &buff);
        len=strlen(buff);
        lcd_csr_put( 5, 5 );
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_REVERS,(B*)"/" , LCD_LF_OFF );
        if (len==0)
          {
            lcd_csr_put(5,6);
            sprintf(buff,"%2d",day_dat.month);
            lcd_string( LCD_ANK_STANDARD, LCD_ATTR_REVERS,(B*)buff , LCD_LF_OFF );
            goto lab2;
           }
        day_dat.month = atoi(buff);


lab2:
        memset(buff,0x00,sizeof(buff));
        pkey_inps.len = 2;
        pkey_inps.column_pos = 9;
        pkey_inps.line_pos = 5;
        retcd = key_num( &pkey_inps, &buff);
        len=strlen(buff);
        lcd_csr_put( 5, 8 );
        lcd_string( LCD_ANK_STANDARD, LCD_ATTR_REVERS,(B*)"/" , LCD_LF_OFF );
        if (len==0)
          {
           lcd_csr_put(5,9);
           sprintf(buff,"%2d",day_dat.day);
           lcd_string( LCD_ANK_STANDARD, LCD_ATTR_REVERS,(B*)buff , LCD_LF_OFF );
           goto lab3;
          }
        day_dat.day = atoi(buff);

lab3:
        s_dateset( &day_dat );

        key_clear();
        while( 1 ){
                if(key_check() != E_NG){
                        break;
                }
        }

}

void dayhourget(UB *dayhour)
{
   struct          day_tabl  day_dat;
   struct          tim_tabl        tim_dat;
   ER              retcd;
   UB               hour[3],minute[3];

        retcd = s_timeget(&tim_dat);
        memset(hour,0x00,sizeof(hour));
        sprintf(hour,"%02d",tim_dat.hour);
        memset(minute,0x00,sizeof(minute));
        sprintf(minute,"%02d",tim_dat.mint);
        strcpy(dayhour,hour);
        strcat(dayhour,minute);
}

void datetimeget(UB *datetime)
{
   struct  day_tabl  day_dat;
   struct  tim_tabl  tim_dat;
   ER      retcd;
   UB      minute[3],hour[3],day[3],month[3],year[5],buff[30];

        retcd = s_dateget(&day_dat);
        memset(day,0x0,sizeof(day));
        sprintf(day,"%02d",day_dat.day);
        memset(month,0x0,sizeof(month));
        sprintf(month,"%02d",day_dat.month);
        memset(year,0x0,sizeof(year));
        sprintf(year,"%4d",day_dat.year);

        retcd = s_timeget(&tim_dat);
        memset(hour,0x00,sizeof(hour));
	sprintf(hour,"%02d",tim_dat.hour);
	memset(minute,0x0,sizeof(minute));
	sprintf(minute,"%02d",tim_dat.mint);
        memset(buff,0x0,sizeof(buff));
        lcd_cls();
        strcpy(buff,year);
        strcat(buff,month);
        strcat(buff,day);
        strcat(buff,hour);
        strcat(buff,":");
        strcat(buff,minute);
        strcpy(datetime,buff);

}



void strsub (B *sdata,B *fint,B *fdecimal)
 {
   ER i,j,point=100;

   for(i=0;i<strlen(sdata);i++)
    {
     if(sdata[i]=='.') point=i;
    }

   if(point==100)
    {
      strcpy(fint,sdata);
      strcpy(fdecimal,"000");
    }

   else if(point==0)
    {
     strcpy(fint,"0");
     strcpy(fdecimal,sdata);
    }
   else
   {
     for(i=0;i<point;i++) fint[i]=sdata[i];
      fint[i]=0x0;

     if((strlen(sdata)-point)==3)
      { fdecimal[0]=sdata[point+1];
        fdecimal[1]=sdata[point+2];
        fdecimal[2]='0';
      }
     if((strlen(sdata)-point)==2)
      { fdecimal[0]=sdata[point+1];
        fdecimal[1]='0';
        fdecimal[2]='0';
      }
     else
      { for(i=point+1,j=0;j<3;i++,j++) fdecimal[j]=sdata[i]; }
     fdecimal[3]=0x0;
   }
 }

 void Dsp_str( B *data,H gyo,H keta)
{

	lcd_csr_put( gyo, keta );
/*	lcd_string( dsp_ank, dsp_mode, (B *)data, LCD_LF_OFF );*/
        cputstr(LCD_ATTR_NORMAL,(B*)data);
	/*cputstr800(dsp_mode,(B *)data); */
}


void Dsp_str2( B *data,H gyo,H keta)
{

	lcd_csr_put( gyo, keta );
        cputstr(LCD_ATTR_REVERS, (UB*)data);
}

void Dsp_str3( B *data,H gyo,H keta)
{

	lcd_csr_put( gyo, keta );
        cputstr(LCD_ATTR_WIDTH, (UB*)data);
}


void Dsp_str4( B *data,H gyo,H keta)
{

	lcd_csr_put( gyo, keta );
        cputstr(LCD_ATTR_DOUBLE, (UB*)data);
}



void addfloat_str(B *str,B *str1,B *str2)
{
  B str1_1[15],str1_2[15],str2_1[15],str2_2[15],str_int[15],str_frct[15];
  W  add_frct;
  ER itg;

  memset(str2_1,0,9);
  memset(str2_2,0,9);
  add_frct=0;
  strsub(str1,str1_1,str1_2);
  strsub(str2,str2_1,str2_2);
  if(str2[0]=='-') add_frct=atoi(str1_2)-atoi(str2_2);
  else
    add_frct=atoi(str1_2)+atoi(str2_2);
  itg=0;
  if(add_frct>=1000) {add_frct-=1000; itg=1; }
  if(add_frct<0) {add_frct+=1000; itg=-1; }
  sprintf(str_frct,"%3ld",add_frct);
  if(str_frct[0]==' ') str_frct[0]='0';
  if(str_frct[1]==' ') str_frct[1]='0';
  sprintf(str_int,"%5ld",(atoi(str1_1)+atoi(str2_1)+itg));

  strcpy(str,str_int);
  strcat(str,".");
  strcat(str,str_frct);

}



/***********************************************************
*   程序名称: strtake
*   详细说明: 在某字符串中截取某一长度的字符串
*   参数    : B *str    :  原始字符串指针
*             B *strsub :  截取出的字符串指针
*             H sos     :  在原始字符串中截取的起始位置
*             H len     :  在原始字符串中截取的字符长度
*   返回值  : 空
***********************************************************/
void strtake(B *strsub, B *str, H sos, H len)
 {

   H i,j;
   for(i=sos,j=0;j<len;i++,j++)   strsub[j]=str[i];
   strsub[j]=0x0;

 }

void strcmb(B *str, B *strsub, H sos)
 {
   H i,j;
   for(i=sos,j=0;j<strlen(strsub);i++,j++)  str[i]=strsub[j];

 }

 /********************************************************************/
/* Notice: 对长度为len的字符串，在其后补加空格，使其长度达到KeyLen  */

/*******************************************************************/
void addspace(B *str,H Keylen)
{
  H i;
  B str0[30];

  strcpy(str0,str);
  for(i=strlen(str0);i<Keylen;i++) str[i]=' ';
  str[Keylen]=0x0;
}


void zeroadd(B *str,H len, H KeyLen)
{
  B temp[20];
  H i;

  for(i=0;i<(KeyLen-len);i++) temp[i]='0';
  temp[i]=0x0;
  strcat(temp,str);
  strcpy(str,temp);
  str[KeyLen]=0x0;
}

/*将字符串str中含有的子字符串substr剪切掉.mode是选择全部剪切还是只剪切第一个*/
void str_cut(B *str,B *substr,B *mode)
{
  ER k[30];
  ER i,j,m,n;
  B strnew[30];


    m=0;
    for(i=0,j=0;i<strlen(str);i++)
     {

       if(str[i]==substr[j])   j++;
       else
        {j=0;
         continue;
        }

       if(j==strlen(substr))
        {
          k[m]=i-j+1;
          j=0;
          m++;
        }

     }
 if (m>0)
   {
    if (strcmp(mode,"FIRST")==0)m=1;
    n=0;j=0;
    for (i=0;i<strlen(str);i++)
     {
       if (i==k[n]&& n<m)
         {
           i=i+strlen(substr)-1;
           n++;
           continue;
         }
       if(i!=k[n])
         {
           strnew[j]=str[i];
           j++;
         }
     }
    strnew[j]=0x0;
    strcpy(str,strnew);
   }
}

/*将string中所有的字符charold改为charnew*/
void char_change(B *string,char charold,char charnew)
{
 H len,i;

 len=strlen(string);
 for (i=0;i<len;i++)
   {
    if (string[i]==charold)
      {string[i]=charnew;}
    else continue;
   }
}


/*******************************************************************
*     程序名称: sendfile
*     详细说明: 上传一文件
*     参数    : UB *fname : 传输的文件名指针
*               文件名形式: 主文件名为8位,不足者加空格
*                           后缀名为txt,主文件名与后缀名间无小数点
*     返回值  : 1 :  错误
*               0:   正确
********************************************************************/
int sendfile (UB *fname)
{

        ER                      ercd,fup,len;
        UH                      idx,i,j,k;
        struct sys_tty          param;
        B                       rec_buff[60],ftemp[15];
        FILE                    *file_a,*file_b;
        CU_FILE_INFO_FORM       fileInfo[1];


        if(( fup = open(fname, O_RDONLY))== E_LOWERR )
    	{
	   	lcd_cls();
		Dsp_str("文件不存在...",1,0);
		Wait2(15L);
		close(fup);
		return (1); /* No Files */
    	}
    	close(fup);

        lcd_cls();
	Dsp_str("Sending....",3,1);

	len=strlen(fname)-4;
	memset(ftemp,0x0,sizeof(ftemp));
	memcpy(ftemp,fname,len);
	addspace(ftemp,8);
	strcat(ftemp,"txt");
        lcd_cls();
        Dsp_str("打开通讯口...",3,0);
        Dsp_str("<F1--终止>",5,0);


        cu_stopKeySet( CU_FNC_1 );

        param.speed = B_19200;
        param.length = CHAR_8;
        param.parity = PARI_NON;
        param.stop_bit = STOP_1;

        ercd = cu_open( COM , CU_CNCT_MULT , &param );
        if(ercd != E_OK)
        {
                cu_stopKeySet( CU_FNC_NON );

                lcd_cls();
                lcd_csr_put( 1 , 0 );
                cputstr( LCD_ATTR_NORMAL , (UB *)"COM 打开错误" );
                lcd_csr_put( 5 , 0 );
                cputstr( LCD_ATTR_NORMAL ,  (UB *)" 按任意键 !" );

                key_clear();
                getkey();

                return( 1 );
        }

        memcpy(fileInfo[0].fileName,ftemp,11);

        ercd = cu_fileSend( COM , 0 , CU_KIND_OTHER , 1 , &fileInfo , CU_GRAPH_ON_1 , 1 );

        if(ercd != E_OK)
        {
                cu_close( COM );
                cu_stopKeySet( CU_FNC_NON );

                lcd_cls();
                Dsp_str("数据文件未上传!",3,0);
                Wait2(20L);
                return( 1 );
        }

        cu_close( COM );
        cu_stopKeySet( CU_FNC_NON );

        key_clear();

        return( 0 );
}

/*******************************************************************
*     程序名称: recvfile
*     详细说明: 接收一文件
*     返回值  : 1 :  错误
*               0:   正确
********************************************************************/
int recvfile()
 {
        ER                      ercd,fup;
        UH                      idx;
        struct sys_tty          param;
        B                       rec_buff[60];
        FILE                    *file_a,*file_b;
        CU_FILE_INFO_FORM       fileInfo[2];
        UH                      fileCount;

        lcd_cls();
	Dsp_str("Recieving...",3,1);
	Dsp_str(" <F1--终止>",5,1);

        cu_stopKeySet( CU_FNC_1 );
        param.speed = B_19200;
        param.length = CHAR_8;
        param.parity = PARI_NON;
        param.stop_bit = STOP_1;

        ercd = cu_open( COM , CU_CNCT_MULT , &param );
        if(ercd != E_OK)
        {
                cu_stopKeySet( CU_FNC_NON );

                lcd_cls();
                lcd_csr_put( 1 , 0 );
                cputstr( LCD_ATTR_NORMAL , (B *)"COM 打开错误" );
                lcd_csr_put( 5 , 0 );
                cputstr( LCD_ATTR_NORMAL ,  (B *)" 按任意键 !" );

                key_clear();
                getkey();

                return( 1 );
        }

        fileCount=20;
        ercd = cu_fileRecv( COM , 0 , CU_KIND_OTHER , &fileCount , &fileInfo , CU_GRAPH_ON_1 , 1 );

        if(ercd != E_OK)
        {
                cu_close( COM );
                cu_stopKeySet( CU_FNC_NON );

                lcd_cls();
                lcd_csr_put( 1 , 0 );
                cputstr(LCD_ATTR_NORMAL ,  (B *)" 文件下传错误" );
                lcd_csr_put( 5 , 0 );
                cputstr( LCD_ATTR_NORMAL , (B *)"  请按任意键!" );

                key_clear();
                getkey();
                return( 1 );
        }

        cu_close( COM );
        cu_stopKeySet( CU_FNC_NON );

        key_clear();
        lcd_cls();
        lcd_csr_put( 1 , 0 );
        cputstr(LCD_ATTR_NORMAL ,  (B *)" 文件下传结束" );
        lcd_csr_put( 5 , 0 );
        cputstr( LCD_ATTR_NORMAL , (B *)"  请按任意键!" );
        getkey();
        return( 0 );
 }



int half_search(ER fn,W pos,H fieldsize,H keypos,H keylen,UB *code,UB *sdata,W *fpos,W num,int lenth)
{
	int i,signal=2;
	int er=0,loca;
	ER len1=0,len2=0;
	W cpos;
	W min=0L,top=0L,bott=num-1;
	B tempcode[15];
	B Top[6],Bott[6],Min[6];

	cpos=pos;
	len1=keylen-lenth;
	er=lseek(fn,cpos,0);
	if(er==E_LOWERR)
	{
		return E_PRM;
	}

	if(er!=pos)
	{
		return E_PRM;
	}
	if(cpos>=0L && cpos<=num*fieldsize)loca=0;
	else loca=1;
	while(top<=bott && loca==0)
	{
		min=(top+bott)/2;
		len2=0;

   	        cpos=min*fieldsize;

		lseek(fn,cpos,0);
		er=read(fn,sdata,fieldsize);
		if(er==E_LOWERR)  break;
		if(er!=fieldsize) break;

		for(i=0;i<keylen;i++)
		   {
		   	tempcode[i]=sdata[i+keypos];
		   	if(tempcode[i]==' ')len2++;
		   }
		tempcode[i]=0x0;

		if (strcmp(code,tempcode)==0)break;

		if (len1<len2)signal=0;
		if (len1>len2)signal=1;

		if (len1==len2)
		  {

           	     for (i=0;i<lenth;i++)
           	       {
           	         if (code[i]!=tempcode[i])
           	             {
           	               if (code[i]>tempcode[i])signal=0;
           	               if (code[i]<tempcode[i])signal=1;
           	               break;
           	             }
           	       }
		  }


		if (signal==1)
		   {

		        bott=min-1;

		   }
                if (signal==0)
                   {

                        top=min+1;
                   }
		if(top>bott)er=-1;

	}


	if(er==E_LOWERR) return E_PRM;

	if(er!=fieldsize) return E_NG;

	     *fpos=cpos;
	     return E_OK;
	if(er==-1)	return E_PRM;
}

ER Order_Search(ER fd,W start_adr,H fieldsize,H keypos,H keylen,B *code,B *sdata,W *fpos)
{
	int i;
	int er=0;
	W cpos;

	cpos=start_adr;
	er=lseek(fd,cpos,0);
	if(er==E_LOWERR)
	{
		return E_PRM;
	}

	if(er!=start_adr)
	{
		return E_PRM;
	}

	while(1)
	{

		er=read(fd,sdata,fieldsize);
		if(er==E_LOWERR)  break;
		if(er!=fieldsize) break;

		for(i=0;i<keylen;i++)
		{
			if(code[i]!=sdata[i+keypos]) break;
		};

		if(i==keylen) break; /* compare OK */
		cpos+=fieldsize;
	}


	if(er==E_LOWERR) return E_PRM;

	if(er!=fieldsize) return E_NG;

	*fpos=cpos;
	return E_OK;

}


void alert(int i)
{
  int j;

  for(j=1;j<=i;j++)
  {
    s_sound(2250,3*j);
    Wait2(10L);
   }
}


