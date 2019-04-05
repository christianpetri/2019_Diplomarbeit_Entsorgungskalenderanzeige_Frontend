# 2019 Diplomarbeit Entsorgungskalenderanzeige Frontend Hardware

## Installation
1.  [Ardunio IDE](https://www.arduino.cc/en/main/software) mit [ESP8266](https://github.com/esp8266/Arduino)  
2.  Optional: [Visual Studio](https://visualstudio.microsoft.com/de/downloads/) mit [Arduino IDE for Visual Studio](https://marketplace.visualstudio.com/items?itemName=VisualMicro.ArduinoIDEforVisualStudio)

## Hardware
[Wemos D1](https://wiki.wemos.cc/products:d1:d1_mini)  

## Einsatz der Hardware

5 Leds an den gewünschten Pin anschliessen (im Code definiert), kompellieren und auf den Wemos D1 mini hochladen.

## Benutzerhandbuch

Die 5 Leds repräsentieren die 5 Entsorgungsgüter:

Das erste Led steht für Grüngut  
Das zweite Led steht für Karton  
Das dritte Led steht für allgemeiner Abfall (Kehricht) und Sperrgut  
Das vierte Led steht für Metall  
Das fünfte Led steht für Papier  

Wenn das Led dauerhaft leuchtet, darf das entsprechende Entsorgungsgut an die Strasse gestellt werden.

### Mögliche Fehlermeldungen

Wenn ein oder mehre Leds blinken, liegt eine Störung vor:

Wenn ein Led blinkt = Keine Verbindung zum Wifi --> Netwerkname und Passwort korrekt?  Wlan ausgeschaltet?  
Wenn zwei Leds blinken = Keine Verbindung zum Internet --> Hat das Wifi Zugang zum Internet?  
Wenn drei Leds blinken = Keine Verbingung zur API -->
Wenn vier Leds blinken = API nicht richtig konfiguriert --> Ist der Kreis eingetragen? Ist die richtige Kreisnummer eingetragen?  
Wenn fünf Leds blinken = Bitte richten sie die Anzeige ein --> Wlan-Name, Wlan-Passwort und den richtigen Kreis eintragen.  
