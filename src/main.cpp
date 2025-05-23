#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* SSID = "LASEC";
const char* password = "lasecca123";
WebServer server(80);

// Definições dos pinos
#define M_DIREITA_HORARIO     23
#define M_DIREITA_ANTI        22
#define M_ESQUERDA_HORARIO    19
#define M_ESQUERDA_ANTI       18

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <title>Controle Robótico - Oficina Robótica Nacional 2025</title>
  <style>
    :root {
      --primary: #0066cc;
      --accent: #ff6600;
      --text: #333333;
      --background: #f8f9fa;
      --vermelhoNacional: #C51A1B;
    }

    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      font-family: 'Segoe UI', Roboto, sans-serif;
    }

    body {
      background-color: var(--background);
      color: var(--text);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 20px;
      text-align: center;
    }

    .header {
      margin-bottom: 2rem;
      width: 100%;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    .logo {
      max-width: 280px;
      width: 100%;
      height: auto;
      margin-bottom: 1rem;
    }

    h1 {
      color: var(--primary);
      font-size: 1.8rem;
      margin-bottom: 0.5rem;
      font-weight: 700;
    }

    .year {
      color: var(--vermelhoNacional);
      font-size: 1.4rem;
      margin-bottom: 2rem;
      font-weight: 600;
    }

    .control-panel {
      width: 100%;
      max-width: 340px;
      margin-bottom: 2rem;
    }

    .control-row {
      display: flex;
      justify-content: center;
      margin: 14px 0;
    }

    .btn {
      width: 110px;
      height: 70px;
      margin: 0 8px;
      border: none;
      border-radius: 12px;
      font-size: 1.1rem;
      font-weight: 600;
      color: white;
      cursor: pointer;
      transition: all 0.2s;
      box-shadow: 0 4px 8px rgba(0,0,0,0.1);
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      user-select: none;
      -webkit-user-select: none;
      -webkit-touch-callout: none;
      touch-action: manipulation;
    }

    .btn:active {
      transform: translateY(2px);
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }

    .btn-up { background-color: #27ae60; }
    .btn-down { background-color: #e74c3c; }
    .btn-left { background-color: #f39c12; }
    .btn-right { background-color: #3498db; }

    .btn-icon {
      font-size: 1.6rem;
      margin-bottom: 4px;
    }

    .footer {
      margin-top: auto;
      color: #7f8c8d;
      font-size: 0.9rem;
      padding: 1rem;
    }

    @media (max-width: 480px) {
      .logo { max-width: 200px; }
      h1 { font-size: 1.5rem; }
      .year { font-size: 1.2rem; }
      .btn {
        width: 120px;
        height: 80px;
        font-size: 1.2rem;
      }
      .btn-icon {
        font-size: 2rem;
      }
    }

    @media (orientation: landscape) and (max-width: 900px) {
      body {
        flex-direction: row;
        flex-wrap: wrap;
        justify-content: center;
        align-items: center;
        padding: 10px;
      }

      .header {
        width: 100%;
      }

      .logo {
        max-width: 280px;
        margin: 1rem auto;
      }

      .control-panel {
        max-width: 300px;
      }

      h1 {
        font-size: 1.3rem;
      }

      .year {
        font-size: 1rem;
      }

      .btn {
        width: 100px;
        height: 70px;
        font-size: 1rem;
      }

      .btn-icon {
        font-size: 1.6rem;
      }
    }
  </style>
</head>
<body>
  <div class="header">
    <img class="logo" src="data:image/png;base64,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ZmYvMS4wLyIKICAgIHhtbG5zOnhtcD0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wLyIKICAgeG1wTU06RG9jdW1lbnRJRD0iZ2ltcDpkb2NpZDpnaW1wOjZkNTdlN2IyLWEyNDctNGQ0Ni05ZmZkLTc5Y2M4ZDdlOWU0OSIKICAgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDpkODU2YmRiZi0wN2ZmLTRlMzItYjNlNS1hOGI5ZmNiN2I5ZjYiCiAgIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDpkZGE1MGQ4NC1kMjc1LTQ2YTktOTEzYi04NjBiMzU0NDc4NjEiCiAgIEdJTVA6QVBJPSIyLjAiCiAgIEdJTVA6UGxhdGZvcm09IldpbmRvd3MiCiAgIEdJTVA6VGltZVN0YW1wPSIxNjY0NTgyMjk4NDY5OTA5IgogICBHSU1QOlZlcnNpb249IjIuMTAuMjIiCiAgIGRjOkZvcm1hdD0iaW1hZ2UvcG5nIgogICB0aWZmOk9yaWVudGF0aW9uPSIxIgogICB4bXA6Q3JlYXRvclRvb2w9IkdJTVAgMi4xMCI+CiAgIDxpcHRjRXh0OkxvY2F0aW9uQ3JlYXRlZD4KICAgIDxyZGY6QmFnLz4KICAgPC9pcHRjRXh0OkxvY2F0aW9uQ3JlYXRlZD4KICAgPGlwdGNFeHQ6TG9jYXRpb25TaG93bj4KICAgIDxyZGY6QmFnLz4KICAgPC9pcHRjRXh0OkxvY2F0aW9uU2hvd24+CiAgIDxpcHRjRXh0OkFydHdvcmtPck9iamVjdD4KICAgIDxyZGY6QmFnLz4KICAgPC9pcHRjRXh0OkFydHdvcmtPck9iamVjdD4KICAgPGlwdGNFeHQ6UmVnaXN0cnlJZD4KICAgIDxyZGY6QmFnLz4KICAgPC9pcHRjRXh0OlJlZ2lzdHJ5SWQ+CiAgIDx4bXBNTTpIaXN0b3J5PgogICAgPHJkZjpTZXE+CiAgICAgPHJkZjpsaQogICAgICBzdEV2dDphY3Rpb249InNhdmVkIgogICAgICBzdEV2dDpjaGFuZ2VkPSIvIgogICAgICBzdEV2dDppbnN0YW5jZUlEPSJ4bXAuaWlkOjhiYzFlMmY4LWJkZmYtNDIwNi05NjY3LWI1M2M3YWE4MWQwNCIKICAgICAgc3RFdnQ6c29mdHdhcmVBZ2VudD0iR2ltcCAyLjEwIChXaW5kb3dzKSIKICAgICAgc3RFdnQ6d2hlbj0iMjAyMi0wOS0zMFQyMDo1ODoxOCIvPgogICAgPC9yZGY6U2VxPgogICA8L3htcE1NOkhpc3Rvcnk+CiAgIDxwbHVzOkltYWdlU3VwcGxpZXI+CiAgICA8cmRmOlNlcS8+CiAgIDwvcGx1czpJbWFnZVN1cHBsaWVyPgogICA8cGx1czpJbWFnZUNyZWF0b3I+CiAgICA8cmRmOlNlcS8+CiAgIDwvcGx1czpJbWFnZUNyZWF0b3I+CiAgIDxwbHVzOkNvcHlyaWdodE93bmVyPgogICAgPHJkZjpTZXEvPgogICA8L3BsdXM6Q29weXJpZ2h0T3duZXI+CiAgIDxwbHVzOkxpY2Vuc29yPgogICAgPHJkZjpTZXEvPgogICA8L3BsdXM6TGljZW5zb3I+CiAgPC9yZGY6RGVzY3JpcHRpb24+CiA8L3JkZjpSREY+CjwveDp4bXBtZXRhPgogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgCiAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAKICAgICAgICAgICAgICAgICAgICAgICAgICAgCjw/eHBhY2tldCBlbmQ9InciPz5qTnt4AAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH5gkeFzoR529bnAAAIABJREFUeNrtnc11GznWht/+zuyHsyUXLkdges2FSxFYikBUBJIiEBWB5AhER2A6ApUXXIuOQOwFuW1OBP0tCjVNy6x/APX3POfoeEYtFlH4ubgvcHEhAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA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" alt="Logo Oficina Robótica Nacional">
    <h1>OFICINA ROBÓTICA NACIONAL</h1>
    <div class="year">2025</div>
  </div>

  <div class="control-panel">
    <div class="control-row">
      <button class="btn btn-up"
              onmousedown="startCommand('up')" onmouseup="stopCommand('up')"
              ontouchstart="startCommand('up')" ontouchend="stopCommand('up')">
        <span class="btn-icon">↑</span><span>Up</span>
      </button>
    </div>
    <div class="control-row">
      <button class="btn btn-left"
              onmousedown="startCommand('left')" onmouseup="stopCommand('left')"
              ontouchstart="startCommand('left')" ontouchend="stopCommand('left')">
        <span class="btn-icon">←</span><span>Left</span>
      </button>
      <button class="btn btn-right"
              onmousedown="startCommand('right')" onmouseup="stopCommand('right')"
              ontouchstart="startCommand('right')" ontouchend="stopCommand('right')">
        <span class="btn-icon">→</span><span>Right</span>
      </button>
    </div>
    <div class="control-row">
      <button class="btn btn-down"
              onmousedown="startCommand('down')" onmouseup="stopCommand('down')"
              ontouchstart="startCommand('down')" ontouchend="stopCommand('down')">
        <span class="btn-icon">↓</span><span>Down</span>
      </button>
    </div>
  </div>

  <div class="footer">
    Sistema de Controle Robótico - Oficina Robótica Nacional 2025
  </div>

  <script>
    let pressInterval;

    function startCommand(dir) {
      sendCommand(dir, 'on');
      pressInterval = setInterval(() => sendCommand(dir, 'on'), 150);
    }

    function stopCommand(dir) {
      if (pressInterval) {
        clearInterval(pressInterval);
        pressInterval = null;
      }
      sendCommand(dir, 'off');
    }

    function sendCommand(dir, action) {
      fetch(`/${dir}/${action}`)
        .then(res => {
          if (!res.ok) throw new Error("Erro no comando");
        })
        .catch(err => console.error(err));
    }
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void pararMotores() {
  digitalWrite(M_DIREITA_HORARIO, LOW);
  digitalWrite(M_DIREITA_ANTI, LOW);
  digitalWrite(M_ESQUERDA_HORARIO, LOW);
  digitalWrite(M_ESQUERDA_ANTI, LOW);
}

void handleDirection(String dir, String action) {
  bool ligar = (action == "on");
  pararMotores(); // sempre para antes de mudar direção

  Serial.printf("Comando: %s %s\n", dir.c_str(), ligar ? "LIGAR" : "DESLIGAR");

  if (!ligar) {
    pararMotores();
    server.send(200, "text/plain", "Motores parados");
    return;
  }

  if (dir == "up") {
    digitalWrite(M_DIREITA_HORARIO, HIGH);
    digitalWrite(M_ESQUERDA_HORARIO, HIGH);
  } else if (dir == "down") {
    digitalWrite(M_DIREITA_ANTI, HIGH);
    digitalWrite(M_ESQUERDA_ANTI, HIGH);
  } else if (dir == "left") {
    digitalWrite(M_DIREITA_HORARIO, HIGH);
    digitalWrite(M_ESQUERDA_ANTI, HIGH);
  } else if (dir == "right") {
    digitalWrite(M_DIREITA_ANTI, HIGH);
    digitalWrite(M_ESQUERDA_HORARIO, HIGH);
  }

  server.send(200, "text/plain", "Comando executado");
}

void handleNotFound() {
  server.send(404, "text/plain", "Recurso não encontrado");
}

void setup() {
  Serial.begin(115200);

  pinMode(M_DIREITA_HORARIO, OUTPUT);
  pinMode(M_DIREITA_ANTI, OUTPUT);
  pinMode(M_ESQUERDA_HORARIO, OUTPUT);
  pinMode(M_ESQUERDA_ANTI, OUTPUT);
  pararMotores();

  WiFi.begin(SSID, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/up/on",    []() { handleDirection("up", "on"); });
  server.on("/up/off",   []() { handleDirection("up", "off"); });
  server.on("/down/on",  []() { handleDirection("down", "on"); });
  server.on("/down/off", []() { handleDirection("down", "off"); });
  server.on("/left/on",  []() { handleDirection("left", "on"); });
  server.on("/left/off", []() { handleDirection("left", "off"); });
  server.on("/right/on", []() { handleDirection("right", "on"); });
  server.on("/right/off",[]() { handleDirection("right", "off"); });

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient();
}
