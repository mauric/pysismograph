#include <Wire.h>

const byte interruptPin = 2;
volatile boolean inter;
int topofScale;
byte Lista_Devices[4];
int nDevices;
String inputString = "";
String inputString2 = "";
boolean stringComplete = false;
String inputString3 = "";
boolean stringComplete3 = false;

int config_reg[3];

void setup(){
  
  Serial.begin(115200);
  Wire.begin();  // join I2C bus
  
  i2c();

  Serial_input();
  
  pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), edu, FALLING);
  attachInterrupt(0, edu, FALLING);
}

void loop()
{
    
  if (inter ==1)
    {
     Serial.print(millis());
     Serial.print(",");
     Serial.print(getadcReading(0x48, 0x00));
     //Serial.print(",");
     //Serial.print(getadcReading(0x49, 0x00));
     //Serial.print(",");
     //Serial.print(getadcReading(0x4A, 0x00));
     //Serial.print(",");
     //Serial.print(getadcReading(0x4B, 0x00));
     Serial.write('\n');
     Serial.write('\r');
     inter = 0;
    }
    
  while (Serial.available()) {
    char inChar3 = (char)Serial.read();
    inputString3 += inChar3;
    if (inChar3 == '\n') {
      stringComplete3 = true;
    }
  }
  
    if (stringComplete3) {
    if (inputString3 == "config\n")
       {inputString = "";
        inputString2 = "";
        Serial_input();}
    inputString3 = "";
    stringComplete3 = false;
    }
  
}

int getadcReading(uint8_t deviceAdd, uint8_t reg)
    {      
      int data;
      Wire.beginTransmission(deviceAdd); // transmit to I2c device address
      Wire.write(reg);                   // point to device register 0 
      Wire.endTransmission();            // stop transmitting
      Wire.requestFrom(deviceAdd, 2);    // request 2 bytes from slave device #2
      while(Wire.available())    // need two bytes, MSB and LSB of converstion value
        { data = Wire.read();  //get MSB of reading
          data = data << 8;       // shift it to high byte of data
          data = data + Wire.read(); // add LSB to data
        }
      return data;
    }


void setConfiguration(uint8_t deviceAdd, uint8_t reg, uint16_t confWord)
    { Wire.beginTransmission(deviceAdd); // transmit to ADS1115 device
      Wire.write(reg);                   // point to configuration register
      Wire.write(highByte(confWord));      // sends MSB of configuration word
      Wire.write(lowByte(confWord));       // sends LSB of configuration word
      Wire.endTransmission();            // stop transmitting
     }

void edu(void) 
    {inter = 1;
    }

void i2c()
{
  byte error, address;
  //Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      nDevices++;
      Lista_Devices[nDevices] = address;
    }
   
  }
  if (nDevices == 0)
    {//Serial.println("No I2C devices found\n"); 
  }
  else
    {//Serial.print("nro de Devices = ");
     //Serial.println(nDevices);
    for (int i = 1; i < (nDevices + 1); i++)
      { //Serial.print("Device num: ");
        //Serial.print(Lista_Devices[i],HEX);
        //Serial.print(" HEX; ");
        //Serial.print(Lista_Devices[i],BIN);
        //Serial.print(" BIN; ");
        //Serial.print(Lista_Devices[i],DEC);
        //Serial.println(" DEC");
      }
    }
    delay(1000);
}


void Serial_input()
{
//  while (inputString != "fin\n"){
//  while (Serial.available()) {
//    char inChar = (char)Serial.read();
//    inputString += inChar;
//    if (inChar == '\n') {
//      stringComplete = true;
//    }
//  }
//  
//    if (stringComplete) {
//    //Serial.println(inputString);
//    if (inputString != "fin\n"){
//    inputString2 = inputString;
//    inputString = "";
//    }
//    stringComplete = false;
//    }
//  }

  inputString2 = "32768,32767,17056";
  int Primer_coma =  inputString2.indexOf(',');
  int Segunda_coma = inputString2.indexOf(',',Primer_coma + 1);
  
  String reg3 = inputString2.substring(0,Primer_coma);
  String reg2 = inputString2.substring(Primer_coma + 1, Segunda_coma);
  String reg1 = inputString2.substring(Segunda_coma + 1);

  config_reg[3] = reg3.toInt();
  config_reg[2] = reg2.toInt();
  config_reg[1] = reg1.toInt();
  
//  Serial.print("Registro 3 = ");
//  Serial.println(config_reg[3]);
//  Serial.print("Registro 2 = ");
//  Serial.println(config_reg[2]);
//  Serial.print("Registro 1 = ");
//  Serial.println(config_reg[1]);
  
  for (int i = 1; i < (nDevices + 1); i++)
      { //Serial.print("Device num: ");
//        Serial.print(Lista_Devices[i],DEC);
//        Serial.println(" DEC");
//        Serial.print(" registro 03 =");
//        Serial.print(config_reg[3]);
//        Serial.print(" registro 02 =");
//        Serial.print(config_reg[2]);
//        Serial.print(" registro 01 =");
//        Serial.println(config_reg[1]);
        setConfiguration(Lista_Devices[i], 0x03, config_reg[3]);
        setConfiguration(Lista_Devices[i], 0x02, config_reg[2]);
        setConfiguration(Lista_Devices[i], 0x01, config_reg[1]);
//        Serial.println("...");
//        Serial.print("Leyendo configuración cargada en ");
//        Serial.println(Lista_Devices[i]);
//        Serial.print(" registro 03 =");
//        Serial.print(getadcReading(Lista_Devices[i], 0x03),DEC);
//        Serial.print(" registro 02 =");
//        Serial.print(getadcReading(Lista_Devices[i], 0x02),DEC);
//        Serial.print(" registro 01 =");
//        Serial.println(getadcReading(Lista_Devices[i], 0x01),DEC);
//        Serial.println("...");
      } 
//  
//  setConfiguration(0b01001000, 0b00000010, 0b0111111111111111); //0x48 ready pin
//  setConfiguration(0b01001000, 0b00000011, 0b1000000000000000); //0x48 ready pin
//  setConfiguration(0b01001000, 0b00000001, 0b0100001011100000); //0x48 config 
//  setConfiguration(0b01001001, 0b00000001, 0b0100001011100011); //0x49 config 
//  setConfiguration(0b01001010, 0b00000001, 0b0100001011100011); //0x4A config 
//  setConfiguration(0b01001011, 0b00000001, 0b0100001011100011); //0x4B config 
//  
//  Serial.println(getadcReading(0x48, 0x01),BIN);
//  Serial.println(getadcReading(0x49, 0x01),BIN);
//  Serial.println(getadcReading(0x4A, 0x01),BIN);
//  Serial.println(getadcReading(0x4B, 0x01),BIN);
//
//
//  
  delay(2000);
  
}
