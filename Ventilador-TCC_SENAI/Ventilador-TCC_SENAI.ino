//Chamando as bibliotecas
#include "config.h"
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>
#include <DHT.h>

// criando as variavel
int botão = 0;
int alex = 0;
int returnu = 0;
int returno = 0;
#define Pin 0
#define DHTPIN 2    // Pino de dados conectado ao DHT22
#define DHTTYPE DHT22   // Tipo do sensor DHT

DHT dht(DHTPIN, DHTTYPE);

//Criando váriavel para o feed do adafruit
AdafruitIO_Feed *digital1 = io.feed("temperatura");
AdafruitIO_Feed *digital2 = io.feed("umidade");
AdafruitIO_Feed *digital3 = io.feed("ventilador");

boolean connectWifi();

//callback functions
void Funcion_pino2(uint8_t brightness);

// Change this!!
const char* ssid = "teste";
const char* password = "11223344";

boolean wifiConnected = false;

Espalexa espalexa;


void setup() {
  pinMode(Pin, OUTPUT);
  // start the serial connection
  Serial.begin(115200);
  dht.begin();

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // received from adafruit io.
 digital3->onMessage(handleMessage2);
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
    wifiConnected = connectWifi();
  
  if(wifiConnected){
    
    // Define your devices here. 
    espalexa.addDevice("ventiladorzinho", Funcion_pino2); 
    espalexa.begin();
    
  } else
  {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  //get da váriavel do adafruit
  digital3->get();
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.

  //Conectando a alexa e o adafruit
  espalexa.loop();
  delay(1);
   io.run();
   //chamando o botão
  digital3->onMessage(handleMessage2);
  //váriaveis para o sensor DHT
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Verifica se a leitura foi bem-sucedida
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler o sensor DHT!");
    return;
  }
  // Comparação para ativar o ventilador
if(botão == 1 || alex == 1){
  digitalWrite(Pin, LOW);
}
if(botão == 0 & alex == 0){
if(temperature >= 30){
  digitalWrite(Pin, LOW);
}
if(temperature < 30){
   digitalWrite(Pin, HIGH);
}
 }
  // Imprime os dados no console serial
  Serial.print("Umidade: ");
  Serial.print(humidity);
  Serial.print("%\t");

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("botao: ");
  Serial.println(botão);
  //Programação para enviar os dados para o adafruit (o save é quem está enviando os dados)
  if(temperature != returnu){
  delay(4000);
  digital1->save(temperature);
  returnu = temperature;
  }
  if(humidity != returno){
  delay(4000);
  digital2->save(humidity);
  returno = humidity;
  }
}
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

//Void do botão do adafruit
void handleMessage2(AdafruitIO_Data *data) {
  if(data->toPinLevel() == 0){
    botão = 1;
  }
  else{
    botão = 0;
  }
}
//Void da alexa
void Funcion_pino2(uint8_t brightness){
  if(brightness){
    alex = 1;
  } else {
    alex = 0;
  }
}
