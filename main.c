
 
#include <io430x20x1.h>
#include "in430.h" /*Genel Kesmeleri aktif etmek için gereken baþlýk dosyasý*/
#define DUGME1 P1IN_bit.P1IN_6
#define DUGME2 P1IN_bit.P1IN_7
//Saatin modlarý
#define MD_NORMAL 0;//Saatin normal çalýþma modu
#define MD_ST_AYAR 1;//Saat ayarlama modu
#define MD_DK_AYAR 2;//Dakika ayarlama modu
#define MD_AL_ST_AYAR 3;//Alarm saat ayarlama modu
#define MD_AL_DK_AYAR 4;//Alarm dakika ayarlama modu
#define MD_ALARM 5;//Alarm çalma modu

unsigned int SaatModu=0;
const unsigned char seg[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
unsigned char digits[4];
int saat=0,dakika=0,saniye=0;
int alrsaat=1,alrdakika=1;
int yanson=0;
unsigned int DurumSayaci=0;
void delay(unsigned long int ms)
{
  for(unsigned long int i=0;i<ms;i++);
}

void SaniyeArtir()
{
  saniye++;
  if(saniye==60)
  {
    saniye=0;
    dakika++;
    if(dakika==60)
    {
      dakika=0;
      saat++;
      if(saat==24)
        saat=0;
    }
  }
}

void SaatBastir()
{
  int bekleme=1000;
  if(yanson && SaatModu==5)
    P2OUT=0x11;
  else    
    P2OUT=0x01;
  P1OUT=digits[0];
  delay(bekleme);
  if(yanson && SaatModu==5)
    P2OUT=0x12;
  else    
    P2OUT=0x02;
  P1OUT=digits[1];
  delay(bekleme);
  if(yanson && SaatModu==5)
    P2OUT=0x14;
  else    
    P2OUT=0x04;
  P1OUT=digits[2];
  delay(bekleme);
  if(yanson && SaatModu==5)
    P2OUT=0x18;
  else    
    P2OUT=0x08;
  P1OUT=digits[3];
  delay(bekleme);
}


void main( void )
{
WDTCTL = WDTPW + WDTHOLD;//Watchdog timer durduruluyor...
BCSCTL3 |= LFXT1S_2;//ACLK için VLO kullan, yaklaþýk 12Khz
BCSCTL1 = CALBC1_1MHZ;// Set DCO to 1MHz
DCOCTL = CALDCO_1MHZ;
CCR0  = 12000-1;// 12000 saat darbesinde 1 kesme üretilecek
CCTL0 = CCIE;// CCR0 kesmeleri aktif
TACTL = TASSEL_1 + MC_1;	// TimerA için ACLK'i saat olarak seç, upmode
//TACCTL0|=CM_3;
P1DIR = 0xFF;   //P1 çýkýþ...
P2DIR = 0X1F;   //P2.6 ve P2.7 giriþ diðerleri çýkýþ
//port interruptlarýný aktif etme
P2IE_bit.P2IE_5=1;//P2.5 için interruptlarý aktif yapma
P2IES_bit.P2IES_5=1;//P2.5 için interrupt çýkan kenarda
P2IFG_bit.P2IFG_5=0;//P2.5 için interrupt bayraðýný sýfýrla
P2IE_bit.P2IE_6=1;//P2.6 için interruptlarý aktif yapma
P2IES_bit.P2IES_6=1;//P2.6 için interrupt çýkan kenarda
P2IFG_bit.P2IFG_6=0;//P2.6 için interrupt bayraðýný sýfýrla
P2IE_bit.P2IE_7=1;//P2.7 için interruptlarý aktif yapma
P2IES_bit.P2IES_7=1;//P2.7 için interrupt çýkan kenarda
P2IFG_bit.P2IFG_7=0;//P2.7 için interrupt bayraðýný sýfýrla

_BIS_SR(GIE);// kesmeleri aktif et

 while(1)
 {
   DurumSayaci++;
   if(SaatModu == 0 ) //MD_NORMAL
   {
     digits[0]=seg[saat/10];
     digits[1]=seg[saat%10];
     digits[2]=seg[dakika/10];
     digits[3]=seg[dakika%10];
     if(yanson)
       digits[1]&=0x7F;
   }
   
   else if(SaatModu==1)//MD_ST_AYAR
   {
     if(DurumSayaci<50)
     {
        digits[0]=0x88;//A harfi
        digits[1]=0x91;//Y Harfi
        digits[2]=0x88;//A harfi
        digits[3]=0xAF;//r harfi

     }
     else
     {
      if(yanson)
      {
        digits[0]=seg[saat/10];
        digits[1]=seg[saat%10];
        digits[2]=seg[dakika/10];
        digits[3]=seg[dakika%10];
      }
      else
      {
        digits[0]=0xFF;
        digits[1]=0xFF;
        digits[2]=seg[dakika/10];
        digits[3]=seg[dakika%10];
      }
     }
   }
   else if(SaatModu==2)//MD_ST_AYAR
   {
      if(yanson)
      {
        digits[0]=seg[saat/10];
        digits[1]=seg[saat%10];
        digits[2]=seg[dakika/10];
        digits[3]=seg[dakika%10];
      }
      else
      {
        digits[0]=seg[saat/10];
        digits[1]=seg[saat%10];
        digits[2]=0xFF;
        digits[3]=0xFF;
      }
   }
   else if(SaatModu==3)//MD_AL_ST_AYAR
   {
     if(DurumSayaci<50)
     {
        digits[0]=0x88;//A harfi
        digits[1]=0xC7;//L Harfi
        digits[2]=0xAF;//r harfi
        digits[3]=0xFF;
     }
     else
     {
      if(yanson)
      {
        digits[0]=seg[alrsaat/10];
        digits[1]=seg[alrsaat%10];
        digits[2]=seg[alrdakika/10];
        digits[3]=seg[alrdakika%10];
      }
      else
      {
        digits[0]=0xFF;
        digits[1]=0xFF;
        digits[2]=seg[alrdakika/10];
        digits[3]=seg[alrdakika%10];
      }
     }
   }
   else if(SaatModu==4)//MD_AL_DK_AYAR
   {
      if(yanson)
      {
        digits[0]=seg[alrsaat/10];
        digits[1]=seg[alrsaat%10];
        digits[2]=seg[alrdakika/10];
        digits[3]=seg[alrdakika%10];
      }
      else
      {
        digits[0]=seg[alrsaat/10];
        digits[1]=seg[alrsaat%10];
        digits[2]=0xFF;
        digits[3]=0xFF;
      }
     }   
   else if(SaatModu==5)//MD_ALARM
   {
      if(yanson)
      {
        digits[0]=seg[alrsaat/10];
        digits[1]=seg[alrsaat%10];
        digits[2]=seg[alrdakika/10];
        digits[3]=seg[alrdakika%10];
      }
      else
      {
        digits[0]=0xFF;
        digits[1]=0xFF;
        digits[2]=0xFF;
        digits[3]=0xFF;
      }
     }   
 SaatBastir();
 }
}


#pragma vector=TIMERA0_VECTOR
__interrupt void TIMERA0_ISR(void) {
  yanson=!yanson;
  if(dakika==alrdakika && saat==alrsaat && SaatModu==0)
    SaatModu=5;//Alarm çalma moduna geç
  if(yanson && SaatModu==0)
  {
    SaniyeArtir();
  }
  else if(SaatModu==5)
  {
    if(yanson)
      SaniyeArtir();
    if(dakika!=alrdakika || saat!=alrsaat)
      SaatModu=0;
  }
  _BIC_SR_IRQ(LPM3_bits);
}

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
  if(P2IFG_bit.P2IFG_7==1)
  {
    if(SaatModu!=5)
    {
      SaatModu=(SaatModu+1)%5;//Mod deðiþtirme
      DurumSayaci=0;
    }
    P2IFG_bit.P2IFG_7=0; //P2.7 kesme bayraðý temizleniyor.
  }
  else if(P2IFG_bit.P2IFG_6==1)
  {
    if(SaatModu==1)//MD_ST_AYAR
    {
      saat=(saat+1)%24;
    }
    else if(SaatModu==2)//MD_ST_AYAR
    {
      dakika=(dakika+1)%60;
    }
    else if(SaatModu==3)//MD_AL_ST_AYAR
    {
      alrsaat=(alrsaat+1)%24;
    }
    else if(SaatModu==4)//MD__AL_DK_AYAR
    {
      alrdakika=(alrdakika+1)%60;
    }
    P2IFG_bit.P2IFG_6=0; //P2.6 kesme bayraðý temizleniyor.
  }
  else
  {
    if(SaatModu==1)//MD_ST_AYAR
    {
      saat=(saat-1+24)%24;
    }
    else if(SaatModu==2)//MD_ST_AYAR
    {
      dakika=(dakika-1+60)%60;
    }
    else if(SaatModu==3)//MD_AL_ST_AYAR
    {
      alrsaat=(alrsaat-1+24)%24;
    }
    else if(SaatModu==4)//MD__AL_DK_AYAR
    {
      alrdakika=(alrdakika-1+60)%60;
    }
    P2IFG_bit.P2IFG_5=0; //P2.5 kesme bayraðý temizleniyor.
  }
}