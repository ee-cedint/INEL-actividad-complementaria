float umbral_v_off = 4.3;
float umbral_v_on  = 0.8;
bool last_state = false;

const int pinSensor = A0;
const int pinTransistor = 13;   // salida al transistor

void setup() {
  pinMode(pinSensor, INPUT);
  pinMode(pinTransistor, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int nivelLuz = analogRead(pinSensor);
  float voltaje = (nivelLuz / 1023.0) * 5.0;

  Serial.print("ADC: ");
  Serial.print(nivelLuz);
  Serial.print(" | V(A0): ");
  Serial.print(voltaje, 3);

  if (voltaje > umbral_v_off) {
    digitalWrite(pinTransistor, LOW);
    last_state = false;
    Serial.println(" | Transistor: OFF | LED: OFF");
  } 
  else if (voltaje < umbral_v_on) {
    digitalWrite(pinTransistor, HIGH);
    last_state = true;
    Serial.println(" | Transistor: ON | LED: ON");
  } 
  else {
    digitalWrite(pinTransistor, last_state ? HIGH : LOW);
    Serial.print(" | Transistor: ");
    Serial.print(last_state ? "ON" : "OFF");
    Serial.print(" | LED: ");
    Serial.println(last_state ? "ON" : "OFF");
  }

  delay(200);
}
