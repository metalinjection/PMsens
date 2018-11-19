

//******************************
 //*Abstract: Read value of PM1,PM2.5 and PM10 of air quality
 //
 //*Version：V3.1
 //*Author：Zuyang @ HUST
 //*Modified by Cain for Arduino Hardware Serial port compatibility
 //*Date：March.25.2016
 //******************************
#include <M5Stack.h>
//#include <Arduino.h>
#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
unsigned char buf[LENG];

int PM01Value=0;          //define PM1.0 value of the air detector module
int PM2_5Value=0;         //define PM2.5 value of the air detector module
int PM10Value=0;         //define PM10 value of the air detector module

byte bDistance = 30, bPosY = 50, bPosX = 145;

void scrSetup(void);
void scrCyc(void);

void setup()
{
  M5.begin();
  M5.Serial2.begin(9600);   //use serial0
  M5.Serial2.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
  M5.Serial0.begin(115200);

  scrSetup();

  //M5.Speaker.tone(440, 2);
  //M5.Speaker.tone(660, 2);
  //M5.Speaker.mute();
}

void scrSetup(void)
{
  M5.Lcd.setBrightness(20);
  M5.Lcd.fillScreen(BLUE);
  M5.Lcd.fillRoundRect( 10, 10, 300, 220, 10, RED);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(0x555555);
  
  M5.Lcd.setCursor( 20,(bPosY + (bDistance * 0)));
  M5.Lcd.print( "Current PM Value");

  M5.Lcd.setTextColor(ORANGE);
  M5.Lcd.setCursor( 20,(bPosY + (bDistance * 1)));
  M5.Lcd.print( "PM 1.0:    ug/m3");
  M5.Lcd.setCursor( 20,(bPosY + (bDistance * 2)));
  M5.Lcd.print( "PM 2.5:    ug/m3");
  M5.Lcd.setCursor( 20,(bPosY + (bDistance * 3)));
  M5.Lcd.print( "PM10.0:    ug/m3");
}

void scrCyc(void)
{
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.fillRect( bPosX, (bPosY + bDistance), (55), (bPosY + (2* bDistance)), RED);
  M5.Lcd.setCursor( bPosX,(bPosY + (bDistance * 1)));
  M5.Lcd.printf( "%03d", PM01Value);
  M5.Lcd.setCursor( bPosX,(bPosY + (bDistance * 2)));
  M5.Lcd.printf( "%03d", PM2_5Value);
  M5.Lcd.setCursor( bPosX,(bPosY + (bDistance * 3)));
  M5.Lcd.printf( "%03d", PM10Value);  
}

void loop()
{
  if(M5.Serial2.find(0x42))
  {    //start to read when detect 0x42
    M5.Serial2.readBytes(buf,LENG);

    if(buf[0] == 0x4d)
    {
      if(checkValue(buf,LENG)){
        PM01Value=transmitPM01(buf); //count PM1.0 value of the air detector module
        PM2_5Value=transmitPM2_5(buf);//count PM2.5 value of the air detector module
        PM10Value=transmitPM10(buf); //count PM10 value of the air detector module 
      }           
    } 
  }
  

  static unsigned long OledTimer=millis();  
  if (millis() - OledTimer >=1000) 
  {
    OledTimer=millis();
    
    scrCyc();

    M5.Serial0.print("PM1.0: ");  
    M5.Serial0.print(PM01Value);
    M5.Serial0.println("  ug/m3");            
    
    M5.Serial0.print("PM2.5: ");  
    M5.Serial0.print(PM2_5Value);
    M5.Serial0.println("  ug/m3");     
      
    M5.Serial0.print("PM1 0: ");  
    M5.Serial0.print(PM10Value);
    M5.Serial0.println("  ug/m3");   
    M5.Serial0.println();
  }
}

char checkValue(unsigned char *thebuf, char leng)
{  
  char receiveflag=0;
  int receiveSum=0;

  for(int i=0; i<(leng-2); i++){
  receiveSum=receiveSum+thebuf[i];
  }
  receiveSum=receiveSum + 0x42;
 
  if(receiveSum == ((thebuf[leng-2]<<8)+thebuf[leng-1]))  //check the serial data 
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

int transmitPM01(unsigned char *thebuf)
{
  int PM01Val;
  PM01Val=((thebuf[3]<<8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}

//transmit PM Value to PC
int transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val=((thebuf[5]<<8) + thebuf[6]);//count PM2.5 value of the air detector module
  return PM2_5Val;
  }

//transmit PM Value to PC
int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val=((thebuf[7]<<8) + thebuf[8]); //count PM10 value of the air detector module  
  return PM10Val;
}
