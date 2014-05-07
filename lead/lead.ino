#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <HMC5883L.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
SoftwareSerial softSerial(10, 11); // RX, TX
String communication = "";
int order;
HMC5883L compass;

void setup() {
  // init serial
  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.println("Serial ok");

  // init ble
  softSerial.begin(9600);

  // init lcd
  lcd.begin(16,2);
  lcd.print("hello, world!");

  // init compass
  int error = 0;
  Wire.begin();
  compass = HMC5883L();
  error = compass.SetScale(1.3);
  if(error != 0)
    Serial.println(compass.GetErrorText(error));
  error = compass.SetMeasurementMode(Measurement_Continuous);
  if(error != 0)
    Serial.println(compass.GetErrorText(error));
}

void loop() {
  if(softSerial.available()){
    lcd.noAutoscroll();
    lcd.clear();
    Serial.println(order);
    lcd.print(order);
  }

  readCompass();
}

String readBle(){
  communication="";
  while (softSerial.available()){
    communication.concat(char(softSerial.read()));
    delay(10);
  }
  return communication;
}

int readCompass(){
  MagnetometerRaw raw = compass.ReadRawAxis();
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  float declinationAngle = 0.0457;
  heading += declinationAngle;
  while(heading < 0)
    heading += 2*PI;
  while(heading > 2*PI)
    heading -= 2*PI;
  float headingDegrees = heading * 180/M_PI;
  serialOutput(raw, scaled, heading, headingDegrees);
  lcdOutput(scaled,headingDegrees);

  return roundFloat(headingDegrees);
}

void serialOutput(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees)
{
  Serial.print("Raw:\t");
  Serial.print(raw.XAxis);
  Serial.print("   ");   
  Serial.print(raw.YAxis);
  Serial.print("   ");   
  Serial.print(raw.ZAxis);
  Serial.print("   \tScaled:\t");

  Serial.print(scaled.XAxis);
  Serial.print("   ");   
  Serial.print(scaled.YAxis);
  Serial.print("   ");   
  Serial.print(scaled.ZAxis);

  Serial.print("   \tHeading:\t");
  Serial.print(heading);
  Serial.print(" Radians   \t");
  Serial.print(headingDegrees);
  Serial.println(" Degrees   \t");
}

void lcdOutput(MagnetometerScaled scaled, float headingDegrees)
{
  lcd.clear();
  lcd.autoscroll();
  lcd.print(scaled.XAxis);
  Serial.print("  ");   
  lcd.print(scaled.YAxis);
  Serial.print("  ");   
  lcd.print(scaled.ZAxis);

  lcd.setCursor(0, 1);
  lcd.print(headingDegrees);
}

int roundFloat(float nombre_float) {
  int nombre = int(nombre_float);
  if(nombre_float - nombre  >= 0.5) nombre++;
  return nombre;
}























