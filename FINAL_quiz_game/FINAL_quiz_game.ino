#include <LiquidCrystal.h>

// ===== PIN CONFIG START =====
const int RS = 2, E = 3, D4 = 4, D5 = 5, D6 = 6, D7 = 7;
const int BTN_A = 8, BTN_B = 9, BTN_C = 10, BTN_D = 11;
const int PIEZO = 12;

const int RGB_RED   = 13;
const int RGB_GREEN = A0;
const int RGB_BLUE  = A1;

// 7-segment timer bar pins
const int SEG_F = A2, SEG_A = A3, SEG_B = A4, SEG_G = A5;
// ===== PIN CONFIG END =====

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

const int COMMON_ANODE = 0;
const int QUESTION_TIME = 15;
const int QUESTIONS_PER_GAME = 10;

struct Question {
  const char* q;
  const char* choices[4];
  byte correctIndex;
};

// ===== 20-QUESTION BANK =====
const Question questionBank[] = {
  {"MCU means?",      {"Micro Ctrl", "Main Ctrl", "Mini CPU",  "Memory"},   0},
  {"UNO MCU?",        {"ATmega328P", "ESP32",     "8051",      "PIC16"},    0},
  {"UNO digi pins?",  {"14",         "12",        "16",        "20"},       0},
  {"UNO analog?",     {"6",          "4",         "8",         "10"},       0},
  {"PWM pins?",       {"6",          "4",         "8",         "10"},       0},
  {"UNO clk?",        {"16MHz",      "8MHz",      "20MHz",     "1MHz"},     0},
  {"I2C wires?",      {"2",          "3",         "4",         "1"},        0},
  {"SPI type?",       {"Serial",     "Parallel",  "Analog",    "Optical"},  0},
  {"ADC bits?",       {"10",         "8",         "12",        "16"},       0},
  {"const is?",       {"Read only",  "Variable",  "Function",  "Loop"},     0},
  {"pinMode sets?",   {"In/Out",     "Speed",     "Clock",     "RAM"},      0},
  {"digitalRead?",    {"HIGH/LOW",   "0-255",     "0-1023",    "Text"},     0},
  {"analogWrite?",    {"PWM",        "ADC",       "SPI",       "I2C"},      0},
  {"loop() runs?",    {"Forever",    "Once",      "10x",       "Never"},    0},
  {"setup() runs?",   {"Once",       "Forever",   "On error",  "10x"},      0},
  {"UNO SRAM?",       {"2KB",        "1KB",       "4KB",       "8KB"},      0},
  {"UNO EEPROM?",     {"1KB",        "2KB",       "4KB",       "8KB"},      0},
  {"UNO flash?",      {"32KB",       "16KB",      "8KB",       "64KB"},     0},
  {"UART type?",      {"Serial",     "Parallel",  "Analog",    "PWM"},      0},
  {"RX/TX used for?", {"Serial",     "Power",     "ADC",       "PWM"},      0}
};

const int BANK_SIZE = sizeof(questionBank) / sizeof(questionBank[0]);

int gameQuestions[QUESTIONS_PER_GAME];

const char* dispA;
const char* dispB;
const char* dispC;
const char* dispD;
char correctDisplayLetter;
const char* correctDisplayText;

int score = 0;
int currentQ = 0;
int lives = 3;
int timer = QUESTION_TIME;
bool answering = false;
unsigned long lastTime = 0;

char scrollLine0[64];
char scrollLine1[96];
int len0 = 0;
int len1 = 0;
int scrollIndex = 0;
unsigned long lastScrollTime = 0;

// ===== FUNCTION PROTOTYPES =====
bool checkButton(int pin);
void showStart();
void startQuestion();
void updateDisplay();
void checkAnswer(char ans);
void handleTimeout();
void showScore();
void showGameOver();
void resetGame();
void updateTimerBar(int timeLeft);
void setRGB(int r, int g, int b);
void prepareGameQuestions();
void prepareCurrentQuestion();
void shuffleInts(int arr[], int n);
unsigned long makeSeed();

void setup() {
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_C, INPUT_PULLUP);
  pinMode(BTN_D, INPUT_PULLUP);
  pinMode(PIEZO, OUTPUT);

  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);

  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_G, OUTPUT);

  lcd.begin(16, 2);

  randomSeed(makeSeed());

  showStart();
}

unsigned long makeSeed() {
  unsigned long seed = 0;
  for (int i = 0; i < 32; i++) {
    seed ^= (unsigned long)analogRead(A5);
    seed ^= micros();
    seed <<= 1;
    delay(1);
  }
  return seed;
}

void loop() {
  char pressedBtn = 0;

  if (checkButton(BTN_A)) pressedBtn = 'A';
  else if (checkButton(BTN_B)) pressedBtn = 'B';
  else if (checkButton(BTN_C)) pressedBtn = 'C';
  else if (checkButton(BTN_D)) pressedBtn = 'D';

  if (pressedBtn) {
    if (currentQ >= QUESTIONS_PER_GAME || lives <= 0) {
      resetGame();
      showStart();
    } else if (!answering) {
      startQuestion();
    } else {
      checkAnswer(pressedBtn);
    }
  }

  if (answering) {
    if (millis() - lastScrollTime >= 400) {
      lastScrollTime = millis();
      scrollIndex++;
      updateDisplay();
    }

    if (millis() - lastTime >= 1000) {
      lastTime = millis();
      timer--;
      updateTimerBar(timer);

      if (timer <= 5 && timer > 0) {
        tone(PIEZO, 800, 50);
      }

      if (timer <= 0) {
        handleTimeout();
      }
    }
  }
}

bool checkButton(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(50);
    while (digitalRead(pin) == LOW);
    return true;
  }
  return false;
}

void shuffleInts(int arr[], int n) {
  for (int i = n - 1; i > 0; i--) {
    int j = random(i + 1);
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

void prepareGameQuestions() {
  int indices[BANK_SIZE];
  for (int i = 0; i < BANK_SIZE; i++) {
    indices[i] = i;
  }

  shuffleInts(indices, BANK_SIZE);

  for (int i = 0; i < QUESTIONS_PER_GAME; i++) {
    gameQuestions[i] = indices[i];
  }
}

void prepareCurrentQuestion() {
  const Question& q = questionBank[gameQuestions[currentQ]];

  int order[4] = {0, 1, 2, 3};
  shuffleInts(order, 4);

  dispA = q.choices[order[0]];
  dispB = q.choices[order[1]];
  dispC = q.choices[order[2]];
  dispD = q.choices[order[3]];

  if (order[0] == q.correctIndex) {
    correctDisplayLetter = 'A';
    correctDisplayText = dispA;
  } else if (order[1] == q.correctIndex) {
    correctDisplayLetter = 'B';
    correctDisplayText = dispB;
  } else if (order[2] == q.correctIndex) {
    correctDisplayLetter = 'C';
    correctDisplayText = dispC;
  } else {
    correctDisplayLetter = 'D';
    correctDisplayText = dispD;
  }

  snprintf(scrollLine0, sizeof(scrollLine0),
           "Q%d:%s   ", currentQ + 1, q.q);

  snprintf(scrollLine1, sizeof(scrollLine1),
           "A:%s B:%s C:%s D:%s   ",
           dispA, dispB, dispC, dispD);

  len0 = strlen(scrollLine0);
  len1 = strlen(scrollLine1);
}

void showStart() {
  lcd.clear();
  lcd.print("MCU Quiz Game!");
  lcd.setCursor(0, 1);
  lcd.print("Press any key..");

  currentQ = 0;
  score = 0;
  lives = 3;
  timer = QUESTION_TIME;
  answering = false;

  randomSeed(makeSeed());
  prepareGameQuestions();

  updateTimerBar(timer);
  setRGB(1, 1, 1);
}

void startQuestion() {
  if (currentQ >= QUESTIONS_PER_GAME) {
    showGameOver();
    return;
  }

  answering = true;
  timer = QUESTION_TIME;
  lastTime = millis();
  lastScrollTime = millis();
  scrollIndex = 0;

  prepareCurrentQuestion();
  updateTimerBar(timer);

  lcd.clear();
  updateDisplay();

  setRGB(1, 1, 1);
}

void updateDisplay() {
  lcd.setCursor(0, 0);
  for (int i = 0; i < 16; i++) {
    int idx = (len0 <= 16) ? i : (scrollIndex + i) % len0;
    char ch = (idx < len0) ? scrollLine0[idx] : ' ';
    lcd.print(ch);
  }

  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    int idx = (len1 <= 16) ? i : (scrollIndex + i) % len1;
    char ch = (idx < len1) ? scrollLine1[idx] : ' ';
    lcd.print(ch);
  }
}

void checkAnswer(char ans) {
  answering = false;

  if (ans == correctDisplayLetter) {
    score++;

    lcd.clear();
    lcd.print("Correct!");
    lcd.setCursor(0, 1);
    lcd.print("Score ");
    lcd.print(score);
    lcd.print("/");
    lcd.print(QUESTIONS_PER_GAME);

    setRGB(1, 0, 1);
    tone(PIEZO, 1000, 100);
    delay(120);
    tone(PIEZO, 1500, 100);
    delay(700);
  } else {
    lives--;

    lcd.clear();
    lcd.print("Wrong!");
    lcd.setCursor(0, 1);
    lcd.print("Ans:");
    lcd.print(correctDisplayLetter);
    lcd.print(" ");
    lcd.print(correctDisplayText);

    setRGB(0, 1, 1);
    tone(PIEZO, 200, 300);
    delay(1200);
  }

  setRGB(1, 1, 1);
  currentQ++;

  if (lives <= 0 || currentQ >= QUESTIONS_PER_GAME) {
    showGameOver();
  } else {
    showScore();
    delay(1000);
    startQuestion();
  }
}

void handleTimeout() {
  answering = false;
  lives--;

  updateTimerBar(0);

  lcd.clear();
  lcd.print("Time's up!");
  lcd.setCursor(0, 1);
  lcd.print("Ans:");
  lcd.print(correctDisplayLetter);
  lcd.print(" ");
  lcd.print(correctDisplayText);

  setRGB(0, 1, 1);
  tone(PIEZO, 200, 300);
  delay(1400);

  setRGB(1, 1, 1);
  currentQ++;

  if (lives <= 0 || currentQ >= QUESTIONS_PER_GAME) {
    showGameOver();
  } else {
    showScore();
    delay(1000);
    startQuestion();
  }
}

void showScore() {
  lcd.clear();
  lcd.print("Score:");
  lcd.print(score);
  lcd.print("/");
  lcd.print(QUESTIONS_PER_GAME);

  lcd.setCursor(0, 1);
  lcd.print("Done:");
  lcd.print(currentQ);
  lcd.print("/");
  lcd.print(QUESTIONS_PER_GAME);
}

void showGameOver() {
  lcd.clear();

  if (score == QUESTIONS_PER_GAME) {
    lcd.print("Excellent!");
  } else {
    lcd.print("Game Over!");
  }

  lcd.setCursor(0, 1);
  lcd.print("Score ");
  lcd.print(score);
  lcd.print("/");
  lcd.print(QUESTIONS_PER_GAME);

  updateTimerBar(0);
  setRGB(1, 1, 1);
  delay(3000);

  showStart();
}

void resetGame() {
  currentQ = 0;
  score = 0;
  lives = 3;
  timer = QUESTION_TIME;
  answering = false;

  randomSeed(makeSeed());
  prepareGameQuestions();

  updateTimerBar(0);
  setRGB(1, 1, 1);
}

void updateTimerBar(int timeLeft) {
  if (timeLeft > 11) {
    digitalWrite(SEG_F, HIGH);
    digitalWrite(SEG_A, HIGH);
    digitalWrite(SEG_B, HIGH);
    digitalWrite(SEG_G, HIGH);
  } else if (timeLeft > 7) {
    digitalWrite(SEG_F, HIGH);
    digitalWrite(SEG_A, HIGH);
    digitalWrite(SEG_B, HIGH);
    digitalWrite(SEG_G, LOW);
  } else if (timeLeft > 3) {
    digitalWrite(SEG_F, HIGH);
    digitalWrite(SEG_A, HIGH);
    digitalWrite(SEG_B, LOW);
    digitalWrite(SEG_G, LOW);
  } else if (timeLeft > 0) {
    digitalWrite(SEG_F, HIGH);
    digitalWrite(SEG_A, LOW);
    digitalWrite(SEG_B, LOW);
    digitalWrite(SEG_G, LOW);
  } else {
    digitalWrite(SEG_F, LOW);
    digitalWrite(SEG_A, LOW);
    digitalWrite(SEG_B, LOW);
    digitalWrite(SEG_G, LOW);
  }
}

void setRGB(int r, int g, int b) {
  if (COMMON_ANODE) {
    digitalWrite(RGB_RED,   r ? LOW : HIGH);
    digitalWrite(RGB_GREEN, g ? LOW : HIGH);
    digitalWrite(RGB_BLUE,  b ? LOW : HIGH);
  } else {
    digitalWrite(RGB_RED,   r ? HIGH : LOW);
    digitalWrite(RGB_GREEN, g ? HIGH : LOW);
    digitalWrite(RGB_BLUE,  b ? HIGH : LOW);
  }
}