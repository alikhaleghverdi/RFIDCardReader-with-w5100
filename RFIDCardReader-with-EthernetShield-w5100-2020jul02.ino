#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
//#include <DateTime.h>
//#include <DateTimeStrings.h>

#define RST_PIN 7
#define SS_PIN 6

MFRC522 mfrc522(SS_PIN, RST_PIN);

String request;
unsigned long refreshCounter = 0;
IPAddress ip(192,168,1,150);
byte mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

EthernetServer server(80);
EthernetClient client;

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  
  // disable ethernet and SD SPI
  pinMode(10,OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4,OUTPUT);
  digitalWrite(4, HIGH);

  Serial.println(F("Initialize System"));
  Ethernet.begin(mac, ip);
  while(!Ethernet.begin(mac)) 
  {
    Serial.println(F("Failed!!  \n Retrying in 1 Seconds..."));
    delay(1000);
    Serial.print(F("Starting W5100 ..."));
  }
  pinMode(2,OUTPUT);
  server.begin();
  Serial.print("IP Address : ");
  Serial.print(Ethernet.localIP());
  
  mfrc522.PCD_Init();
  Serial.println("Card Here");
  Serial.println();
  delay(1000);

  
}
void loop() {

  client = server.available();
  clientRequest();
  handleRequest();

//  if(!mfrc522.PICC_IsNewCardPresent())
//  {
//    return;
//  }
//  if(!mfrc522.PICC_ReadCardSerial())
//  {
//    return;
//  }
//  Serial.print("UID Tag : ");
//  String content= "";
//  byte letter;
//  for(byte i = 0;i<mfrc522.uid.size; i++)
//  {
//    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//    Serial.print(mfrc522.uid.uidByte[i], HEX);
//    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
//    content.concat(String(mfrc522.uid.uidByte[i], HEX));
//  }
//  Serial.println();
//  content.toUpperCase();
//  if(content.substring(1) == "09 C2 E6 A2")
//  {
//    Serial.println("WELCOME ALI");
//    Serial.println();
//    delay(3000);
//  }
//  else if(content.substring(1) == "59 91 1C 98")
//  {
//    Serial.println("WELCOME REZA");
//    Serial.println();
//    delay(3000);
//  }
//  else{
//    Serial.println("ACCESS DENIED");
//    delay(3000);
//  }
  delay(2000);

}

void clientRequest()
{
  if(!client)
  {
    return;
  }
  while(!client.available())
  {
    delay(1);
  }
  request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
}
void handleRequest()
{
  if(request.indexOf("/dig2on") > 0)
  {
    digitalWrite(2, HIGH);
  }
  if(request.indexOf("/dig2off") >0)
  {
    digitalWrite(2, LOW);
  }
  if(request.indexOf("GET")>=0)
  {
    webpage(client);
    client.stop();
  }
}
void webpage(EthernetClient client)
{
  client = server.available();
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println();
  //client.println(F("<DOCTYPE html>"));
  //client.println(F("<html>"));
  client.println(F("<head>"));
  client.println(F("<title>Arduino RFID RC522 with W5100</title>"));
  client.println(F("<meta charset='utf-8'>"));
  client.println(F("<meta http-equiv='refresh' content='5'>"));
  client.println(F("<style> p{ color:white;} </style>"));
  client.println(F("</head>"));
  client.println(F("<body style='background-color:#333333;'>"));
  client.println(F("<h1 style='color:#ffffff;'>دانشگاه علم و فرهنگ</h1>"));
  client.println(F("<hr><h2 style='color:green;'>پروژه برنامه سازی سیستم - استاد علی صالحی</h2>"));
  client.println(F("<p style='color:#ffffff;'>گزارشات</p>"));

  if(!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if(!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Serial.print(" UID TAG : ");
  client.println(F("<p> UID Tag | Name | Time | Type </p>" ));
  String content= "";
  byte letter;
  for(byte i = 0;i<mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
    client.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    client.print(mfrc522.uid.uidByte[i], HEX);
    //byte UID = mfrc522.uid.uidByte[i];
  }
  Serial.println();
  
  content.toUpperCase();
  if(content.substring(1) == "09 C2 E6 A2")
  {
    Serial.println("WELCOME ALI");
    Serial.println();
    client.println(F("<p> Ali | xxxx | xxxx | " /*time_t now();*/ " GRANTED </p>"));
    delay(3000);
  }
  else if(content.substring(1) == "59 91 1C 98")
  {
    Serial.println("WELCOME REZA");
    Serial.println();
    client.println(F("<p> Reza | xxxx | xxxx | " /*time_t now();*/ " GRANTED </p>"));
    delay(3000);
  }
  else{
    Serial.println("ACCESS DENIED");
    client.println(F("<p> NaN | xxxx | xxxx | " /*time_t now();*/ " DENIED </p>"));
    client.println(F("<script> alert('Access Denied');</script>"));
    delay(3000);
  }
  delay(2000);

  client.println(F("</tbody></body></html>"));
}
