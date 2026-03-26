// ============================================================
// PRACTICAS INEL SESIÓN 2 - ACTIVIDADES 1 Y 2
// Compatible con Arduino Uno R3 y Uno R4 Minima 
// cambiando BOARD_IS_R4=true/false
//
// ACTIVIDAD 1 (BJT):
//   D13 -> Rin -> base
//   A0 = Vb
//   A1 = Ve
//   A2 = Vc
//   A3 = Vcc (opcional)
//
// ACTIVIDAD 2 (MOSFET PWM):
//   D9 -> gate
//   A0 = Vg
//   A1 = Vd
//   A2 = Vled o Vr (opcional)
//   A3 = Vcc (opcional)
//
// COMANDOS SERIE:
//   h        ayuda
//   m1       modo actividad 1 (BJT)
//   m2       modo actividad 2 (MOSFET)
//
//   0        en modo BJT: D13 = LOW
//   1        en modo BJT: D13 = HIGH
//   t        en modo BJT: toggle D13
//
//   a       en modo MOSFET: PWM 0%
//   b       en modo MOSFET: PWM 25%
//   c       en modo MOSFET: PWM 50%
//   d       en modo MOSFET: PWM 75%
//   e       en modo MOSFET: PWM 100%
//   t       en modo MOSFET: siguiente PWM
//
// Salida periódica:
//   A0<TAB>A1<TAB>A2<TAB>A3
// ============================================================


// ---------------- USER SETTINGS ----------------

// Placa: true = Uno R4 Minima, false = Uno R3
static const bool BOARD_IS_R4 = true;

// Mostrar en voltios o cuentas
static const bool OUTPUT_VOLTS = true;

// Velocidad serie
static const uint32_t BAUD = 1000000;

// Muestreo interno (frames/s)
static const uint32_t SAMPLE_RATE_HZ = 2000;

// Frecuencia de impresión por serie
static const uint32_t OUTPUT_RATE_HZ = 2;

// Referencia ADC usada para convertir a voltios
static const float VREF_VOLTS = 5.0f;

// Resolución ADC según placa
static const uint8_t ADC_BITS = BOARD_IS_R4 ? 12 : 10;

// ------------------------------------------------


// Pines analógicos
static const uint8_t CH_PINS[4] = {A0, A1, A2, A3};

// Pines de control
static const uint8_t BJT_PIN = 13;      // Actividad 1
static const uint8_t MOSFET_PIN = 9;    // Actividad 2 (PWM)

// Parámetros derivados
static const uint16_t ADC_MAX = (1u << ADC_BITS) - 1;

// Estados
enum Mode {
  MODE_BJT = 1,
  MODE_MOSFET = 2
};

static Mode currentMode = MODE_MOSFET;
static bool bjtState = false;
static uint8_t pwmPercent = 0;


// ---------- Utilidades ----------

static inline float countsToVolts(float counts) {
  return counts * (VREF_VOLTS / (float)ADC_MAX);
}

static inline uint8_t pwmPercentToValue(uint8_t percent) {
  if (percent >= 100) return 255;
  return (uint8_t)((percent * 255UL) / 100UL);
}

void applyOutputs() {
  if (currentMode == MODE_BJT) {
    digitalWrite(BJT_PIN, bjtState ? HIGH : LOW);
    analogWrite(MOSFET_PIN, 0);
  } else {
    digitalWrite(BJT_PIN, LOW);
    analogWrite(MOSFET_PIN, pwmPercentToValue(pwmPercent));
  }
}

void printStatus() {
  Serial.println();
  Serial.println(F("----- ESTADO -----"));
  Serial.print(F("Modo: "));
  if (currentMode == MODE_BJT) {
    Serial.println(F("Actividad 1 - BJT"));
    Serial.print(F("D13 = "));
    Serial.println(bjtState ? F("HIGH") : F("LOW"));
  } else {
    Serial.println(F("Actividad 2 - MOSFET PWM"));
    Serial.print(F("PWM D9 = "));
    Serial.print(pwmPercent);
    Serial.println(F("%"));
  }

  Serial.print(F("Placa: "));
  Serial.println(BOARD_IS_R4 ? F("UNO R4 Minima") : F("UNO R3"));

  Serial.print(F("ADC bits: "));
  Serial.println(ADC_BITS);

  Serial.print(F("Vref conversion: "));
  Serial.println(VREF_VOLTS, 3);
  Serial.println(F("------------------"));
  Serial.println();
}

void printHelp() {
  Serial.println();
  Serial.println(F("=== COMANDOS ==="));
  Serial.println(F("h        -> ayuda"));
  Serial.println(F("s        -> estado actual"));
  Serial.println(F("m1       -> modo actividad 1 (BJT, D13)"));
  Serial.println(F("m2       -> modo actividad 2 (MOSFET PWM, D9)"));
  Serial.println();
  Serial.println(F("Modo BJT:"));
  Serial.println(F("  0      -> D13 LOW"));
  Serial.println(F("  1      -> D13 HIGH"));
  Serial.println(F("  t      -> toggle D13"));
  Serial.println();
  Serial.println(F("Modo MOSFET PWM:"));
  Serial.println(F("  a     -> PWM 0%"));
  Serial.println(F("  b     -> PWM 25%"));
  Serial.println(F("  c     -> PWM 50%"));
  Serial.println(F("  d     -> PWM 75%"));
  Serial.println(F("  e     -> PWM 100%"));
  Serial.println(F("  t     -> siguiente PWM: 0,25,50,75,100,0..."));
  Serial.println();
  Serial.println(F("Salida: A0<TAB>A1<TAB>A2<TAB>A3"));
  Serial.println();
}

void handleCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "h" || cmd == "help") {
    printHelp();
    return;
  }

  if (cmd == "s") {
    printStatus();
    return;
  }

  if (cmd == "m1") {
    currentMode = MODE_BJT;
    applyOutputs();
    Serial.println(F("Modo cambiado a Actividad 1 (BJT)"));
    return;
  }

  if (cmd == "m2") {
    currentMode = MODE_MOSFET;
    applyOutputs();
    Serial.println(F("Modo cambiado a Actividad 2 (MOSFET PWM)"));
    return;
  }

  if (currentMode == MODE_BJT) {
    if (cmd == "0") {
      bjtState = false;
      applyOutputs();
      Serial.println(F("D13 = LOW"));
      return;
    }

    if (cmd == "1") {
      bjtState = true;
      applyOutputs();
      Serial.println(F("D13 = HIGH"));
      return;
    }

    if (cmd == "t" || cmd == "T") {
      bjtState = !bjtState;
      applyOutputs();
      Serial.print(F("D13 = "));
      Serial.println(bjtState ? F("HIGH") : F("LOW"));
      return;
    }
  }

  if (currentMode == MODE_MOSFET) {
    if (cmd == "a") {
      pwmPercent = 0;
      applyOutputs();
      Serial.println(F("PWM D9 = 0%"));
      return;
    }

    if (cmd == "b") {
      pwmPercent = 25;
      applyOutputs();
      Serial.println(F("PWM D9 = 25%"));
      return;
    }

    if (cmd == "c") {
      pwmPercent = 50;
      applyOutputs();
      Serial.println(F("PWM D9 = 50%"));
      return;
    }

    if (cmd == "d") {
      pwmPercent = 75;
      applyOutputs();
      Serial.println(F("PWM D9 = 75%"));
      return;
    }

    if (cmd == "e") {
      pwmPercent = 100;
      applyOutputs();
      Serial.println(F("PWM D9 = 100%"));
      return;
    }
    
    if (cmd == "t" || cmd == "T") {
      pwmPercent += 25;
      if (pwmPercent > 100) {
        pwmPercent = 0;
      }
      applyOutputs();
      Serial.print(F("PWM D9 = "));
      Serial.print(pwmPercent);
      Serial.println(F("%"));
      return;
  	}
  }

  Serial.print(F("Comando no reconocido: "));
  Serial.println(cmd);
}

void handleSerial() {
  while (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    handleCommand(cmd);
  }
}


// ---------- Setup ----------

void setup() {
  Serial.begin(BAUD);
  while (!Serial) {}

  pinMode(BJT_PIN, OUTPUT);
  pinMode(MOSFET_PIN, OUTPUT);

  digitalWrite(BJT_PIN, LOW);
  analogWrite(MOSFET_PIN, 0);

#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  if (BOARD_IS_R4) {
    analogReadResolution(12);
  }
#endif

  // Warm-up ADC
  for (uint8_t i = 0; i < 4; i++) {
    (void)analogRead(CH_PINS[i]);
  }

  printHelp();
  printStatus();
  applyOutputs();
}


// ---------- Loop ----------

void loop() {
  handleSerial();

  const uint32_t sampleIntervalUs = 1000000UL / SAMPLE_RATE_HZ;
  static uint32_t nextSampleUs = micros();

  const uint32_t outputIntervalMs = 1000UL / OUTPUT_RATE_HZ;
  static uint32_t lastOutputMs = millis();

  static uint32_t sum0 = 0;
  static uint32_t sum1 = 0;
  static uint32_t sum2 = 0;
  static uint32_t sum3 = 0;
  static uint32_t n = 0;

  uint32_t nowUs = micros();
  if ((int32_t)(nowUs - nextSampleUs) >= 0) {
    nextSampleUs += sampleIntervalUs;

    sum0 += (uint32_t)analogRead(CH_PINS[0]);
    sum1 += (uint32_t)analogRead(CH_PINS[1]);
    sum2 += (uint32_t)analogRead(CH_PINS[2]);
    sum3 += (uint32_t)analogRead(CH_PINS[3]);
    n++;
  }

  uint32_t nowMs = millis();
  if (nowMs - lastOutputMs >= outputIntervalMs) {
    lastOutputMs += outputIntervalMs;

    if (n == 0) return;

    float avg0 = (float)sum0 / (float)n;
    float avg1 = (float)sum1 / (float)n;
    float avg2 = (float)sum2 / (float)n;
    float avg3 = (float)sum3 / (float)n;

    sum0 = 0;
    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    n = 0;

    if (!OUTPUT_VOLTS) {
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