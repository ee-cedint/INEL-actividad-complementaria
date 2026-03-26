/*
  4CH "Oscilloscope" for Arduino UNO R4 Minima
  - Samples A0..A3 continuously (fast)
  - Outputs ONLY once per second (so plot/monitor changes max 1 Hz)
  - Output is the average over the last second (more stable)

  Use:
  - Tools -> Serial Plotter (or Serial Monitor)
*/

#include <Arduino.h>

// -------- USER SETTINGS --------
static const uint32_t BAUD = 1000000;

// How fast to sample internally (frames/s). 1 frame = read A0..A3 once.
static const uint32_t SAMPLE_RATE_HZ = 2000;

// How often to PRINT a value (Hz). For "at most once per second" use 1.
static const uint32_t OUTPUT_RATE_HZ = 1;

// Output format
static const bool OUTPUT_VOLTS = true;    // false: raw counts (0..4095), true: volts
static const float VREF_VOLTS  = 4.75f;     // typical on UNO R4
// -------------------------------

static const uint8_t CH_PINS[4] = {A0, A1, A2, A3};
static const uint16_t ADC_BITS = 12;
static const uint16_t ADC_MAX  = (1u << ADC_BITS) - 1; // 4095

static const uint8_t D13_PIN = 13;   // D13 en UNO R4
static bool d13_state = false;

static inline float countsToVolts(float counts) {
  return counts * (VREF_VOLTS / (float)ADC_MAX);
}

void setup() {
  Serial.begin(BAUD);
  while (!Serial) {}

  analogReadResolution(ADC_BITS);

  pinMode(D13_PIN, OUTPUT);
  digitalWrite(D13_PIN, LOW);

  // Warm-up
  (void)analogRead(CH_PINS[0]);
  (void)analogRead(CH_PINS[1]);
  (void)analogRead(CH_PINS[2]);
  (void)analogRead(CH_PINS[3]);

  // Mensaje (mejor verlo en Serial Monitor)
  Serial.println("Envia '1' (D13=HIGH), '0' (D13=LOW), 't' (toggle).");
}

void handleSerial() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '1') d13_state = true;
    else if (c == '0') d13_state = false;
    else if (c == 't' || c == 'T') d13_state = !d13_state;
    else continue;

    digitalWrite(D13_PIN, d13_state ? HIGH : LOW);
  }
}

void loop() {
  handleSerial();

  // Internal sampling cadence
  const uint32_t sample_interval_us = 1000000UL / SAMPLE_RATE_HZ;
  static uint32_t next_sample_us = micros();

  // Output cadence (once per second)
  const uint32_t output_interval_ms = 1000UL / OUTPUT_RATE_HZ;
  static uint32_t last_output_ms = millis();

  // Accumulators for 1-second window
  static uint32_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
  static uint32_t n = 0;

  // Sample at SAMPLE_RATE_HZ
  uint32_t now_us = micros();
  if ((int32_t)(now_us - next_sample_us) >= 0) {
    next_sample_us += sample_interval_us;

    sum0 += (uint32_t)analogRead(CH_PINS[0]);
    sum1 += (uint32_t)analogRead(CH_PINS[1]);
    sum2 += (uint32_t)analogRead(CH_PINS[2]);
    sum3 += (uint32_t)analogRead(CH_PINS[3]);
    n++;
  }

  // Print only once per second (or OUTPUT_RATE_HZ)
  uint32_t now_ms = millis();
  if (now_ms - last_output_ms >= output_interval_ms) {
    last_output_ms += output_interval_ms;

    if (n == 0) return; // por seguridad

    float avg0 = (float)sum0 / (float)n;
    float avg1 = (float)sum1 / (float)n;
    float avg2 = (float)sum2 / (float)n;
    float avg3 = (float)sum3 / (float)n;

    // Reset window
    sum0 = sum1 = sum2 = sum3 = 0;
    n = 0;

    if (!OUTPUT_VOLTS) {
      // Sin etiquetas (rápido). Ideal para Plotter: 4 trazas.
      Serial.print((int)(avg0 + 0.5f)); Serial.print('\t');
      Serial.print((int)(avg1 + 0.5f)); Serial.print('\t');
      Serial.print((int)(avg2 + 0.5f)); Serial.print('\t');
      Serial.println((int)(avg3 + 0.5f));
    } else {
      float v0 = countsToVolts(avg0);
      float v1 = countsToVolts(avg1);
      float v2 = countsToVolts(avg2);
      float v3 = countsToVolts(avg3);

      Serial.print(v0, 3); Serial.print('\t');
      Serial.print(v1, 3); Serial.print('\t');
      Serial.print(v2, 3); Serial.print('\t');
      Serial.println(v3, 3);
    }
  }
}
