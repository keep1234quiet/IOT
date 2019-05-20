/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define USE_SERIAL Serial

bool isConnected = false;


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
      isConnected = false;
			break;
		case WStype_CONNECTED: {
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
      isConnected = true;

			// send message to server when Connected
//			webSocket.sendTXT("Connected");//发送Conneted 导致服务器抛出 Expecting value: line 1 column 1 (char 0) 异常而断开连接。//先注释掉试试
		}
			break;
		case WStype_TEXT:
			 USE_SERIAL.printf("[WSc] get text: %s\n", payload);
  		 // send message to server
       //webSocket.sendTXT(rcvMsg);
			 break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
	}

}

void setup() {
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(115200);

	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	WiFiMulti.addAP("YOUR-WIFI-NAME", "YOURWIFI-PASSWORD");

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
	webSocket.begin("192.168.43.102", 8000, "/ws/chat/M/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
////webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);  //这个时间很准，可以重wireshark里面查看，ESP8266每15秒钟Ping一次服务器，期间会收到服务器的Ping,ESP8266也会Pong回去
//等下查看一下服务器端的Ping时间间隔在哪
}




int count = 0;
String inputString = "";
boolean stringComplete = false;
char sendStr[100];

void loop() {
	webSocket.loop();
  serialEvent();
  count++;
  if(count == 60*100){
      count = 0;
      if(isConnected){
          webSocket.sendTXT("{\"message\":\"heartbeat\"}"); 
      }
  }
  if (stringComplete) {
    String s1 = "{\"message\":\"";
    String s2 = "\"}";
    webSocket.sendTXT(s1+inputString+s2);
    // clear the string:
    inputString = "";
    stringComplete = false;
    memset(sendStr,0,100);
  }
  delay(10);
}


void serialEvent() {
  while (Serial.available() && !stringComplete) {
    // get the new byte:
    char inChar = (char)Serial.read();
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      return;
    }
    
    // add it to the inputString:
    inputString += inChar;
  }
}






