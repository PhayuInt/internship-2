#include "HX711.h"
#define DOUT  3
#define CLK  2
HX711 scale(DOUT, CLK);
float Zero,Weight,value,b,c;

void setup() {

  Serial.begin(115200);
  Serial.print("Wait...");
  Serial.println();
  Zero = scale.read_average(50); //น้ำหนักเริ่มต้น
 
}

void loop() {
     value = scale.read_average(35); //ค่าน้ำหนักที่รับ
      b = value-Zero;
      Weight = (((value - Zero)/126)); //ค่าน้ำหนักที่ได้
     if(b < 250){
         Weight = 0;
    
      }
    Serial.print(" Reading: ");
    Serial.print(round(Weight));
    Serial.print(" g");
    Serial.println();
     
  delay(3000);

}
