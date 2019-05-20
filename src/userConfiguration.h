/**
EXAMPLE CONFIG
Make sure that the "" are set correctly.  

#define WIFI_NAME "MyAwsomeWifi" 
#define WIFI_PASSWORD "ThereIsNoSpoon" 
#define CIRCLE_ID 3

#define HOST "www.entsorgungskalenderanzeige.christianpetri.ch"
// Use web browser to view and copy
// SHA1 fingerprint of the certificate
#define FINGERPRINT "9620a9ed9756297500f628c9ce9a1e093e3658ae";
*/

#ifndef _USERCONFIGURATION_H_
#define _USERCONFIGURATION_H_

//Please enter your wifi name and password
#define WIFI_NAME "CP"
#define WIFI_PASSWORD "ChristianPetri1988"

//Please enter your circle-ID
#define CIRCLE_ID 5

//Please enter your host
#define HOST "www.entsorgungskalenderanzeige.christianpetri.ch"
// Use web browser to view and copy
// SHA1 fingerprint of the certificate
#define FINGERPRINT "9620a9ed9756297500f628c9ce9a1e093e3658ae" //certificate --> until Sunday, 21. July 2019

//Please specify the pins to which the LEDs are connected
#define LED_GREEN_WASTE_PIN D8
#define LED_CARDBORD_WASTE_PIN D7
#define LED_GENERAL_WASTE_AND_BULKY_GOODS_PIN D6
#define LED_METAL_WASTE_PIN D5
#define LED_PAPER_WASTE_PIN D0
#define LED_POWER_PIN LED_BUILTIN

//Enable DEBUG and/or DEMO mode
#define DEBUG //comment out to disable Serial prints
//#define DEMO //comment out to disable DEMO mode

#endif // _USERCONFIGURATION_H_