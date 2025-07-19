#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include <driver/dac.h>

#define CRANK_DAC DAC_CHANNEL_2  // GPIO26
#define CAM_DAC   DAC_CHANNEL_1  // GPIO25

const char* ssid = "VR-Simulator";
const char* password = "12345678";
WebServer server(80);

const int samples_per_cycle = 100;
uint8_t sine_wave[samples_per_cycle];

volatile int crank_delay_us = 200; // domyślnie 50 Hz
volatile int cam_offset_us = 0;
volatile bool run_simulator = false;

unsigned long last_cam_time_us = 0;
bool cam_active = false;
int cam_index = 0;
int crank_index = 0;

const unsigned long cam_period_us = 398400; // co 398.4 ms

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><title>VR Trigger</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>body{font-family:sans-serif;text-align:center;margin-top:40px;}input{width:200px;}</style>
</head><body>
<h2>Honda 24+1 VR Trigger</h2>
<label>RPM: <span id="rpmVal">125</span></label><br>
<input type="range" min="50" max="1000" value="125" id="rpm"><br><br>
<label>CAM Offset (ms): <span id="offsetVal">0</span></label><br>
<input type="range" min="0" max="1000" value="0" id="offset"><br><br>
<button onclick="fetch('/start')">Start</button>
<button onclick="fetch('/stop')">Stop</button>
<script>
let rpmSlider = document.getElementById('rpm');
let offsetSlider = document.getElementById('offset');
function updateParam() {
  fetch(`/set?rpm=${rpmSlider.value}&offset=${offsetSlider.value}`);
  document.getElementById('rpmVal').innerText = rpmSlider.value;
  document.getElementById('offsetVal').innerText = offsetSlider.value;
}
rpmSlider.oninput = updateParam;
offsetSlider.oninput = updateParam;
</script>
</body></html>
)rawliteral";

void setupWiFi() {
  WiFi.softAP(ssid, password);
  server.on("/", []() { server.send_P(200, "text/html", index_html); });
  server.on("/start", []() { run_simulator = true; server.send(200, "text/plain", "Started"); });
  server.on("/stop", []() { run_simulator = false; server.send(200, "text/plain", "Stopped"); });
  server.on("/set", []() {
    if (server.hasArg("rpm")) {
      int rpm = server.arg("rpm").toInt();
      crank_delay_us = (1000000UL * 60) / (rpm * samples_per_cycle);  // czyli 60 sek w µs    }
    if (server.hasArg("offset")) {
      cam_offset_us = server.arg("offset").toInt() * 1000;
    }
    server.send(200, "text/plain", "OK");
  });
  server.begin();
}

void setup() {
  dac_output_enable(CRANK_DAC);
  dac_output_enable(CAM_DAC);
  for (int i = 0; i < samples_per_cycle; i++) {
    float theta = 2.0 * PI * i / samples_per_cycle;
    sine_wave[i] = (uint8_t)((sin(theta) + 1.0) * 127.5);
  }
  setupWiFi();
}

void loop() {
  server.handleClient();
  if (!run_simulator) return;

  dac_output_voltage(CRANK_DAC, sine_wave[crank_index]);
  crank_index = (crank_index + 1) % samples_per_cycle;

  unsigned long now_us = micros();
  if (!cam_active && (now_us - last_cam_time_us) >= (cam_period_us + cam_offset_us)) {
    cam_active = true;
    last_cam_time_us = now_us;
    cam_index = 0;
  }

  if (cam_active) {
    dac_output_voltage(CAM_DAC, sine_wave[cam_index]);
    cam_index++;
    if (cam_index >= samples_per_cycle) {
      cam_active = false;
      dac_output_voltage(CAM_DAC, 0);
    }
  }

  delayMicroseconds(crank_delay_us);
}
