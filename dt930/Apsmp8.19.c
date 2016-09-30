
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
                    "      ��ӭʹ��   ",
		            "                    ",
                    "  �ֿ����ݲɼ�ϵͳ  ",
                    "     2012 �� 9 ��",
                    "������ɫũ����ҵ"};
   short          cnt, x, y;
   DAT_DSP_STR    disp_set;

   /*����������������ʾ����Ϊ12���󣭣�������*/
   dat_system( SYSD_FNC_READ, SYSD_DSP, &disp_set );
   disp_set.font_md = FONT6_SET;
   dat_system( SYSD_FNC_WRITE, SYSD_DSP, &disp_set );

   for( x=0,y=1,cnt=0; cnt<4; y+=2,cnt++ )
   Dsp_str(msg[cnt],y,x);   /*--����Ļ����(y,x)����ʾ�ַ���msg[cnt]--*/
}

/*ѡ��˵�*/
void work(void)
 {
    H x,y,cnt,code;
    ER  fup;
    unsigned char msg[][25]={" ��ɫũ��V2113",
             " 1:���ݲɼ�",
			     " 2:�����ϴ�",
			     " 3:�˳�"
                           };

    Initial();        /*----- ��ʼ�� ------*/
    for(;;)
    {

     lcd_cls();
     for( x=0,y=1,cnt=0; cnt<8; y+=2,cnt++ ) 
     Dsp_str(msg[cnt],y,x);
     code=getkey();   /*----  �ȴ�����  -----*/

     switch (code)
      {
       case '1':
        collect();              	/*--- ���ݲɼ� ----*/
        break;
       case '2':
        sendfile(fgoods);    	 /*-- �ϴ��ļ��������ļ�    ----*/
        if(( fup = open(fgoods, O_RDONLY))== E_LOWERR )
    	{
	   	/*lcd_cls();*/
		Dsp_str("�ļ�������...",1,0);
		/*Wait2(15L);*/
		close(fup);
		
    	} else
          {  close(fup);
             lcd_cls();
             Dsp_str("ɾ���ϴ��ļ�?",1,0);
             Dsp_str("1.ɾ��",3,0);
             Dsp_str("2.����",5,0); 
             code=getkey(); 
             switch(code)
             {
              case '1': 
        	dat_fdel(fgoods);        /*�ϴ�������ɾ�����ļ�*/
                break;
              default:
                break;
              }
            }           /*�ϴ�������ɾ�����ļ�*/
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
   Lenth=55;
   KeyLen=5;
   strcpy(fgoods,"wenjian.txt");
  
  
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
   UB datim[20],scount[8],head[40],bph[40],lsh[40],ygh[10],ErrStr[6],tpstr[40],hgnr[43];
   UB qxdm[7],TopStr[4],qx_h4w[6],qxnr[30];
   H  w=8,g=8,o=8;       /*  workshop,group,operator�����λ��   */
   UB barcode[25],record[25],data[130],barcodes[100],ch[30],scount1[8];
   H  codelen=24,lmt=6;  /*  codelen--��Ʒ�����󳤶ȣ� lmt--ȱ����ĳ���*/
   W  lno,lno2;
   ER fn,fm,ret,Status,i,i1,j,k,fn2,fn3,flag1,count1;
    
   struct          tim_tabl        tim_dat;
   struct          day_tabl        day_dat;
   ER                      retcd;        
   UB                      buff[8],rqsj1[20];
   H code;
   Status=0;
   i=0;
   flag1=1;
   count1=0;
   if(( fn = open(fgoods, O_CREAT | O_RDWR))== E_LOWERR )
    	{
	   	lcd_cls();
		Dsp_str("File Open Error!",0,1);
		Wait2(15L);
		close(fn);
		return; /* No Files */
    	}

   lcd_cls();
   strcpy(ErrStr,"ERROR!");
   
     write(fn,"���",4);          
     write(fn,",",1);      
     write(fn,"������",6);          
     write(fn,",",1);         
     write(fn,"����",4);    
     write(fn,"\x0d\x0a",2); 	  	
   

   
   while(1){
   if (flag1==1){
     Dsp_str("��������:",1,0);
     memset(bph,0x0,sizeof(bph));
     ret=read_str(3,0,0,bph,18);
     if(ret==E_KEY_CLR) break;
     strtake(head,bph,0,strlen(bph));        
    
     flag1=0;
   }
   	
	
   	 sprintf(scount1[], "%d", count1);  
     Dsp_str(scount1[],7,0);
     Dsp_str("����:",3,0);
     memset(bph,0x0,sizeof(bph));
     ret=read_str(3,0,0,bph,40);
     if(ret==E_KEY_CLR) break;
     strtake(lsh,bph,0,strlen(bph));
     count1++;     
     sprintf(scount1[], "%d", count1);  
     Dsp_str(scount1[],7,0);

      
        
     write(fn,"\"",1);     
     write(fn,head,strlen(head));
     write(fn,"\"",1);       
     write(fn,",",1);  
     write(fn,"\"",1);          
     write(fn,lsh,strlen(lsh));
     write(fn,"\"",1);       
     write(fn,",",1);       
     write(fn,scount,strlen(scount));     
     write(fn,"\x0d\x0a",2); 
 //    continue;
       }
   
    close(fn);


 }




