/* * Project: SKYNET AKASH HACKER BD
 * Version: v1.2 [Stable - All Features Combined]
 * Developer: Akash [Hacking Title]
 * Device: ESP8266 (NodeMCU/Wemos D1 Mini)
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

// আপনার GitHub 'Raw' লিঙ্ক
const char* update_url = "https://raw.githubusercontent.com/renakashkhan0196580-crypto/Skynet_Update/main/update.bin";

AsyncWebServer server(80);

// --- Graphical Interface (HTML/CSS/JS) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>SKYNET AKASH HACKER BD</title>
    <style>
        body { background: #000; color: #0f0; font-family: 'Courier New', monospace; text-align: center; margin: 0; padding: 10px; }
        .box { border: 1px solid #0f0; margin: 15px auto; padding: 15px; background: #050505; border-radius: 8px; max-width: 500px; }
        .btn { width: 100%; padding: 12px; margin: 5px 0; background: #111; border: 1px solid #0f0; color: #0f0; font-weight: bold; cursor: pointer; text-transform: uppercase; }
        .btn-red { border-color: #f00; color: #f00; }
        .btn-cyan { border-color: cyan; color: cyan; }
        .btn-gold { border-color: #ffd700; color: #ffd700; box-shadow: 0 0 10px #ffd700; }
        input { background: #111; border: 1px solid #0f0; color: #0f0; width: 90%; padding: 10px; margin-bottom: 5px; }
        .toggle-box { display: flex; align-items: center; gap: 10px; margin: 10px 0; color: #0f0; font-size: 12px; }
        #update_overlay { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.95); z-index: 1000; }
        .loader { border: 4px solid #111; border-top: 4px solid #ffd700; border-radius: 50%; width: 50px; height: 50px; animation: spin 1s linear infinite; margin: 45% auto 20px; }
        @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
    </style>
</head>
<body>
    <div id="update_overlay">
        <div class="loader"></div>
        <div style="color:#ffd700;">FETCHING UPDATES FROM SKYNET SERVERS...<br>REBOOTING AUTOMATICALLY</div>
    </div>

    <h2 style="text-shadow: 0 0 10px #0f0;">SKYNET AKASH HACKER BD</h2>
    <small style="color:yellow;">KERNEL: v1.2 [GHOST EDITION]</small>

    <div class="box">
        <div style="color:yellow">▶ TARGET MANAGER</div>
        <button class="btn" onclick="scan()">🔍 SCAN AIRWAVES</button>
        <div id="targets" style="height:80px; overflow-y:auto; font-size:12px; margin-top:10px; background:#111;"></div>
    </div>

    <div class="box">
        <div style="color:#f00">▶ ATTACK ENGINE</div>
        <button class="btn" style="color:gold; border-color:gold;" onclick="run('force_handshake')">⚡ FORCE CAPTURE HANDSHAKE</button>
        <button class="btn btn-red" onclick="run('deauth_target')">🎯 SELECTIVE DEAUTH (TARGET)</button>
        <button class="btn btn-red" onclick="run('deauth_all')">🔥 PANIC MODE (DEAUTH ALL)</button>
    </div>

    <div class="box">
        <div style="color:cyan">▶ GHOST REPEATER & CLONING</div>
        <input type="text" id="t_ssid" placeholder="Target SSID">
        <input type="password" id="t_pass" placeholder="Target Password">
        <input type="text" id="cloned_name" placeholder="Clone Name (e.g. iPhone 15)">
        <div class="toggle-box">
            <input type="checkbox" id="hide_ssid"> Hide My SSID
            <input type="checkbox" id="anti_block" checked> Anti-Block Mode
        </div>
        <button class="btn btn-cyan" onclick="startRepeater()">🚀 START GHOST REPEATER</button>
    </div>

    <div class="box">
        <div style="color:yellow">▶ FILE MANAGER (CAPTURERS)</div>
        <div id="file_list" style="font-size:12px; margin-bottom:10px;"></div>
        <button class="btn" onclick="loadFiles()">📂 REFRESH STORAGE</button>
    </div>

    <div class="box" style="border-style: dashed; border-color: gold;">
        <button class="btn btn-gold" onclick="triggerUpdate()">🚀 CHECK & INSTALL UPDATE</button>
    </div>

    <script>
        function scan() {
            document.getElementById('targets').innerHTML = "Sniffing...";
            fetch('/scan').then(r => r.json()).then(data => {
                let html = "";
                data.forEach(n => {
                    html += `<div onclick="document.getElementById('t_ssid').value='${n.ssid}'" style="padding:5px; border-bottom:1px solid #222; cursor:pointer;">${n.ssid} [${n.rssi}dBm]</div>`;
                });
                document.getElementById('targets').innerHTML = html;
            });
        }
        function startRepeater() {
            let s = document.getElementById('t_ssid').value;
            let p = document.getElementById('t_pass').value;
            let n = document.getElementById('cloned_name').value;
            let h = document.getElementById('hide_ssid').checked ? 1 : 0;
            let a = document.getElementById('anti_block').checked ? 1 : 0;
            fetch(`/repeater?ssid=${s}&pass=${p}&name=${n}&hide=${h}&anti=${a}`).then(r => r.text()).then(d => alert(d));
        }
        function loadFiles() {
            fetch('/list_files').then(r => r.json()).then(files => {
                let html = "";
                files.forEach(f => { html += `<div>${f.name} <a href="/download?file=${f.name}" style="color:cyan;">[GET]</a></div>`; });
                document.getElementById('file_list').innerHTML = html || "Empty.";
            });
        }
        function triggerUpdate() {
            if(confirm("বোর্ডটি কি আপনার GitHub থেকে আপডেট করতে চান?")) {
                document.getElementById('update_overlay').style.display = 'block';
                fetch('/trigger_update');
            }
        }
        function run(cmd) { fetch('/' + cmd).then(r => r.text()).then(d => alert(d)); }
    </script>
</body>
</html>
)rawliteral";

void setup(){
    Serial.begin(115200);
    SPIFFS.begin();
    WiFi.softAP(ap_ssid, ap_pass);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
        int n = WiFi.scanNetworks();
        String json = "[";
        for (int i = 0; i < n; ++i) {
            json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
            if (i < n - 1) json += ",";
        }
        json += "]";
        request->send(200, "application/json", json);
    });

    server.on("/repeater", HTTP_GET, [](AsyncWebServerRequest *request){
        String s = request->arg("ssid"), p = request->arg("pass"), n = request->arg("name");
        bool h = request->arg("hide") == "1", a = request->arg("anti") == "1";
        if(a) {
            uint8_t mac[6] = {0x00, 0xBD, (uint8_t)random(255), (uint8_t)random(255), (uint8_t)random(255), (uint8_t)random(255)};
            wifi_set_macaddr(STATION_IF, mac); 
        }
        WiFi.hostname(n);
        WiFi.begin(s.c_str(), p.c_str());
        WiFi.softAP(ap_ssid, ap_pass, 1, h);
        request->send(200, "text/plain", "Ghost Mode Active.");
    });

    server.on("/list_files", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "[";
        Dir dir = SPIFFS.openDir("/");
        while (dir.next()) { json += "{\"name\":\"" + dir.fileName() + "\"},"; }
        if (json.endsWith(",")) json.remove(json.length()-1);
        json += "]";
        request->send(200, "application/json", json);
    });

    server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, request->arg("file"), "application/octet-stream");
    });

    server.on("/trigger_update", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Update Started.");
        WiFiClientSecure client;
        client.setInsecure();
        ESPhttpUpdate.rebootOnUpdate(true);
        ESPhttpUpdate.update(client, update_url);
    });

    server.begin();
}

void loop(){}

