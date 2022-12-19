#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

IPAddress Ip(192, 168, 0, 1);
IPAddress NMask(255, 255, 255, 0);

const uint8_t a[9] = {1, 3, D8, D7, D6, D5, D4, D3, D2};

String sliderValue = "1000";

const char* PARAM_INPUT = "value";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Metro Speed Controller</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; }
  </style>
</head>
<body>
  <h2>Metro Speed Controller</h2>
  <p><span id="textSliderValue">%SLIDERVALUE% Milliseconds</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="1000" max="5000" value="%SLIDERVALUE%" step="1" class="slider"></p>
<script>
function updateSliderPWM(element) {
  var sliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("textSliderValue").innerHTML = sliderValue +" Milliseconds";
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+sliderValue, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

String processor(const String& var) {
  if (var == "SLIDERVALUE") {
    return sliderValue;
  }
  return String();
}

void setup() {

  WiFi.softAPConfig(Ip, Ip, NMask);

  WiFi.mode(WIFI_STA);

  if (!WiFi.softAP("MetroModel", "MetroModelPassword"))
  {
    return;
  }

  for (int i = 0; i < 9; i++)
  {
    pinMode(a[i], OUTPUT);
  }

  delay(100);
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage + " Milliseconds";
    }

    else
    {
      inputMessage = "No message sent";
    }

    request->send(200, "text/plain", "OK");
  });
  server.begin();
}

void loop() {

  for (int i = 0; i < 9; i++)
  {
    digitalWrite(a[i], LOW);
    delay(sliderValue.toInt());
    digitalWrite(a[i], HIGH);
  }

  delay(sliderValue.toInt());

  for (int i = 8; i >= 0; i--)
  {
    digitalWrite(a[i], LOW);
    delay(sliderValue.toInt());
    digitalWrite(a[i], HIGH);
  }
}
