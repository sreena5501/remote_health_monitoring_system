#include <Event.h>
#include <Timer.h>
#include <ThingSpeak.h>
#define USE_ARDUINO_INTERRUPTS true
#define DEBUG true
#define SSID "wifi"
#define PASS "innovate"
#define IP"184.106.153.149"
#include<SoftwareSerial.h>
#include"Timer.h"
#include<PulseSensorPlayground.h>
Timer t;
PulseSensorPlayground pulseSensor;
String msg="GET/update?key=1APFH0GIFHFL8YWR";
SoftwareSerial esp8266(10,11);


const int PulseWire=A0;
const int LED13=13;
int Threshold=550;
float myTemp;
int myBPM;
String BPM;
String temp;
int error;
int panic;
int raw_myTemp;
float Voltage;
float tempC;
void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200);
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13);
  pulseSensor.setThreshold(Threshold);

  if(pulseSensor.begin()){
    Serial.println("We created a pulse Sensor");
  }
  Serial.println("AT");
  esp8266.println("AT");


  delay(3000);

  if(esp8266.find("OK"))
  {
    connectWiFi();
  }
  t.every(10000,getReadings);
  t.every(10000,updateInfo);
}
void loop()
{
  panic_button();
  start:
  error=0;
  t.update();
  if(error==1)
  {
    goto start;
  }
  delay(4000);
}
void updateInfo()
{
  String cmd="AT+CIPSTART=\"TCP\",\"";
  cmd+=IP;
  cmd+="\",80";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(2000);
  if(esp8266.find("error"))
  {
    return;
  }
  cmd = msg;
  cmd +="&field1=";
  cmd += BPM;
  cmd +="field=";
  cmd +=temp;
  cmd +="\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  esp8266.println(cmd.length());
  if(esp8266.find(">"))
  {
    Serial.print(cmd);
    esp8266.print(cmd);
  }
  else
  {
    Serial.println("AT+CIPCLOSE");
    esp8266.println("AT+CIPCLOSE");
    error=1;
  }
}
boolean connectWiFi()
{
  Serial.println("AT+CWMODE=1");
  esp8266.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(5000);
  if(esp8266.find("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}
void getReadings(){
  raw_myTemp =analogRead(A1);
  Voltage =(raw_myTemp/1023.0)*5000;
  tempC=Voltage*0.1;
  myTemp = (tempC*1.8)+3;
  Serial.println(myTemp);
  int myBPM = pulseSensor.getBeatsPerMinute();
  if(pulseSensor.sawStartOfBeat()){
    Serial.println(myBPM);
  }
  delay(20);
  char buffer1[10];
  char buffer2[10];
  BPM =dtostrf(myBPM,4,1,buffer1);
  temp =dtostrf(myTemp,4,1,buffer2);
}
void panic_button(){
  panic =digitalRead(8);
  if(panic == HIGH){
    Serial.println(panic);
    String cmd ="AT+CIPSTART=\"TCP\",\"";
    cmd+=IP;
    cmd+="\",80";
    Serial.println(cmd);
    esp8266.println(cmd);
    delay(2000);
    if(esp8266.find("error"))
    {
      return;
    }
    cmd=msg;
    cmd+="&field3=";
    cmd+=panic;
    cmd+="\r\n";
    Serial.print("AT+CIPSEND=");
    esp8266.print("AT+CIPSEND=");
    Serial.println(cmd.length());
    esp8266.println(cmd.length());
    if(esp8266.find(">"))
    {
      Serial.print(cmd);
      esp8266.print(cmd);
    }
    else
    {
      Serial.println("AT+CIPCLOSE");
      esp8266.println("AT+CIPCLOSE");
      error=1;
    }
  }
}
