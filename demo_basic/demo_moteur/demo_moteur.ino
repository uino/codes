// WORK IN PROGRESS



// position courante (valeur du capteur optique)
volatile int cur_pos;

// nombre de pas du device
int number_of_positions = 2100;

// tolérence en nombre de pas
int tolerence_target = 2;
int tolerence_slow = 10;

// puissance à utiliser pour les déplacements
int cur_power;
int power_high = 200;
int power_low = 50;
int power_for_calibration = -30;

// vitesse courante
double speed = 0.;
long last_date;
int last_pos;
long last_disp_speed;

// destination cible
int target_pos;

// récupère la position courante
int getPos() {
  return cur_pos;
}

// règle la puissance du moteur
void setPower(int power) {
  // pas de changement si pas nécessaire
  if (power == cur_power)
    return;
  cur_power = power;
  if (power >= 0) {
    // pin1 write power
    // pin2 write 0
  } else {
    // pin1 write 0
    // pin2 write -power
  }
}

// calibration initiale
void calibrate() {
  setPower(power_for_calibration);
  delay(200); // assez pour laisser le temps d'accélerer
  int last_pos = getPos();
  while (1) {
    delay(20); // assez pour laisser le temps de bouger
    int pos = getPos();
    if (pos == lastPos) { // arrêté
      break;
    }
  }
  cur_pos = 0; // fixe la référence
  Serial.println("calibration success");
}

void setup() {
  Serial.begin(9600);
  Serial.println("starting");

  // TODO: enregistre cur_pos comme variable pour le capteur

  calibrate();

  cur_power = 0;
  last_disp_speed = millis();
  last_date = millis();
  target_pos = getPos(); // on vise de rester sur place
}

void updateTarget() {
  int x = // read sur le potar
  // arrondi la valeur pour désigner 10 cibles possibles.
  int x = (x / 100) * 100;
  // calcul le pas correspondant
  int new_target = (x * number_of_positions) / 1024;

  // juste pour être sûr qu'on ne vise pas en dehors des bornes
  if (new_target < 0) {
    new_target = 0;
  } else if (new_target >= number_of_positions) {
    new_target = number_of_positions;
  }

  // mise à jour si nécessaire
  if (new_target != target_pos) {
    target_pos = new_target;
    Serial.print("New target : ");
    Serial.println(new_target);
  }
}

void measureSpeed() {
  long now = millis();
  long delta_t = now - last_date;
  if (delta_t == 0) { // improbable, mais bon
    return;
  }
  int delta_x = getPos() - last_pos;
  speed = delta_x / delta_t;

  if (now - last_disp_speed > 500) { // period display
    last_disp_speed = now;
    Serial.print("Speed: ");
    Serial.println(speed);
  }
}

void controlMotor() {
  int pos = getPos();
  int dx = target_pos - pos;
  int power = 0;
  if (abs(dx) <= tolerence_target) {
    power = 0;
  } else (abs(dx) <= tolerence_slow) {
    power = power_low;
  } else {
    power = power_high;
  }
  if (dx < 0) {
    power = -power;
  }
  setPower(power);
}

void loop() {

  updateTarget();

  measureSpeed();

  controlMotor();

  sleep(20);
}

