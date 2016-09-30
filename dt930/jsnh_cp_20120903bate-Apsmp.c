
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
    Wait2( 5L );                /* ��ʱ --casio      */
    s_sound( 2250, 5 );         /* ���� --casio      */
    Wait2( 5L );
    s_sound( 2250, 5 );
    FNC_Init();                 /*  ���ܼ�����       */
    lcd_cls();                  /*  ���� --casio     */
    MenuOpening();              /*  ��ʾ������Ϣ--clce */
    Wait2( 20L );
    obr_change(0);              /*  ���ÿɶ���������� */
    work();                     /*  ��Ҫ��������       */
    exit(0);                    /*  Ӧ�ó������       */
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
                    "  �ϲ��ֿ�   ",
                    "  2012-09 ",
                    " ����ɼ�ϵͳ  ",
                    "  by QQ 3921342",
                    "������ɫũ��"};
   short          cnt, x, y;
   DAT_DSP_STR    disp_set;

   /*����������������ʾ����Ϊ12���󣭣�������*/
   dat_system( SYSD_FNC_READ, SYSD_DSP, &disp_set );
   disp_set.font_md  =  FONT6_SET;
   dat_system( SYSD_FNC_WRITE, SYSD_DSP, &disp_set );

   for( x  =  0,y  =  1,cnt  =  0; cnt  <  5; y  +=  2,cnt++  )
   Dsp_str(msg[cnt],y , x);   /*--����Ļ����(y,x)����ʾ�ַ���msg[cnt]--*/
}

/*ѡ��˵�*/
void work(void)
 {
    H  x,  y,  cnt,  code;
    ER  fup;
    unsigned char msg[][25]={"�ϲ��ֿ�����ϵͳ",
                             " 1:���ݲɼ�",
                             " 2:�����ϴ�",
                             " 3:�˳�"
                           };

    Initial();        /*----- ��ʼ�� ------*/
    for(  ;  ;)
    {

     lcd_cls();
     for( x  =  0,y  =  1,cnt  =  0; cnt  <  4; y  +=  2,cnt++ )
     Dsp_str(msg[cnt],  y,  x);
     code  =  getkey();   /*----  �ȴ�����  -----*/

     switch (code)
      {
       case '1':
            collect();              	/*--- ���ݲɼ� ----*/
            break;
       case '2':
        close(fn);
        sendfile(fgoods);    	 /*-- �ϴ��ļ��������ļ�    ----*/
        if(( fup  =  open(fgoods, O_RDONLY))  ==   E_LOWERR )
    	{
	   	/*lcd_cls();*/
		Dsp_str("�ļ�������...",1,0);
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

            Dsp_str("ɾ��?",1,0);
            Dsp_str("1.ɾ��",3,0);
            Dsp_str("2.����",5,0);
            code=getkey();
            switch(code)
            {
            case '1':
                close(fn);
                dat_fdel(fgoods); /*�ϴ�������ɾ�����ļ�*/

                break;
            default:
                break;
            }
        }
                /*�ϴ�������ɾ�����ļ�*/
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


/*�ɼ�����*/

/*  *********************************************


    dh_heji�����ż���  danhao������     buff��   ��ʱ
    tiaoma :  ������   heji  : �ܼ���   neednub: ����ɨ������


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
        Dsp_str("����:",  1,  0);
        Dsp_str("����: 0",  7,  0);
        Dsp_str("�ϼ�",  7,  11);
        Dsp_str(heji,  7,  18);


        memset(buff,  0x0,  sizeof(buff));
        ret=read_str(3,  0,  0,  buff,  18);
        if(ret  ==  E_KEY_CLR)  break;
        strtake(danhao,   buff,  0,  strlen(buff));

        lcd_cls();
        Dsp_str("����Ҫɨ��ļ���",  1,  0);

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

            Dsp_str("����:",  1,  0);
            Dsp_str(danhao,  1,  7);

            if (j  >  1)
            {   if (turn  == 1)
                {
                    Dsp_str("��һ����ͬ�ţ�",  7,  0);
                }
                else
                {
                    Dsp_str("��һ����:",  7,  0);
                    Dsp_str(tiaoma,  7,  11);
                }

            }

            Dsp_str("����:",  9,  0);
            Dsp_str(dh_heji,  9,  6);
            Dsp_str("�ϼ�:",  9,  9);
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
                        Dsp_str("ע�⣺��ͬ�ţ�",  1,  0);
                        Dsp_str("�������������",  3,  0);
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
                    Dsp_str("ɨ�����",  1,  0);
                    Dsp_str("�������ɨ��һ�ң�",  3,  0);
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




