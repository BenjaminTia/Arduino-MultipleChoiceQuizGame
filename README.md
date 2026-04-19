# Arduino MCU Quiz Game

An educational microcontroller quiz game that tests players on Arduino and embedded systems knowledge. Features timed questions, multiple choice answers, visual feedback via LCD and 7-segment display, and audio feedback via piezo buzzer.

## Hardware Components

| Component | Quantity | Notes |
|-----------|----------|-------|
| Arduino UNO | 1 | Main microcontroller |
| LCD 1602A (16x2) | 1 | Display |
| 7-Segment Display | 1 | Single digit, timer |
| Push Buttons | 4 | A, B, C, D |
| RGB LED | 1 | Common cathode |
| Piezo Buzzer | 1 | Audio feedback |
| 10k Potentiometer | 1 | LCD contrast |
| 220Ω Resistors | 7 | For LED and 7-seg |
| Breadboard + wires | 1 | Assembly |

## Pin Connections

| Component | Arduino Pin |
|-----------|------------|
| LCD RS | 2 |
| LCD E | 3 |
| LCD D4-D7 | 4-7 |
| Button A | 8 |
| Button B | 9 |
| Button C | 10 |
| Button D | 11 |
| Piezo | 12 |
| RGB Red | 13 |
| RGB Green | A0 |
| RGB Blue | A1 |
| 7-seg F | A2 |
| 7-seg A | A3 |
| 7-seg B | A4 |
| 7-seg G | A5 |

See PROJECT_DOCUMENTATION.txt for detailed wiring.

## Features

- 33 engineering questions
- Multiple choice (A/B/C/D)
- 9-second timer per question
- Visual timer on 7-segment (progress circle)
- Score tracking
- 3 lives system
- RGB LED feedback (green=correct, red=wrong)
- Piezo audio feedback
- Question scrolling for long text
- Choice page switching (A+B ↔ C+D)

## How to Play

1. Press any button to start
2. Read question on LCD (row 1)
3. View choices (row 2) - press B to switch pages (A+B ↔ C+D)
4. Press A to select and submit answer
5. Green LED + happy beep = Correct
6. Red LED + sad beep = Wrong
7. Game ends when lives = 0 or all questions answered

## Controls

- **Button A** = Select/Submit answer
- **Button B** = Switch choice page
- **Any button** = Start game / Restart

## Files

| File | Description |
|------|-------------|
| FINAL_quiz_game.ino | Main Arduino code |
| questions.txt | Question bank (editable) |
| generate_quiz.py | Regenerate code from questions |
| PROJECT_DOCUMENTATION.txt | Detailed documentation |
| README.md | This file |

## Customizing Questions

Edit `questions.txt` in this format:
```
Question text? | Option A | Option B | Option C | Option D | CorrectAnswer
```

Then run:
```bash
python generate_quiz.py
```

## Requirements

- Arduino IDE
- LiquidCrystal library
- Arduino UNO or compatible

## License

MIT