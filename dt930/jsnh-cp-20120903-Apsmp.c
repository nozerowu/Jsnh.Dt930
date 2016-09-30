
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
   disp_set.font_md = FONT6_SET;
   dat_system( SYSD_FNC_WRITE, SYSD_DSP, &disp_set );

   for( x = 0,y = 1,cnt = 0; cnt < 5; y += 2,cnt++ )
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
        if(( fup = open(fgoods, O_RDONLY))== E_LOWERR )
    	{
	   	/*lcd_cls();*/
		Dsp_str("�ļ�������...",1,0);
		/*Wait2(15L);*/
		close(fup);

    	} else
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
   Dsp_str("PC��ͨѶ��:",1,0);
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

/*�ɼ�����*/
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
        Dsp_str("����:",1,0);
        Dsp_str("����: 0",7,0);
        Dsp_str("�ϼ�",7,11);
        Dsp_str(total,7,18);


        memset(strbuff,0x0,sizeof(strbuff));
        ret=read_str(3,0,0,strbuff,18);
        if(ret==E_KEY_CLR) break;
        strtake(customer,strbuff,0,strlen(strbuff));

        lcd_cls();
        Dsp_str("����Ҫɨ��ļ���",1,0);

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
            Dsp_str("����:",1,0);
            Dsp_str(customer,1,7);

            Dsp_str("����:",7,0);
            Dsp_str(numbs,7,6);
            Dsp_str("�ϼ�:",7,9);
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
                        Dsp_str("ע�⣺��ͬ�ţ�",1,0);
                        Dsp_str("����ɨ�裡��",3,0);
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
                    Dsp_str("ɨ�����",1,0);
                    Dsp_str("�������ɨ��һ�ң�",3,0);
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




