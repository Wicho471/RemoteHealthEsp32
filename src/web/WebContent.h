#ifndef WEB_CONTENT_H
#define WEB_CONTENT_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <title>MyHealthLink</title>
  <link rel="stylesheet" href="/styles.css">
</head>
<body>
  <h1>Bienvenido a MyHealthLink</h1>
  <p>Seleccione la red WiFi y escriba la contraseña</p>
  <select id="ssid"></select><br>
  <input type="password" id="password" placeholder="Contraseña"><br>
  <button onclick="connectWiFi()">Conectar</button>

  <div id="qrcode"></div>

  <script src="/qrcode.min.js.gz"></script>
  <script src="/main.js"></script>
</body>
</html>
)rawliteral";

const char MAIN_JS[] PROGMEM = R"rawliteral(
async function loadNetworks() {
  let response = await fetch('/scan');
  let networks = await response.json();
  let select = document.getElementById('ssid');
  select.innerHTML = '';
  networks.forEach(ssid => {
    let option = document.createElement('option');
    option.value = ssid;
    option.textContent = ssid;
    select.appendChild(option);
  });
}

async function connectWiFi() {
  let ssid = document.getElementById('ssid').value;
  let password = document.getElementById('password').value;
  let response = await fetch('/connect', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ ssid, password })
  });
  let result = await response.text();
  alert(result);
}

let qr;
async function updateQR() {
  try {
    let response = await fetch('/info');
    let data = await response.json();
    let jsonString = JSON.stringify(data);
    console.log("JSON String:", jsonString);

    if (!qr) {
      qr = new QRCode(document.getElementById("qrcode"), {
        text: jsonString,
        width: 200,
        height: 200
      });
    } else {
      document.getElementById("qrcode").innerHTML = "";
      qr = new QRCode(document.getElementById("qrcode"), {
        text: jsonString,
        width: 500,
        height: 500
      });
    }
  } catch (e) {
    console.error("Error al actualizar QR", e);
  }
}

loadNetworks();
setInterval(updateQR, 5000);
)rawliteral";

const char STYLES_CSS[] PROGMEM = R"rawliteral(
body {
  font-family: Arial, sans-serif;
  background: #f4f4f4;
  text-align: center;
}
h1 {
  color: #2c3e50;
}
select, input, button {
  padding: 10px;
  margin: 10px;
  font-size: 16px;
}
#qrcode {
  margin: 30px auto;     
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100%;
}
#qrcode canvas, #qrcode img {
  width: 300px !important;   /* tamaño fijo más grande */
  height: 300px !important;
}

)rawliteral";

#endif
