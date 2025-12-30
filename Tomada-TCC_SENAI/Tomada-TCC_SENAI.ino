    /*** Configuration ****/

// bibliotecas
#include "config.h"
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

/*** Example Starts Here ****/

// pino 2 é Tomada
#define LED_PIN1 2
// setando o nome da variável do adafruit
AdafruitIO_Feed *digital1 = io.feed("lampada3");
boolean connectWifi();

//callback functions
void Funcion_pino2(uint8_t brightness);

// Change this!!
const char* ssid = "teste";
const char* password = "11223344";

boolean wifiConnected = false;

Espalexa espalexa;


void setup() {
  
  pinMode(LED_PIN1, OUTPUT);
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'digital' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  digital1->onMessage(handleMessage1);
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
    wifiConnected = connectWifi();
  
  if(wifiConnected){
    
    // Define your devices here. 
    espalexa.addDevice("tomadinha", Funcion_pino2); //simplest definition, default state off
    espalexa.begin();
    
  } else
  {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }

  Serial.println();
  Serial.println(io.statusText());
  // get pra puxar a informação do adafruit
  digital1->get();
}

void loop() {

  //Conexão com adafruit e alexa
  io.run();
  espalexa.loop();
  delay(1);

}
//Verificando conexao da Alexa
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}

// this function is called whenever an 'digital' feed message
// is received from Adafruit IO. it was attached to
// the 'digital' feed in the setup() function above.

// Void do botão do adafruit
void handleMessage1(AdafruitIO_Data *data) {
  digitalWrite(LED_PIN1, data->toPinLevel());
}
// Void da alexa
void Funcion_pino2(uint8_t brightness){
  if(brightness){
    digitalWrite(LED_PIN1,LOW);
  } else {
    digitalWrite(LED_PIN1,HIGH);
  }
}
