/* * Project: SKYNET AKASH HACKER BD - GHOST EDITION
 * Version: v1.5 [Stable Fix - No Data Loss]
 * Developer: Akash [Hacking Title]
 */

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266httpUpdate.h>
#include <FS.h>

extern "C" {
  #include "user_interface.h"
}

// --- Configuration ---
char ap_ssid[64] = "SKYNET AKASH HACKER BD";
char ap_pass[32] = "akash_hacker";
const char* update_url = "https://raw.githubusercontent.com/renakashkhan0196580-crypto/Skynet_Update/main/update.bin";

AsyncWebServer server(80);

// --- Graphical Interface (Sleek Hacker UI) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>SKYNET AKASH HACKER BD</title>
    <style>
        body { background: #000; color: #0f0; font-family: 'Courier New', monospace; text-align: center; margin: 0; padding: 10px; }
        .box { border: 1px solid #0f0; margin: 15px auto; padding: 15px; background: #050505; border-radius: 8px; max-width: 500px; box-shadow: 0 0 5px #0f0; }
        .btn { width: 100%; padding: 12px; margin: 5px 0; background: #111; border: 1px solid #0f0; color: #0f0; font-weight: bold; cursor: pointer; text-transform: uppercase; }
        .btn:active { background: #0f0; color: #000; }
        .btn-cyan { border-color: cyan; color: cyan; }
        .btn-gold { border-color: gold; color: gold; }
        #targets { height:150px; overflow-y:auto; font-size:12px; margin-top:10px; background:#111; border: 1px solid #222; text-align: left; padding: 5px; }
        input { background: #111; border: 1px solid #0f0; color: #0f0; width: 90%; padding: 10px; margin: 5px 0; }
    </style>
</head>
<body>
    <h2 style="text-shadow: 0 0 10px #0f0;">SKYNET AKASH HACKER BD</h2>
    <div style="color:yellow; font-size: 10px; margin-bottom: 20px;">SYSTEM STATUS: ONLINE | KERNEL: v1.5</div>

    <div class="box">
        <div style="color:yellow">➤ TARGET MANAGER</div>
        <button class="btn" onclick="startScan()">🔍 START SNIFFING</button>
        <button class="btn btn-cyan" onclick="getResults()">📂 VIEW DETECTED NETWORKS</button>
        <div id="targets">System Idle...</div>
    </div>

    <div class="box">
        <div style="color:#f00">➤ ATTACK ENGINE</div>
        <button class="btn" style="color:gold; border-color:gold;" onclick="run('force_handshake')">⚡ CAPTURE HANDSHAKE</button>
        <button class="btn" style="color:#f00; border-color:#f00;" onclick="run('deauth_all')">🔥 PANIC MODE (DEAUTH ALL)</button>
    </div>

    <div class="box">
        <div style="color:cyan">➤ GHOST REPEATER</div>
        <input type="text" id="t_ssid" placeholder="Target SSID">
        <input type="password" id="t_pass" placeholder="Password">
        <button class="btn btn-cyan" onclick="alert('Ghosting Started...')">🚀 ACTIVATE CLONE</button>
    </div>

    <div class="box" style="border-style: dashed; border-color: gold;">
        <button class="btn btn-gold" onclick="triggerUpdate()">🚀 CLOUD UPDATE</button>
    </div>

    <script>
        function startScan() {
            document.getElementById('targets').innerHTML = "Scanning Airwaves... (Disconnect possible)";
            fetch('/scan-start').then(r => r.text());
        }
        function getResults() {
            fetch('/scan-results').then(r => r.json()).then(data => {
                let html = "";
                if(data.length == 0) html = "<span style='color:red;'>No Data. Scan first & wait 10s.</span>";
                data.forEach(n => {
                    html += `<div onclick="document.getElementById('t_ssid').value='${n.ssid}'" style="padding:8px; border-bottom:1px solid #222; cursor:pointer;">📡 ${n.ssid} <span style="color:gray;">[${n.rssi}dBm]</span></div>`;
                });
                document.getElementById('targets').innerHTML = html;
            }).catch(e => alert("Reconnecting... Please wait 5s and try again."));
        }
        function run(cmd) { fetch('/' + cmd).then(r => r.text()).then(d => alert(d)); }
        function triggerUpdate() {
            if(confirm("Download update from GitHub?")) {
                fetch('/trigger_update');
                alert("Updating... Board will reboot.");
            }
        }
    </script>
</body>
</html>
)rawliteral";

void setup(){
    Serial.begin(115200);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ap_ssid, ap_pass);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    server.on("/scan-start", HTTP_GET, [](AsyncWebServerRequest *request){
        WiFi.scanNetworks(true); 
        request->send(200, "text/plain", "Scanning...");
    });

    server.on("/scan-results", HTTP_GET, [](AsyncWebServerRequest *request){
        int n = WiFi.scanComplete();
        String json = "[";
        if (n >= 0) {
            for (int i = 0; i < n; ++i) {
                json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
                if (i < n - 1) json += ",";
            }
        }
        json += "]";
        request->send(200, "application/json", json);
    });

    server.on("/trigger_update", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Updating...");
        WiFiClientSecure client;
        client.setInsecure();
        ESPhttpUpdate.rebootOnUpdate(true);
        ESPhttpUpdate.update(client, update_url);
    });

    server.begin();
}

void loop(){}
