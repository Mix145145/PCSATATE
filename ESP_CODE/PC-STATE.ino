#include <WiFi.h>
#include <WebServer.h>
#include <string>

int cpuTemp  = 0;
int cpuUsage = 0;
int gpuTemp  = 0;
int gpuUsage = 0;
String staticData = "";

const char* ssid = "PC-STATE";
const char* password = "00000000";
WebServer server(80);

void setup() {
  Serial.begin(112500);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", prepareHtmlPage());
  });

  server.on("/refresh", HTTP_GET, []() {
    server.send(200, "text/plain", getSensorData());
  });

  server.begin();
}

void loop() {
  if (Serial.available() > 0){
    staticData = Serial.readString();
    cpuTemp = staticData.substring(1,4).toInt();
    cpuUsage = staticData.substring(5,8).toInt();
    gpuTemp = staticData.substring(9,12).toInt();
    gpuUsage = staticData.substring(13,16).toInt();
  }
  server.handleClient();
}

String prepareHtmlPage() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
    .container { max-width: 600px; margin: auto; }
    h1 { color: #0d6efd; }
    p { font-size: 20px; }
    .button { background-color: #0d6efd; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 18px; margin-top: 10px; }
    .footer { margin-top: 30px; color: #6c757d; font-size: 16px; }
  </style>
  <script>
    function refreshData() {
      fetch('/refresh').then(response => response.text()).then(data => {
        document.getElementById('data').innerHTML = data;
      });
    }

    let autoRefresh = false;
    function toggleAutoRefresh() {
      autoRefresh = !autoRefresh;
      document.getElementById('autoRefreshBtn').innerText = autoRefresh ? 'Выключить автообновление' : 'Включить автообновление';
    }

    setInterval(function() {
      if (autoRefresh) {
        refreshData();
      }
    }, 5000);
  </script>
</head>
<body>
  <div class="container">
    <h1>Данные Системы</h1>
    <div id="data">)" + getSensorData() + R"(</div>
    <button class='button' onclick='refreshData()'>Обновить данные</button>
    <button class='button' id='autoRefreshBtn' onclick='toggleAutoRefresh()'>Включить автообновление</button>
  </div>
  <div class='footer'>Авторы проекта: Студенты группы ИИПб-22-1</div>
</body>
</html>
)";
  return html;
}

String getSensorData() {
  return "<p>CPU Температура: " + String(cpuTemp) + " &deg;C</p>"
         "<p>CPU Использование: " + String(cpuUsage) + "%</p>"
         "<p>GPU Температура: " + String(gpuTemp) + " &deg;C</p>"
         "<p>GPU Использование: " + String(gpuUsage) + "%</p>";
}
