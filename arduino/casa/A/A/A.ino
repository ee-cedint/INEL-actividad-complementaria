float umbral_v_off = 4.3;
float umbral_v_on  = 0.8;
bool last_state = false;

void setup() {
  pinMode(A0, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int nivelLuz = analogRead(A0);
  float voltaje = (nivelLuz / 1023.0) * 5.0;

  Serial.print("ADC: ");
  Serial.print(nivelLuz);
  Serial.print(" | V(A0): ");
  Serial.print(voltaje, 3);

  // Solo calcular R_LDR si el voltaje no es casi cero
  float R_fijo = 10000.0;
  if (voltaje > 0.01 && voltaje < 4.99) {
    float R_LDR = R_fijo * ((5.0 / voltaje) - 1.0);
    Serial.print(" | R_LDR: ");
    Serial.print(R_LDR, 1);
  } else {
    Serial.print(" | R_LDR: fuera de rango");
  }

  if (voltaje > umbral_v_off) {
    digitalWrite(13, LOW);
    last_state = false;
    Serial.println(" | LED: OFF");
  } 
  else if (voltaje < umbral_v_on) {
    digitalWrite(13, HIGH);
    last_state = true;
    Serial.println(" | LED: ON");
  } 
  else {
    digitalWrite(13, last_state ? HIGH : LOW);
    Serial.print(" | LED: ");
    Serial.println(last_state ? "ON" : "OFF");
  }

  delay(500);
}