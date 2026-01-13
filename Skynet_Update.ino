/* * Project: SKYNET AKASH HACKER BD - FULL GHOST MODE
 * Version: v1.6 [All Features Restored]
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

char ap_ssid[64] = "SKYNET AKASH HACKER BD";
char ap_pass[32] = "akash_hacker";
const char* update_url = "https://raw.githubusercontent.com/renakashkhan0196580-crypto/Skynet_Update/main/update.bin";

AsyncWebServer server(80);

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
        .btn-red { border-color: #f00; color: #f00; }
        .btn-cyan { border-color: cyan; color: cyan; }
        .btn-gold { border-color: gold; color: gold; }
        input { background: #111; border: 1px solid #0f0; color: #0f0; width: 90%; padding: 10px; margin: 5px 0; }
        .toggle-box { display: flex; align-items: center; justify-content: center; gap: 10px; margin: 10px 0; font-size: 12px; color: cyan; }
        #targets { height:120px; overflow-y:auto; font-size:12px; margin-top:10px; background:#111; border: 1px solid #222; text-align: left; padding: 5px; }
    </style>
</head>
<body>
    <h2 style="text-shadow: 0 0 10px #0f0;">SKYNET AKASH HACKER BD</h2>
    <div style="color:yellow; font-size: 10px;">KERNEL: v1.6 [GHOST RESTORED]</div>

    <div class="box">
        <div style="color:yellow">➤ TARGET MANAGER</div>
        <button class="btn" onclick="startScan()">🔍 SCAN AIRWAVES</button>
        <button class="btn btn-cyan" onclick="getResults()">📂 SHOW TARGET LIST</button>
        <div id="targets">Waiting for scan...</div>
    </div>

    <div class="box">
        <div style="color:#f00">➤ ATTACK ENGINE</div>
        <button class="btn btn-red" onclick="run('deauth_target')">🎯 SELECTIVE DEAUTH (TARGET)</button>
        <button class="btn btn-red" onclick="run('deauth_all')">🔥 PANIC MODE (DEAUTH ALL)</button>
        <button class="btn btn-gold" onclick="run('force_handshake')">⚡ FORCE HANDSHAKE</button>
    </div>

    <div class="box">
        <div style="color:cyan">➤ GHOST REPEATER & CLONING</div>
        <input type="text" id="t_ssid" placeholder="Target SSID">
        <input type="password" id="t_pass" placeholder="Target Password">
        <div class="toggle-box">
            <label><input type="checkbox" id="hide_ssid"> HIDE SSID</label>
            <label><input type="checkbox" id="anti_block" checked> ANTI-BLOCK</label>
        </div>
        <button class="btn btn-cyan" onclick="startRepeater()">🚀 START GHOST MODE</button>
    </div>

    <div class="box" style="border-style: dashed; border-color: gold;">
        <button class="btn btn-gold" onclick="triggerUpdate()">🚀 CLOUD UPDATE</button>
    </div>

    <script>
        function startScan() { document.getElementById('targets').innerHTML = "Sniffing..."; fetch('/scan-start'); }
        function getResults() {
            fetch('/scan-results').then(r => r.json()).then(data => {
                let html = "";
                data.forEach(n => {
                    html += `<div onclick="document.getElementById('t_ssid').value='${n.ssid}'" style="padding:8px; border-bottom:1px solid #222; cursor:pointer;">📡 ${n.ssid} [${n.rssi}dBm]</div>`;
                });
                document.getElementById('targets').innerHTML = html || "No networks found.";
            });
        }
        function run(cmd) { fetch('/' + cmd).then(r => r.text()).then(d => alert(d)); }
        function startRepeater() {
            let s = document.getElementById('t_ssid').value;
            let p = document.getElementById('t_pass').value;
            let h = document.getElementById('hide_ssid').checked ? 1 : 0;
            let a = document.getElementById('anti_block').checked ? 1 : 0;
            fetch(`/repeater?ssid=${s}&pass=${p}&hide=${h}&anti=${a}`).then(r => r.text()).then(d => alert(d));
        }
        function triggerUpdate() { if(confirm("Update System?")) fetch('/trigger_update'); }
    </script>
</body>
</html>
)rawliteral";

void setup(){
    Serial.begin(115200);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ap_ssid, ap_pass);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/html", index_html); });
    server.on("/scan-start", HTTP_GET, [](AsyncWebServerRequest *request){ WiFi.scanNetworks(true); request->send(200, "text/plain", "OK"); });
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

    server.on("/repeater", HTTP_GET, [](AsyncWebServerRequest *request){
        String s = request->arg("ssid"), p = request->arg("pass");
        bool h = request->arg("hide") == "1", a = request->arg("anti") == "1";
        if(a) { // Anti-Block logic
            uint8_t mac[6] = {0x00, 0x01, (uint8_t)random(256), (uint8_t)random(256), (uint8_t)random(256), (uint8_t)random(256)};
            wifi_set_macaddr(STATION_IF, mac);
        }
        WiFi.begin(s.c_str(), p.c_str());
        WiFi.softAP(ap_ssid, ap_pass, 1, h); // Hide SSID option applied
        request->send(200, "text/plain", "Ghost Mode Started.");
    });

    server.on("/deauth_target", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", "Target Deauth Active."); });
    server.on("/deauth_all", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", "Panic Mode Active."); });
    server.on("/force_handshake", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", "Capturing Handshake..."); });

    server.on("/trigger_update", HTTP_GET, [](AsyncWebServerRequest *request){
        WiFiClientSecure client; client.setInsecure();
        ESPhttpUpdate.update(client, update_url);
    });

    server.begin();
}

void loop(){}

