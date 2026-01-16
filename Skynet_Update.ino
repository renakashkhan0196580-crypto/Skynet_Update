/*
 * Project: SkyNet Digital AI - Handshake Sniffer 
 * Developer: Akash [Hacking Title]
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

extern "C" {
  #include "user_interface.h"
}

ESP8266WebServer server(80);
const int ledPin = 2; 

// কন্ট্রোল ভেরিয়েবল
bool isSniffing = false;
bool handshakeFound = false;
unsigned long sniffStartTime = 0;
const unsigned long SNIFF_TIMEOUT = 180000; // ৩ মিনিট

// ১. এই ফাংশনটি প্যাকেট চেক করবে
void sniffer_callback(uint8_t *buf, uint16_t len) {
  if (isSniffing && !handshakeFound) {
    // হ্যান্ডশেক প্যাকেট (EAPOL) চেনার লজিক
    if (buf[12] == 0x88 && buf[13] == 0x8e) { 
      handshakeFound = true; // হ্যান্ডশেক পাওয়া গেছে!
    }
  }
}

// ২. ওয়েব ইন্টারফেস (Chrome-এ যা দেখবেন)
String getHTML() {
  String res = "<h1>SkyNet Digital AI</h1>";
  if (!isSniffing) {
    res += "<p>Target found. Click to Start.</p>";
    res += "<a href='/start'><button>START CAPTURE</button></a>";
  } else {
    res += "<h2>SNIFFING...</h2><p>Wait for LED to blink or timeout.</p>";
  }
  return res;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // LED বন্ধ
  
  WiFi.softAP("SkyNet_Digital_AI", "skynet123");
  server.on("/", []() { server.send(200, "text/html", getHTML()); });
  
  server.on("/start", []() {
    isSniffing = true;
    sniffStartTime = millis();
    server.send(200, "text/html", "Attack Started! Board is now sniffing...");
    
    // স্নিফিং মোড সেটআপ
    wifi_set_opmode(STATION_MODE);
    wifi_promiscuous_enable(0);
    wifi_set_promiscuous_rx_cb(sniffer_callback);
    wifi_promiscuous_enable(1);
  });

  server.begin();
}

void loop() {
  if (!isSniffing) {
    server.handleClient();
  } else {
    // ৩. চেকিং কন্ডিশন (আপনার মূল দাবি)
    if (handshakeFound || (millis() - sniffStartTime > SNIFF_TIMEOUT)) {
      isSniffing = false;
      digitalWrite(ledPin, LOW); // সাফল্য বোঝাতে LED জ্বলবে
      delay(2000);
      ESP.restart(); // অটোমেটিক আগের মোডে ব্যাক আসবে
    }
  }
}
