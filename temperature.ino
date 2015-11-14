#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2); 

byte dht_dpin = 7; // пин за комуникация със сензора
byte dht_dat[5];   // масив за данните от сензора

void setup(){ // начални инициализации

  InitDHT(); // инициализира сензора
  lcd.init();                            //     Инициализация на LCD
  delay(100);
  lcd.clear();                         //     Изстриване на екрана
  delay(100);
  lcd.setBacklight(0);
  lcd.setCursor(2,0);              //     Позициониране курсора на 3 символ 1 ред
  delay(100);
  delay(1000); // изчаква 1 секунда
}

void loop(){ // програмен цикъл

  StartDHT22(); // започва комуникация със сенозра

  for (byte i=0; i<5; i++)   // повтаря се 5 пъти
    dht_dat[i] = readByte(); // извиква фунцкията за четене на байт от сензора

   //// проверява правилно си са получени данните
   byte checksum = dht_dat[0] + dht_dat[1] + dht_dat[2 ] + dht_dat[3];	 
   if(dht_dat[4] == checksum) { // ако данните са коректни

   //// намира цялата и дробната част на относителната влажност
        int HighByte = dht_dat[0]; // първи байт
        int LowByte = dht_dat[1];  // втори байт
        int TReading = (HighByte << 8) + LowByte; // обединява байтовете
        int HWhole = TReading / 10; // намира цялата част
        int HFract = TReading % 10; // намира дробната част

    
        HighByte = dht_dat[2];  // трети байт
        LowByte = dht_dat[3];   // четвърти байт
        TReading = (HighByte << 8) + LowByte; // обединява байтовете
        int TWhole = TReading / 10;  // намира цялата част
        int TFract = TReading % 10;  // намира дробната част

    //// извежда стойността в Serial Monitor
        //lcd.print("temperature = ");
        int SignBit = TReading & 0x8000; // проверява знака на температурата
        if (SignBit)           // ако е отрицателна
           lcd.print("-");  // слага минус пред стойността
	lcd.setCursor(0,0);
        lcd.print("Temp: " + String(TWhole) + "." + String(TFract) + "C");
        lcd.setCursor(0,1);
        lcd.print("Humidity: " + String(HWhole) + "." + String(HFract) + "%");
        //lcd.clear();
}

  delay(800); // закъснение, необходимо за да се продотврати прегряване 
}

/////////////////////// инициализира сензозра

void InitDHT(){

  pinMode(dht_dpin,OUTPUT); // пина за комуникация става изходен
  digitalWrite(dht_dpin,HIGH); // вдига линията на ниво HIGH
}

/////////////////////// започва комуникация със сензора

void StartDHT22(){

//// изпраща начален сигнал  
  pinMode(dht_dpin,OUTPUT);    // пина за комуникация става изходен
  digitalWrite(dht_dpin,LOW);  // сваля линията на ниво LOW и
  delay(1);                    // изчаква 1 милисекунда
  digitalWrite(dht_dpin,HIGH); // вдига линията на HIGH и
  delayMicroseconds(40);       // изчаква 40 микросекунди

//// изчаква отговор от сензора  
  pinMode(dht_dpin,INPUT);             // пина за комуникация става входен
  while(digitalRead(dht_dpin) == LOW); // изчаква линията да стане LOW
  while(digitalRead(dht_dpin) == HIGH);// изчаква да се върне на HIGH
}

//////////////////////// прочита един байт от сензора

byte readByte(){

  byte result = 0; // съхранява стойността на байта

  for(byte i=0; i< 8; i++){ // прочита 8 бита от линията

      while(digitalRead(dht_dpin) == LOW); // изчаква 50us от началото на бита
      delayMicroseconds(45);               // изчаква още 45us
      if (digitalRead(dht_dpin) == HIGH)   // проверява линията дали е HIGH
 	  result |= (1 << (7 - i));        // счита байта за 1 и го добавя на
                                           // съответната позиция в байта                
      while (digitalRead(dht_dpin)==HIGH); // изчаква сигнал за нов бит
    }

  return result; // връща стойността на байта
}
