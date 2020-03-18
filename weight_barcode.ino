#include <Keyboard.h>
#include <HID.h>
#include <LiquidCrystal.h>
#include "HX711.h"
#include <hid.h>                          
#include <hiduniversal.h>                 
#include <usbhub.h>
#include <usbhid.h>
#include <hidboot.h>
#include <avr/power.h>
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include <Usb.h>
#include <usbhub.h>
#include <avr/pgmspace.h>
#include <hidboot.h>

#define DISPLAY_WIDTH 16

LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

USB     Usb;
USBHub     Hub(&Usb);           //I enable this line                                
HIDUniversal      Hid(&Usb);   //Add this line so that the barcode scanner will be recognized, I use "Hid" below 

#define DOUT  3
#define CLK  2
HX711 scale(DOUT, CLK);
float base_weight,Weight,value,b,c;


String Barcodes[13];
String Keep_weight;
char ctrlKey = KEY_BACKSPACE;

class BTHKBAdapter : public HIDReportParser 
{
    protected:
    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t Length, uint8_t *buf);
    
};

class KbdRptParser : public KeyboardReportParser
{
  void PrintKey(uint8_t mod, uint8_t key);              // Add this line to print character in ASCII        
  protected:
  virtual void OnKeyDown  (uint8_t mod, uint8_t key);
  virtual void OnKeyPressed(uint8_t key);
  
 
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)  
{
  uint8_t c = OemToAscii(mod, key); 
    if (c)
        OnKeyPressed(c);
}

int i = 0;
void KbdRptParser::OnKeyPressed(uint8_t key)  
{
static uint32_t next_time = 0;      
static uint8_t current_cursor = 0; 

if(key){
  
  if(key == 19){ //
      Weight = (scale.read_average(20) - base_weight) / 10;
      if(Weight < 0){
         Weight = 0;   
      }
      else{
        value = scale.read_average(30); //น้ำหนักที่ได้ในแต่ละค่า
        Weight = (((value - base_weight)/389)); //หาค่าน้ำหนัก
        
      }
      
      Keep_weight = String(Weight);
 
      Serial.println();
      Serial.println("Weight: " + Keep_weight + " g");
      
      String tacking = ""+Barcodes[0]+ //บาร์โค๊ด 13 ตัว
      Barcodes[1]+
      Barcodes[2]+
      Barcodes[3]+
      Barcodes[4]+
      Barcodes[5]+
      Barcodes[6]+
      Barcodes[7]+
      Barcodes[8]+
      Barcodes[9]+
      Barcodes[10]+
      Barcodes[11]+
      Barcodes[12]+
      "";
      Keep_weight = String(round(Weight));
      while (digitalRead(4) == HIGH) {
          delay(200);
        }
        //Keyboard input devices 
      Keyboard.print(tacking);
      Keyboard.press(KEY_TAB);
      Keyboard.releaseAll();
      Keyboard.print((Keep_weight));
      Keyboard.releaseAll();

      i = 0;  
  }else{ 
    if(key < 65){  //สแกนบาร์โค๊ดบรูทูธ ต้องปรับค่า
      if(key == 41)
      {
        key = key+7;//0
      }
      if(key == 33 || key == 35 || key == 36 || key == 37 || key == 40){
        key = key+16;//1 and 3 4 5 6 8
      }
      if(key == 64){
        key = key-14;//2
      }
       if(key == 38){
        key = key+17;//7
      }
      if(key == 40){
       key = key+13;//9
      }   
    }
    Barcodes[i] = (char)key;
    Serial.print((char)key);
    Serial.print(" ");
   
    i = i+1;
  }
}
else{
  Serial.print("++");
  }
};
KbdRptParser Prs;
void setup() {
  Keyboard.begin();
  Serial.begin(115200);
  Serial.print("Start...");
  Serial.println();
  if (Usb.Init() == -1) {
        Serial.println("OSC did not start."); //เช็คการเชื่อมต่อของUSB
   }
  delay( 100 );
  Hid.SetReportParser(0, (HIDReportParser*)&Prs);    //Here I change  "Keyboard" for "Hid"
  base_weight = scale.read_average(30); // น้ำหนักเริ่มต้น
   delay( 100 );
}
void loop() {
  Usb.Task();
}
