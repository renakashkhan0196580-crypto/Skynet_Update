#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

extern "C" {
  #include "user_interface.h"
}

ESP8266WebServer server(80);
const int ledPin = 2; // বিল্ট-ইন নীল এলইডি

bool isSniffing = false;
bool handshakeFound = false;
unsigned long sniffStartTime = 0;
const unsigned long SNIFF_TIMEOUT = 180000; // ৩ মিনিট

// ১. হ্যান্ডশেক প্যাকেট চেক করার লজিক
void sniffer_callback(uint8_t *buf, uint16_t len) {
  if (isSniffing && !handshakeFound) {
    // EAPOL (Handshake) প্যাকেট শনাক্তকরণ
    if (len > 14 && buf[12] == 0x88 && buf[13] == 0x8e) {
      handshakeFound = true; 
    }
  }
}

// ২. ক্রোম ব্রাউজারে যা দেখবেন (ইন্টারফেস)
void handleRoot() {
  String s = "<html><body style='background:#000;color:#0f0;text-align:center;'>";
  s += "<h1>SkyNet Digital AI</h1>";
  if (!isSniffing) {
    s += "<h3>Ready to Sniff</h3>";
    s += "<a href='/start'><button style='padding:15px;background:#0f0;'>START CAPTURE</button></a>";
  } else {
    s += "<h2>SNIFFING...</h2><p>Wait for LED blink or auto-reset.</p>";
  }
  s += "</body></html>";
  server.send(200, "text/html", s);
}

void handleStart() {
  isSniffing = true;
  handshakeFound = false;
  sniffStartTime = millis();
  
  server.send(200, "text/html", "Attack Started! Reconnect in a few minutes.");
  delay(1000);
  
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(0);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  wifi_promiscuous_enable(1);
  digitalWrite(ledPin, LOW); // LED অন (স্নিফিং সিগন্যাল)
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); 
  WiFi.softAP("SkyNet_Digital_AI", "skynet123");
  
  server.on("/", handleRoot);
  server.on("/start", handleStart);
  server.begin();
}

void loop() {
  if (!isSniffing) {
    server.handleClient();
  } else {
    // ৩. আপনার শর্ত: হ্যান্ডশেক পেলে অথবা ৩ মিনিট পার হলে অটো-রিস্টার্ট
    if (handshakeFound || (millis() - sniffStartTime > SNIFF_TIMEOUT)) {
      isSniffing = false;
      wifi_promiscuous_enable(0);
      digitalWrite(ledPin, HIGH); // LED অফ
      delay(1000);
      ESP.restart(); // অটোমেটিক আগের মোডে ব্যাক আসবে
    }
  }
}
