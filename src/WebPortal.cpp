#include "WebPortal.h"
#include <WiFi.h>
#include <ArduinoJson.h>

WebPortal::WebPortal(AppSettings& set, StorageManager& stor) 
    : server(80), settings(set), storage(stor) {}

void WebPortal::begin() {
    server.on("/", HTTP_GET, std::bind(&WebPortal::handleRoot, this));
    server.on("/save-settings", HTTP_POST, std::bind(&WebPortal::handleSaveSettings, this));
    server.on("/add-wifi", HTTP_POST, std::bind(&WebPortal::handleAddWifi, this));
    server.on("/del-wifi", HTTP_POST, std::bind(&WebPortal::handleDeleteWifi, this));
    server.on("/scan-wifi", HTTP_GET, std::bind(&WebPortal::handleScanWifi, this));
    server.on("/restart", HTTP_POST, std::bind(&WebPortal::handleRestart, this));

    server.begin();
    Serial.println("[WebPortal] Servidor Web ativo na porta 80");
}

void WebPortal::handleClient() {
    server.handleClient();
}

void WebPortal::handleRoot() {
    server.send(200, "text/html", generateHTML());
}

void WebPortal::handleSaveSettings() {
    if (server.hasArg("city")) settings.cityName = server.arg("city");
    if (server.hasArg("lat")) settings.latitude = server.arg("lat").toFloat();
    if (server.hasArg("lon")) settings.longitude = server.arg("lon").toFloat();
    if (server.hasArg("brightness")) settings.brightness = server.arg("brightness").toInt();
    if (server.hasArg("eco")) settings.ecoMode = (server.arg("eco") == "1");
    if (server.hasArg("eco_start")) settings.ecoStartHour = server.arg("eco_start").toInt();
    if (server.hasArg("eco_end")) settings.ecoEndHour = server.arg("eco_end").toInt();
    if (server.hasArg("eco_br")) settings.ecoBrightness = server.arg("eco_br").toInt();
    if (server.hasArg("rgb")) settings.rgbLedEnabled = (server.arg("rgb") == "1");

    storage.saveSettings(settings);
    server.sendHeader("Location", "/");
    server.send(303);
}

void WebPortal::handleAddWifi() {
    if (server.hasArg("ssid") && server.arg("ssid").length() > 0) {
        String ssid = server.arg("ssid");
        String pass = server.hasArg("pass") ? server.arg("pass") : "";
        storage.addWifiNetwork(settings, ssid, pass);
    }
    server.sendHeader("Location", "/");
    server.send(303);
}

void WebPortal::handleDeleteWifi() {
    if (server.hasArg("index")) {
        int idx = server.arg("index").toInt();
        storage.removeWifiNetwork(settings, idx);
    }
    server.sendHeader("Location", "/");
    server.send(303);
}

void WebPortal::handleScanWifi() {
    int n = WiFi.scanNetworks();
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    for (int i = 0; i < n; ++i) {
        JsonObject obj = array.add<JsonObject>();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);
        obj["secure"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void WebPortal::handleRestart() {
    server.send(200, "text/html", "<p>Reiniciando o Atmos BR... aguarde 10 segundos.</p><script>setTimeout(()=>window.location='/', 10000);</script>");
    delay(1000);
    ESP.restart();
}

String WebPortal::generateHTML() {
    String html = "<!DOCTYPE html><html lang='pt-BR'><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1.0'>";
    html += "<title>Atmos BR — Configuracao</title><style>";
    html += ":root{--bg:#0A0B10;--panel:#161926;--border:#293047;--text:#F0F4FC;--muted:#9AA3B8;--accent:#00E5FF;--green:#00E676;--orange:#FF7000;}";
    html += "*{box-sizing:border-box}body{margin:0;font-family:-apple-system,BlinkMacSystemFont,Segoe UI,Roboto,sans-serif;background:var(--bg);color:var(--text);padding:20px 14px;}";
    html += "main{max-width:700px;margin:auto;}";
    html += "h1{font-size:24px;color:var(--text);margin:0 0 4px;font-weight:700;}";
    html += ".badge{display:inline-block;padding:3px 8px;border-radius:12px;background:rgba(0,229,255,0.15);color:var(--accent);font-size:12px;margin-bottom:20px;}";
    html += ".card{background:var(--panel);border:1px solid var(--border);border-radius:14px;padding:18px;margin-bottom:18px;}";
    html += "h2{font-size:16px;color:var(--accent);margin:0 0 14px;text-transform:uppercase;letter-spacing:0.05em;}";
    html += "label{display:block;font-size:13px;color:var(--muted);margin:10px 0 4px;}";
    html += "input,select{width:100%;padding:10px 12px;background:#0E101A;border:1px solid var(--border);border-radius:8px;color:#fff;font-size:14px;outline:none;}";
    html += "input:focus{border-color:var(--accent);}";
    html += ".row{display:grid;grid-template-columns:1fr 1fr;gap:12px;}";
    html += "button{background:var(--accent);color:#000;border:none;border-radius:8px;padding:12px 18px;font-weight:600;font-size:14px;cursor:pointer;width:100%;margin-top:14px;}";
    html += "button.sec{background:transparent;border:1px solid var(--border);color:var(--text);margin-top:0;}";
    html += "button.del{background:rgba(255,45,85,0.2);border:1px solid #FF2D55;color:#FF2D55;width:auto;padding:6px 12px;margin:0;}";
    html += ".wifi-item{display:flex;align-items:center;justify-content:space-between;padding:10px;background:#0E101A;border-radius:8px;margin-bottom:8px;border:1px solid var(--border);}";
    html += "</style></head><body><main>";

    html += "<h1>🌤️ Atmos BR</h1>";
    html += "<div class='badge'>Estacao Meteorologica CYD • Open-Source</div>";

    // 0. PREVIEW AO VIVO DA TELA
    html += "<div class='card'><h2>📱 Preview da Tela do CYD</h2>";
    html += "<div style='display:flex;justify-content:center;'>";
    html += "<div style='width:240px;height:320px;background:#0B0D15;border:2px solid #293047;border-radius:12px;padding:8px;box-shadow:0 10px 25px rgba(0,0,0,0.5);font-family:sans-serif;'>";
    html += "<div style='display:flex;justify-content:space-between;font-size:12px;margin-bottom:6px;'><span>" + settings.cityName + "</span><span style='color:#00E5FF;'>12:00</span></div>";
    html += "<div style='background:#171B2B;border:1px solid #293047;border-radius:8px;padding:8px;margin-bottom:6px;'>";
    html += "<div style='font-size:28px;font-weight:bold;color:#fff;'>22°</div><div style='font-size:11px;color:#9AA3B8;'>Min 16° Max 26°</div><div style='font-size:12px;color:#00E5FF;'>Ceu Limpo</div></div>";
    html += "<div style='display:grid;grid-template-columns:1fr 1fr;gap:4px;margin-bottom:6px;'>";
    html += "<div style='background:#171B2B;border:1px solid #293047;border-radius:6px;padding:6px;font-size:10px;'><span style='color:#9AA3B8;'>VENTO</span><br><strong style='font-size:12px;color:#fff;'>12 km/h</strong></div>";
    html += "<div style='background:#171B2B;border:1px solid #293047;border-radius:6px;padding:6px;font-size:10px;'><span style='color:#9AA3B8;'>UMIDADE</span><br><strong style='font-size:12px;color:#fff;'>75%</strong></div>";
    html += "<div style='background:#171B2B;border:1px solid #293047;border-radius:6px;padding:6px;font-size:10px;'><span style='color:#9AA3B8;'>PRESSAO</span><br><strong style='font-size:12px;color:#fff;'>1018 hPa</strong></div>";
    html += "<div style='background:#171B2B;border:1px solid #293047;border-radius:6px;padding:6px;font-size:10px;'><span style='color:#9AA3B8;'>AR</span><br><strong style='font-size:12px;color:#00E676;'>Excelente</strong></div>";
    html += "</div>";
    html += "<div style='background:#171B2B;border:1px solid #293047;border-radius:6px;padding:6px;font-size:9px;'><span style='color:#9AA3B8;'>DICA</span><br>Condicoes estaveis e agradaveis.</div>";
    html += "<div style='text-align:center;font-size:9px;color:#9AA3B8;margin-top:6px;'>&lt; Toque para navegar &gt;</div>";
    html += "</div></div></div>";


    // 1. CIDADE & LOCALIZAÇÃO
    html += "<form method='POST' action='/save-settings'>";
    html += "<div class='card'><h2>📍 Localizacao da Previsao</h2>";
    html += "<label>Buscar Cidade (Open-Meteo):</label>";
    html += "<div style='display:flex;gap:8px;'><input id='citySearch' placeholder='Ex: Curitiba, Sao Paulo, Rio...' onkeydown='if(event.key===\"Enter\"){event.preventDefault();searchCity();}'><button type='button' class='sec' style='width:auto' onclick='searchCity()'>Buscar</button></div>";
    html += "<div id='searchResults' style='margin-top:8px;'></div>";
    html += "<label>Nome da Cidade exibido no Display:</label><input name='city' id='locCity' value='" + settings.cityName + "'>";
    html += "<div class='row'>";
    html += "<div><label>Latitude:</label><input name='lat' id='locLat' value='" + String(settings.latitude, 4) + "'></div>";
    html += "<div><label>Longitude:</label><input name='lon' id='locLon' value='" + String(settings.longitude, 4) + "'></div>";
    html += "</div></div>";

    // 2. DISPLAY & MODO NOTURNO
    html += "<div class='card'><h2>💡 Display & Modo Noturno</h2>";
    html += "<label>Brilho Diurno (20 a 255):</label><input type='range' min='20' max='255' name='brightness' value='" + String(settings.brightness) + "'>";
    html += "<div class='row' style='margin-top:10px;'>";
    html += "<div><label>Modo Noturno (Eco):</label><select name='eco'><option value='1'" + String(settings.ecoMode ? " selected" : "") + ">Ativado</option><option value='0'" + String(!settings.ecoMode ? " selected" : "") + ">Desativado</option></select></div>";
    html += "<div><label>LED RGB Mood:</label><select name='rgb'><option value='1'" + String(settings.rgbLedEnabled ? " selected" : "") + ">Ligado</option><option value='0'" + String(!settings.rgbLedEnabled ? " selected" : "") + ">Desligado</option></select></div>";
    html += "</div>";
    html += "<div class='row'>";
    html += "<div><label>Inicio Eco (Hora):</label><input type='number' min='0' max='23' name='eco_start' value='" + String(settings.ecoStartHour) + "'></div>";
    html += "<div><label>Fim Eco (Hora):</label><input type='number' min='0' max='23' name='eco_end' value='" + String(settings.ecoEndHour) + "'></div>";
    html += "</div>";
    html += "<label>Brilho Noturno Reduzido (20 a 100):</label><input type='range' min='20' max='100' name='eco_br' value='" + String(settings.ecoBrightness) + "'>";
    html += "<button type='submit'>💾 Salvar Configuracoes</button></div></form>";

    // 3. GERENCIADOR DE REDES WI-FI MEMORIZADAS (MULTI-WIFI)
    html += "<div class='card'><h2>📶 Redes Wi-Fi Memorizadas (" + String(settings.savedNetworks.size()) + ")</h2>";
    html += "<p style='font-size:13px;color:var(--muted);margin:0 0 12px;'>O ESP32 conectara automaticamente a qualquer uma destas redes salvas:</p>";

    if (settings.savedNetworks.empty()) {
        html += "<p style='color:var(--muted);font-size:13px;'>Nenhuma rede memorizada.</p>";
    } else {
        for (size_t i = 0; i < settings.savedNetworks.size(); i++) {
            html += "<div class='wifi-item'><div><strong>" + settings.savedNetworks[i].ssid + "</strong></div>";
            html += "<form method='POST' action='/del-wifi' style='margin:0;'><input type='hidden' name='index' value='" + String(i) + "'><button class='del' type='submit'>Excluir</button></form></div>";
        }
    }

    html += "<h3 style='font-size:14px;color:var(--text);margin:18px 0 8px;'>Adicionar Nova Rede Wi-Fi</h3>";
    html += "<form method='POST' action='/add-wifi'>";
    html += "<div style='display:flex;gap:8px;'><input name='ssid' id='wifiSsid' placeholder='Nome da Rede (SSID)' required><button type='button' class='sec' style='width:auto' onclick='scanWifi()'>Escanear</button></div>";
    html += "<div id='wifiList' style='margin-top:6px;'></div>";
    html += "<input name='pass' type='password' placeholder='Senha do Wi-Fi' style='margin-top:8px;'>";
    html += "<button type='submit'>➕ Memorizar Rede Wi-Fi</button></form></div>";

    // 4. STATUS DO DISPOSITIVO
    html += "<div class='card'><h2>⚙️ Status do Sistema</h2>";
    html += "<p style='font-size:13px;color:var(--muted);margin:4px 0;'>IP Local: <strong style='color:#fff'>" + WiFi.localIP().toString() + "</strong></p>";
    html += "<p style='font-size:13px;color:var(--muted);margin:4px 0;'>Sinal Wi-Fi: <strong style='color:#fff'>" + String(WiFi.RSSI()) + " dBm</strong></p>";
    html += "<p style='font-size:13px;color:var(--muted);margin:4px 0;'>Memoria Livre: <strong style='color:#fff'>" + String(ESP.getFreeHeap() / 1024) + " KB</strong></p>";
    html += "<form method='POST' action='/restart' onsubmit='return confirm(\"Deseja reiniciar o ESP32?\")'><button class='sec' style='margin-top:10px;' type='submit'>🔄 Reiniciar ESP32</button></form></div>";

    // JAVASCRIPT DINÂMICO
    html += "<script>";
    html += "async function searchCity(){";
    html += "  let q=document.getElementById('citySearch').value.trim();";
    html += "  let resDiv=document.getElementById('searchResults');";
    html += "  if(!q){return;}resDiv.innerHTML='<span style=\"color:var(--muted);font-size:12px;\">Buscando...</span>';";
    html += "  try{";
    html += "    let res=await fetch('https://geocoding-api.open-meteo.com/v1/search?name='+encodeURIComponent(q)+'&count=5&language=pt&format=json');";
    html += "    let data=await res.json();";
    html += "    if(!data.results||!data.results.length){resDiv.innerHTML='<span style=\"color:#FF2D55;font-size:12px;\">Nenhuma cidade encontrada.</span>';return;}";
    html += "    let h='<select onchange=\"selectCity(this)\" style=\"margin-top:4px;\"><option value=\"\">Selecione o resultado:</option>';";
    html += "    data.results.forEach(c=>{";
    html += "      let label=[c.name,c.admin1,c.country].filter(Boolean).join(' - ');";
    html += "      h+='<option value=\"'+c.name+'\" data-lat=\"'+c.latitude+'\" data-lon=\"'+c.longitude+'\">'+label+'</option>';";
    html += "    });";
    html += "    h+='</select>';resDiv.innerHTML=h;";
    html += "  }catch(e){resDiv.innerHTML='<span style=\"color:#FF2D55;font-size:12px;\">Erro ao buscar cidade.</span>';}";
    html += "}";
    html += "function selectCity(sel){";
    html += "  let opt=sel.options[sel.selectedIndex];";
    html += "  if(!opt.value)return;";
    html += "  document.getElementById('locCity').value=opt.value;";
    html += "  document.getElementById('locLat').value=opt.dataset.lat;";
    html += "  document.getElementById('locLon').value=opt.dataset.lon;";
    html += "}";
    html += "async function scanWifi(){";
    html += "  let list=document.getElementById('wifiList');";
    html += "  list.innerHTML='<span style=\"color:var(--muted);font-size:12px;\">Escaneando redes 2.4GHz proximas...</span>';";
    html += "  try{";
    html += "    let res=await fetch('/scan-wifi');let arr=await res.json();";
    html += "    if(!arr.length){list.innerHTML='<span style=\"color:var(--muted);font-size:12px;\">Nenhuma rede encontrada.</span>';return;}";
    html += "    let s='<select onchange=\"document.getElementById(\\'wifiSsid\\').value=this.value\" style=\"margin-top:4px;\"><option value=\"\">Selecione a rede:</option>';";
    html += "    arr.forEach(w=>{s+='<option value=\"'+w.ssid+'\">'+w.ssid+' ('+w.rssi+' dBm)'+(w.secure?' 🔒':'')+'</option>';});";
    html += "    s+='</select>';list.innerHTML=s;";
    html += "  }catch(e){list.innerHTML='<span style=\"color:#FF2D55;font-size:12px;\">Erro ao escanear redes.</span>';}";
    html += "}";
    html += "</script></main></body></html>";

    return html;
}
