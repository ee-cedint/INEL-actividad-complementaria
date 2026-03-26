const int pinSensor = A0;
const int pinPWM = 9;   // pin PWM
const float Vcc = 5.0;

void setup() {
  pinMode(pinSensor, INPUT);
  pinMode(pinPWM, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int nivelLuz = analogRead(pinSensor);
  float voltaje = (nivelLuz / 1023.0) * Vcc;

  // Más luz -> menos brillo
  int pwm = map(nivelLuz, 0, 1023, 255, 0);

  // Limitar por seguridad
  pwm = constrain(pwm, 0, 255);

  analogWrite(pinPWM, pwm);

  Serial.print("ADC: ");
  Serial.print(nivelLuz);
  Serial.print(" | V(A0): ");
  Serial.print(voltaje, 3);
  Serial.print(" | PWM: ");
  Serial.print(pwm);
  Serial.print(" | Duty: ");
  Serial.print((pwm / 255.0) * 100.0, 1);
  Serial.println("%");

  delay(500);
}