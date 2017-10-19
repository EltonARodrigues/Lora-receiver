# Código com uma comunicação ponto a ponto entre Shields(RF96) e envio dos dados para o broker por meio do ESP8266 01

#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"


#include "SX1272.h"
#include <ArduinoJson.h>

#define WIFI_AP "SEIP7"
#define WIFI_PASSWORD "sorocaba2017"


// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(8,9); // RX, TX

void setup() {
  Serial.begin(38400);
  soft.begin(19200);
  WiFi.init(&soft);
  //Endereço do servidor e porta para envios de dados pelo MQTT
  client.setServer("192.168.0.35",1883);

  int status = WL_IDLE_STATUS;
  
  while ( status != WL_CONNECTED) {
    
    Serial.println(F("Attempting to connect...."));
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(3000);
    if(status){
      Serial.println(F("[FAILED] retrying"));
    }
  }

  sx1272.ON();
  // Set transmission mode
  sx1272.setMode(4);
  // Set header
  sx1272.setHeaderON();
  // Select frequency channel
  sx1272.setChannel(CH_05_900);
  sx1272.setCRC_ON();
  sx1272.setPower('x');  
  sx1272.setNodeAddress(1);
  Serial.println(F("SX1272 successfully configured"));


}

void loop() {
  char my_packet[70];
  if (!sx1272.receivePacketTimeout(1000))
  {
    if ( !client.connected() ) {
      reconnect();
    }
    if(client.connected()){
    
    for (unsigned int i = 0; i < sx1272.packet_received.length; i++)
    {
      my_packet[i] = (char)sx1272.packet_received.data[i];
    }
    Serial.println(F("Message:"));
    Serial.println(my_packet);
    client.publish( "/admin/7b4a/attrs", my_packet ); 

    delay(1000);
    client.loop();
    delay(300);
    //client.disconnect();
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if ( client.connect("SWAN2", NULL, NULL) ) {

      Serial.println(F( "[Connected]" ));
    } 
    else {
      Serial.println(F( "[FAILED] retrying" ));
      delay(5000);
    }
  } 
}

