/*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continuous integration
 *  build.
 *
 *  Limitations:
 *    only RSA certificates
 *    no support of Perfect Forward Secrecy (PFS)
 *    TLSv1.2 is supported since version 2.4.0-rc1
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define DEBUG; //comment out to disable Serial prints

#ifdef DEBUG 
	 #define DEBUG_PRINT(x)       Serial.print(x)
	 #define DEBUG_PRINTDEC(x)    Serial.print(x, DEC)
	 #define DEBUG_PRINTLN(x)     Serial.println(x)
#else
	 #define DEBUG_PRINT(x)
	 #define DEBUG_PRINTDEC(x)
	 #define DEBUG_PRINTLN(x) 
#endif

const char* ssid = "CP";
const char* password = "ChristianPetri1988";

const char* host = "www.entsorgungskalenderanzeige.christianpetri.ch";
//##  PLEASE configure the userConfiguredCirlceId ### 
#define CIRCLE_ID 5

const String circleId =  String(CIRCLE_ID);
const int httpsPort = 443;
const String path = "/plain/";
const String parameter = String("?circleId=" + circleId);

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
//const char* fingerprint = "8e 99 24 7e 5f 65 42 c4 ac 5c 9a ec 1c 15 83 5b f2 d4 5b 78";
const char fingerprint[] PROGMEM = "9620a9ed9756297500f628c9ce9a1e093e3658ae"; //certificate --> until Sunday, 21. July 2019

//Setup LED
const int ledPin[5] = {D8, D7, D6, D5 , D0 };
const int ledPinPower = LED_BUILTIN; //on = HIGH = successful off = LOW = not connected

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 60000;        // will store last time LED was updated
unsigned long previousMillisBlinkLed = 0;        // will store last time the LED  blinked

//Error LEDs
boolean toggleErrorLed = true;

#define ERROR_NO_WIFI 0  
boolean isErrorNoWiFi = false;

#define ERROR_UNABLE_TO_CONNECT_TO_THE_HOST 1 
boolean isErrorUnableToConnectToTheHost = false;
boolean lastKnowStatusErrorUnableToConnectToTheHost = false;

#define ERROR_PAGE_NOT_FOUND 2
boolean isErrorPageNotFound = false;
boolean lastKnowStatusErrorPageNotFound = false;

#define ERROR_API_NOT_CONFIGURED_CORRECTLY 3
boolean isErrorApiNotConfiguredCorrectly = false;
boolean lastKnowStatusErrorApiNotConfiguredCorrectly = false;

#define ERROR_PLEASE_RESTART_MICROCONTROLLER 4 

//ERROR
boolean isThereAConnectionRelatedError = false;
boolean isWifiErrorCleared = true;

// constants won't change:
const long interval = 60000;           // interval at which to blink (milliseconds) --> 60000 ms = 1 minute

void setup() {
	// set the digital pin as output: 
	for(int i = 0; i < (sizeof(ledPin)/sizeof(int)); i++){
		pinMode(ledPin[i], OUTPUT);
		digitalWrite(ledPin[i], HIGH);
	}
	pinMode(ledPinPower, OUTPUT);
	digitalWrite(ledPinPower,LOW); //in this case LOW = LED ON
	delay(1000); 
	turnOffStatusLeds();
	delay(500);
	#ifdef DEBUG
		Serial.begin(115200);
	#endif
	DEBUG_PRINTLN();
	DEBUG_PRINT("connecting to ");
	DEBUG_PRINTLN(ssid);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		DEBUG_PRINT(".");
		//DEBUG_PRINTLN(WiFi.status());
		if (WiFi.status() == WL_NO_SSID_AVAIL) {
			DEBUG_PRINT("The SSID ");
			DEBUG_PRINT(ssid);
			DEBUG_PRINT(" was not found");
			DEBUG_PRINTLN();
			showErrorLed(ERROR_NO_WIFI);
		}
	}
	DEBUG_PRINTLN("");
	DEBUG_PRINTLN("WiFi connected");
	DEBUG_PRINTLN("IP address: ");
	DEBUG_PRINTLN(WiFi.localIP());
	turnOffStatusLeds();
	delay(3000);
	isTheCircleIdSetProperly(path, parameter);
}

// here is where you'd put code that needs to be running all the time.
void loop() {
	if (isWifiError()) {
		tryingToReconnectToTheWifi();
	}
	else if (isItTimeToMakeAnApiCall()) {  
		makeApiCall();
	}
	else if (isThereAConnectionRelatedError) {
		showConnectionRelatedError();
	}
}

boolean isItTimeToMakeAnApiCall() {
	return isIntervalDone() || !isWifiErrorCleared;
}

void makeApiCall() {
	if (isThereAConnectionRelatedError || !isWifiErrorCleared) {
		isWifiErrorCleared = true;
		turnOffStatusLeds();
		DEBUG_PRINTLN("Connected to the WiFi.");
		DEBUG_PRINTLN("Back online?");
	}
	String apiResponse = getDataFromAPI(path, parameter); //+ "demo/"
	isThereAConnectionRelatedError = true;
	if (apiResponse != "fail") {
		if (isAbleToDisplayDataSuccessfully(apiResponse)) {
			isThereAConnectionRelatedError = false;
		}
	}
}

void tryingToReconnectToTheWifi() {
	if (isWifiErrorCleared)
		DEBUG_PRINTLN("Trying to reconnect to the WiFi");
	delay(500);
	DEBUG_PRINT(".");
	showErrorLed(ERROR_NO_WIFI);
	isWifiErrorCleared = false;
}

void turnOffStatusLeds() {
	DEBUG_PRINTLN("Turning off all the status LEDs");
	for (int i = 0; i < (sizeof(ledPin) / sizeof(int)); i++) {
		pinMode(ledPin[i], OUTPUT);
		digitalWrite(ledPin[i], LOW);
	}
}

boolean isWifiError() {
	if (WiFi.status() == WL_DISCONNECTED) { 
		isErrorNoWiFi = true; 
		return true;
	}
	else if (WiFi.status() == WL_CONNECTED) {
		isErrorNoWiFi = false;
		return false;
	} 
}

void showConnectionRelatedError() {
	if (isErrorUnableToConnectToTheHost) {
		showErrorLed(ERROR_UNABLE_TO_CONNECT_TO_THE_HOST);
		if(!lastKnowStatusErrorUnableToConnectToTheHost)
			DEBUG_PRINTLN("Unable to connect to the Host / API");
	}
	else if (isErrorPageNotFound) {
		showErrorLed(ERROR_PAGE_NOT_FOUND);
		if(!lastKnowStatusErrorPageNotFound)
			DEBUG_PRINTLN("API: Page not found (Status != 'HTTP 200 OK'");
	}
	else if (isErrorApiNotConfiguredCorrectly) {
		showErrorLed(ERROR_API_NOT_CONFIGURED_CORRECTLY);
		if(!lastKnowStatusErrorApiNotConfiguredCorrectly)
			DEBUG_PRINTLN("API is not configured correctly");
	}
	lastKnowStatusErrorUnableToConnectToTheHost = isErrorUnableToConnectToTheHost;
	lastKnowStatusErrorPageNotFound = isErrorPageNotFound;
	lastKnowStatusErrorApiNotConfiguredCorrectly = isErrorApiNotConfiguredCorrectly;
}

boolean isIntervalDone(){
	if (millis() - previousMillis >= interval) {
		previousMillis = millis(); 
		return true;
	} 
	return false;
}

String getDataFromAPI(String path, String parameter){
    // Use WiFiClientSecure class to create TLS connection
    WiFiClientSecure client;
    DEBUG_PRINTLN("START: getDataFromAPI");
    DEBUG_PRINTLN("connecting to ");
    DEBUG_PRINTLN(host);
    DEBUG_PRINT("Using fingerprint ");
    DEBUG_PRINTLN(fingerprint);
    client.setFingerprint(fingerprint);
    isErrorUnableToConnectToTheHost = false;
	if (!client.connect(host, httpsPort)) {
		DEBUG_PRINTLN("connection failed"); 
		isErrorUnableToConnectToTheHost = true;
		return "fail";
	}
    DEBUG_PRINT("requesting URL: ");
    DEBUG_PRINTLN(path + parameter);

    client.print(String("GET ") + path + parameter + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: ESP8266\r\n" +
                 "Connection: close\r\n\r\n"); 
                 
    DEBUG_PRINTLN("request sent");
	 
    int counter = 0;
	
    while (client.connected()) { 
		String line = client.readStringUntil('\n'); 
		DEBUG_PRINTLN(line);

		isErrorPageNotFound = false;
		if (line.indexOf("HTTP") >= 0 && !(line.substring(0, 15) == "HTTP/1.1 200 OK")) {  
			DEBUG_PRINTLN("Failed to get the content!");
			DEBUG_PRINTLN(line);
			isErrorPageNotFound = true;
			return "fail";
		}  
		if (line == "\r") {
			DEBUG_PRINTLN("headers received");
			break;
		}
		if(counter > 10000){
			DEBUG_PRINTLN("Time out");
			return "fail";
		}
    } 
      
	DEBUG_PRINTLN("Reading the body of the GET request. Result:");
	String httpGetResponseBody = client.readStringUntil('\n');
	DEBUG_PRINTLN(httpGetResponseBody);
	//now output HTML data header
    client.println("HTTP/1.1 204");
    client.println();
    client.println();
    delay(1);
    //stopping client
    DEBUG_PRINTLN("closing connection");
    client.stop();
	DEBUG_PRINTLN("END: getDataFromAPI");
	return httpGetResponseBody;
}

boolean isAbleToDisplayDataSuccessfully(String apiResponse){
	int result = apiResponse.toInt(); 
    DEBUG_PRINTLN("reply was:");
    DEBUG_PRINTLN("==========");
    DEBUG_PRINTLN(result); 
    DEBUG_PRINTLN("=========="); 

	turnOffStatusLeds();
	isErrorApiNotConfiguredCorrectly = false;

	#ifdef DEBUG
		Serial.flush();
	#endif
	if (result == 200000) {
		DEBUG_PRINTLN("Error: Nothing to do. Is the circleId set correctly?");
		DEBUG_PRINTLN("The circleId was NOT set correctly (for the GET-Request!).");
		DEBUG_PRINTLN("Please recompile the code with the correct ID");
		isErrorApiNotConfiguredCorrectly = true;
		return false;
	} else if(result == 100000){
		DEBUG_PRINTLN("Nothing to do");
    } else {
		if( int(result / 10000) % 10  == 1){
			DEBUG_PRINTLN("Turn on LED 1: green waste");
			digitalWrite(ledPin[0], HIGH);
		} 
		if( int(result / 1000) % 10  == 1){
			DEBUG_PRINTLN("Turn on LED 2: cardboard");
			digitalWrite(ledPin[1], HIGH);
		} 
		if( int(result / 100) % 10  == 1){
			DEBUG_PRINTLN("Turn on LED 3: general waste and 'bulky goods'");
			digitalWrite(ledPin[2], HIGH);
		} 
		if( int(result / 10) % 10  == 1){
			DEBUG_PRINTLN("Turn on LED 4: metal");
			digitalWrite(ledPin[3], HIGH);
		} 
		if( result % 10  == 1){
			DEBUG_PRINTLN("Turn on LED 5: paper");
			digitalWrite(ledPin[4], HIGH);
		} 
		return true;
    }
}

void isTheCircleIdSetProperly(String path, String parameter) { 
	DEBUG_PRINTLN("----"); 
	String callPath = path + "test/"; 
	DEBUG_PRINT(callPath);
	DEBUG_PRINT(parameter);
	DEBUG_PRINTLN();
	String response = getDataFromAPI(callPath, parameter); 
	if(response != "fail"){
		int result = response.toInt();
		DEBUG_PRINT("isTheCircleIdSetProperly (1=yes, 0=no): ");
		DEBUG_PRINT(result);
		DEBUG_PRINTLN();
		if (!result) {  
			DEBUG_PRINTLN("The circleId is NOT configured correctly! Please recompile the code with the correct ID");
			
			//isErrorApiNotConfiguredCorrectly = true;
			while (true) { //do nothing forever more
				showErrorLed(ERROR_API_NOT_CONFIGURED_CORRECTLY);
				delay(1);
			}
		}
		DEBUG_PRINTLN("The circleId is configured correctly.");
	}
	else {
		DEBUG_PRINTLN("Something went wrong with the API call. Please restart the microcontroller.");
		int counter = 0;
		while (true) { //do nothing forever more 
			counter++; 
			if(counter <= 5000)
				showErrorLed(ERROR_PLEASE_RESTART_MICROCONTROLLER);
			else if(counter > 5000)
				showConnectionRelatedError(); 
			if (counter >= 10000)
				counter = 0;
			delay(1);
		}
	} 
}

void showErrorLed(int errorNumber) { 
	if (isBlinkLedTime()) {
		toggleErrorLed = !toggleErrorLed;
	}
	for (int i = 0; i < (sizeof(ledPin) / sizeof(int)); i++) {
		if (i <= errorNumber) {
			digitalWrite(ledPin[i], toggleErrorLed);
			continue;
		}
		digitalWrite(ledPin[i], LOW);  
	}
}

boolean isBlinkLedTime() {
	if (millis() - previousMillisBlinkLed >= 1000) {
		previousMillisBlinkLed = millis();
		return true;
	} 
	return false; 
}
