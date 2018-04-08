//   Serra autonoma con sensore di temperatura e umidità (DHT11), igrometro, fotoresistore, display LCD 20x4, sensore livello acqua e relè.
#include <ESP8266WiFi.h>

const char* SSID = "Franco";
const char* PASS = "Fiwoldiois01$";
char server[] = "smtp.gmail.com";

WiFiClient client;

  
const int valore_limite = 27; //Valore dell'igrometro al quale il relay sarà ON
int contatore=0;
byte ret=0;

//DHT11 Sensor:
#include "DHT.h"
#define DHTPIN 15     // Sensore collegato al PIN 8 (15 è 8...)
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void setup(){
  //CONNESSIONE WIFI
  Serial.begin(115200);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP sessione: ");
  Serial.println(WiFi.localIP());
  //FINE CONNESSIONE

// PIN 9 al Relè - IN1
//	pinMode(8, OUTPUT);

Serial.println("Serra Autonoma di Franco Avino");
  
// Avvio sensore DHT11  
	dht.begin();
}


void loop(){
// Lettura umidità e temperatura del sensore DHT11
int h = dht.readHumidity();
int t = dht.readTemperature();

  Serial.print("Temperatura ambiente: ");
  Serial.print(t);
  Serial.println("°C");
  Serial.print("Umidità: ");
  Serial.print(h);
  Serial.println("%");
	
// Igrometro
int igro = analogRead(A0); // Legge il valore analogico
int umdtrr = 0; // Variabile umidità suolo
umdtrr = map (igro, 100, 990, 100, 0); // converto il valore analogico in percentuale
Serial.print("Umidità del terreno: ");
Serial.print(umdtrr);
Serial.println("%"); //Stampa a schermo il valore

if (umdtrr <= valore_limite){
	digitalWrite(8,HIGH); // Attiva Relè 1
  ret = mail(umdtrr,h,t);
}else{
  digitalWrite(8,LOW); // Spegni Relè 1
}

delay(1000);
contatore++;
if (contatore>3600){
  contatore=0;
  ret = mail(umdtrr,h,t);
}
}





void alarm(){
  tone(3,850,700);  // Imposta tono acustico su PIN 3
  delay(150);       // Aspetta per 0,15 secondi
  tone(3,850,700);  // Imposta tono acustico su PIN 3
  delay(150);       // Aspetta per 0,15 secondi
}

void flash() {
  digitalWrite(4, HIGH);   // Imposta il LED su ON
  delay(1000);               // Aspetta per 1 secondo
  digitalWrite(4, LOW);    // Impostiamo il LED su OFF
  delay(1000);               // Aspetta per 1 secondo
}

byte mail(int umdtrr,int umidita,int temperatura){
  byte thisByte = 0;
  byte respCode;

  if (client.connect(server, 465) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!eRcv()) return 0;

  Serial.println(F("Sending EHLO"));
  client.println("EHLO www.example.com");
  if (!eRcv()) return 0;
  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if (!eRcv()) return 0;
  Serial.println(F("Sending User"));
  // Change to your base64, ASCII encoded user
  client.println("c2VycmEuYXV0b21hdGljYUBnbWFpbC5jb20="); // SMTP UserID
  if (!eRcv()) return 0;
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password
  client.println("Rml3b2xkaW9pczAxJA==");//  SMTP Passw
     if (!eRcv()) return 0;
    Serial.println(F("Sending From"));   // change to your email address (sender)
   client.println(F("MAIL From: serra.automatica@gmail.com"));// not important 
   if (!eRcv()) return 0;   // change to recipient address
    Serial.println(F("Sending To"));
    client.println(F("RCPT To: iz4tow@gmail.com"));
    if (!eRcv()) return 0;
    Serial.println(F("Sending DATA"));
    client.println(F("DATA"));
    if (!eRcv()) return 0;
    Serial.println(F("Sending email"));   // change to recipient address
   client.println(F("To: iz4tow@gmail.com"));   // change to your address
   client.println(F("From: serra.automatica@gmail.com"));
 client.println(F("Subject: MAIL DA SERRA AUTOMATICA\r\n"));
    client.print(F("Umidità terreno: "));
    client.print(umdtrr);
    client.println(F("%"));
    client.print(F("Umidità ambiente: "));
    client.print(umidita);
    client.println(F("%"));
    client.print(F("Temperatura ambiente: "));
    client.println(temperatura);
    client.println(F("."));
    if (!eRcv()) return 0;
    Serial.println(F("Sending QUIT"));
    client.println(F("QUIT"));
    if (!eRcv()) return 0;
    client.stop();
    Serial.println(F("disconnected"));
    return 1;
  }
  byte eRcv()
  {
    byte respCode;
    byte thisByte;
    int loopCount = 0;
    while (!client.available())
  {
      delay(1);
      loopCount++;     // if nothing received for 10 seconds, timeout
      if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();
  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    //  efail();
    return 0;
  }
  return 1;
}


