
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
void Initial(void);

/*----------------------*
** Global Variable     **
*-----------------------*/

UB fgoods[20],snub[6];
UB allcode[2000][15],  allnub[200][21];
H  Lenth,  KeyLen,  COM  =  COM0;
ER  fn,  sumcount  =  0,a  =  0,  nb  =  0;

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
   unsigned char    msg[][20]=
   {
                    "��ӭʹ�ñ��������",
                    " ",
                    "������ɫũ����˾",
                    "No_Zero",
                    "QQ�� 3921342"
   };
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
    H  x,  y,  cnt,  m, code,  turn  =  0;
    ER  fup;
    unsigned char msg[][25]={"####������ũ�ֿ�#####",
                             "    1:���ݲɼ�",
                             "    2:�����ϴ�",
                             "    3:����ͳ��",
                             "    4:�������"
                           };
    unsigned char msg1[][20]={
                    "��ӭʹ�ñ��������",
                    " ",
                    "������ɫũ��",
                    "No_Zero",
                    "QQ�� 3921342"};

    Initial();        /*----- ��ʼ�� ------*/
    for(  ;  ;)
    {

     lcd_cls();
     for( x  =  0,y  =  1,cnt  =  0; cnt  <  5; y  +=  2,cnt++ )
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
                memset(allnub,0x0,sizeof(allnub));

                Dsp_str("ɾ��?",1,0);
                Dsp_str("1.ɾ��",3,0);
                Dsp_str("2.����",5,0);
                code=getkey();
                switch(code)
                {
                case '1':
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
            Dsp_str("�ļ��򿪴���!",0,1);
            Wait2(15L);
            close(fn);
            return; /* No Files */
    	}

            break;
        case '4':
            lcd_cls();
            for( x  =  0,y  =  1,cnt  =  0; cnt  <  5; y  +=  2,cnt++ )
            Dsp_str(msg1[cnt],  y,  x);
            code  =  getkey();
            switch(code)
            {
                default:
                break;
            }
            break;
        case '3':
            m  =  0;
            turn  =  0;
            if (nb ==  0) break;

            while(1)
            {
            lcd_cls();
            for( x  =  0,y  =  1,cnt  =  0; cnt  <  5; y  +=  2,  cnt++ )
            {
                if (cnt ==  nb - m)
                break;
                Dsp_str(allnub[cnt+m],  y,  x);
            }

            code  =  getkey();  /*----  �ȴ�����  -----*/
            switch(code)
            {
                case '2':
                m  +=  5;
                turn  =  1;
                break;

                case '6':
                m  +=  5;
                turn  =  1;
                break;

                default:
                turn  =  2;
                break;
            }
            if (turn  ==  1) continue;
            if (turn  ==  2) break;
            }
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
   memset(snub,0x0,sizeof(snub));

     if(( fn = open(fgoods, O_CREAT | O_RDWR))== E_LOWERR )
    	{
            lcd_cls();
            Dsp_str("�ļ�����!",0,1);
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
   UB dh_heji[8],dh_nn[8],danhao[40],buff[40],tiaoma[40],heji[10],neednub[40];
   ER ret,  i,  j,  k,  m,  m1,  turn;
   H code;

   while(1)             /*          ���뵥�źʹ˵��ŵĸ���         */
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
        if (danhao[4]  == '-')
        {
            lcd_cls();
            Dsp_str("�����������",  1,  0);
            Dsp_str("�ܿ��������룡",  3,  0);
            s_beep();
            lcd_el(1);
            code  =  getkey();
            switch(code)
            {
                default:
                lcd_el(0);
                break;
            }
            continue;
        }

        while(1)        /*             ɨ��������        */
        {

            lcd_cls();
            memset(dh_heji,  0x0,  sizeof(dh_heji));
            sprintf(dh_heji,  "%d",  j);
            memset(heji,  0x0,  sizeof(heji));
            sprintf(heji,  "%d",  sumcount);

            Dsp_str("����:",  1,  0);
            Dsp_str(danhao,  1,  7);

            if (j  >  0)
            {   if (turn  == 1)
                {
                    Dsp_str("����ͬ�ţ�",  7,  0);
                    Dsp_str(tiaoma,  7,  11);
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
            if(ret  ==  E_KEY_CLR)
            {

                if (  (nb  ==  0)  ||  ((strcmb(allnub[nb-1],danhao) != 0))  )
                {
                    memset(dh_nn,  0x0,  sizeof(  dh_nn));
                    sprintf(dh_nn,"%d",j);
                    strtake(allnub[nb],danhao,0,strlen(danhao));
                    addspace(allnub[nb],13);
                    strcat(allnub[nb],dh_nn);
                    nb++;
                }
                break;
            }
            strtake(tiaoma,  buff,  0,  strlen(buff));

            turn  =  0;

            if (a  >  0)                /*  ������ͬ�����������  */
            {
                m  =  a;
                if (a  >  900)
                m  =  900;

                while(m  >  0)
                {
                     if (strcmp(allcode[m-1],  tiaoma)  ==  0)
                     {
                        lcd_cls();
                        Dsp_str("ע�⣺��ͬ�ţ�",  1,  0);
                        Dsp_str("�������������",  3,  0);
                        lcd_el(1);
                        alert(3);
               /*         s_sound(4000,  20);       */
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
                if (a  == 2000)
                {
                    a  =  0;

                }
                strtake(allcode[a],  buff,  0,  strlen(buff));
                a++;

                lseek(fn,0L,2);
                write(fn,  danhao,  strlen(danhao));
                k  =  strlen(danhao);
                while(k  <  30)
                {
                    write(fn,  " ",  1);
                    k++;
                }
                write(fn,  tiaoma,  strlen(tiaoma));
                write(fn,  "\x0d\x0a",  2);


                continue;
            }
            else
            continue;
        }
   }



 }




