int s[8], sum;
int position[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
int reflectance = 600, sensor;
int motorlimit = 130;
int lmotorspd = 180, Rmotorspd = 180;
int kp = 75, kd = 600;
int turningspd = 190;
float midpoint = 4.1;

int lmotor, rmotor;
float weightavg;
int PID;
float error, last_error;
int turn;

void setup() {
  pinMode(7, OUTPUT); //IN1
  pinMode(4, OUTPUT); //IN2
  pinMode(3, OUTPUT); //IN3
  pinMode(2, OUTPUT); //IN4

  pinMode(8, INPUT_PULLUP);//START BUTTON
  pinMode(13, OUTPUT);//INDICATION
  Serial.begin(9600);
}

void loop() {
  while (digitalRead(8) != 1) {
    flashLED();
    while (1) {
      PID_reading();
      if (sum == 0) {
        if (turn != 's') {
          delay(30);
          motor(0, 0);
          if (turn == 'r') {
            motor(turningspd, -turningspd);
          }
          else {
            motor(-turningspd, turningspd);
          }
          while (s[3] != 1 && s[4] != 1) PID_reading();
          turn = 's';
        }
      }

      PID_reading();
      error = midpoint - weightavg;
      PID = (error * kp) + (kd)(error - last_error);
      last_error = error;

      rmotor = Rmotorspd - PID;
      lmotor = lmotorspd + PID;
      motor(lmotor, rmotor);

      if (s[0] == 1 && s[7] != 1) turn = 'r';
      if (s[7] == 1 && s[0] != 1) turn = 'l';
      else if (sum == 8) {
        delay(20);
        PID_reading();
        if (sum == 8) {
          motor(0, 0);
          while (sum == 8) PID_reading();
        }
        else if (sum == 0) turn = 'r';
      }
    }
  }
}

void flashLED() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
  }
}

void PID_reading() {
  sensor = 0;
  sum = 0;
  for (byte i = 0; i < 8; i++) {
    s[i] = analogRead(i);
    if (s[i] > reflectance) s[i] = 1;
    else s[i] = 0;
    sensor += s[i] * position[i];
    sum += s[i];
  }
  weightavg = sensor / sum;
}

void motor(int a, int b) {
  if (a > 0) {
    digitalWrite(3, 1);
    digitalWrite(2, 0);
  }
  else {
    a = -(a);
    digitalWrite(3, 0);
    digitalWrite(2, 1);
  }
  if (b > 0) {
    digitalWrite(7, 1);
    digitalWrite(4, 0);
  }
  else {
    b = -(b);
    digitalWrite(7, 0);
    digitalWrite(4, 1);
  }

  if (a > motorlimit) a = motorlimit;
  if (b > motorlimit) b = motorlimit;

  analogWrite(5, a);
  analogWrite(6, b);
}
