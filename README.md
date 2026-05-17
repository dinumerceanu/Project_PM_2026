# Two-Player Reaction Time Game
**PM Project 2026 — ATmega328P XMINI**
Author: Dinu Merceanu

## Description
A two-player reaction time game inspired by the Romanian TV show "Ce spun romanii" and the
Formula 1 lights start.
Three red LEDs light up sequentially as a countdown, then turn 
off simultaneously after a random delay. The first player to press their button 
wins. Results are displayed on an OLED screen.

## Hardware
| Component | Quantity |
|-----------|----------|
| ATmega328P XMINI | 1 |
| OLED display 0.96" SSD1306 I2C | 1 |
| LED red 5mm | 3 |
| LED blue 5mm | 2 |
| Resistor 220Ω | 5 |
| Push button 12x12mm | 2 |
| Breadboard 830 points | 1 |
| Jumper wires | 19 |

## Pin Connections
| Pin | Component |
|-----|-----------|
| PB0, PB1, PB2 | Red LEDs (countdown) |
| PB3, PB4 | Blue LEDs (winner P1/P2) |
| PB7 | Start button (on-board SW0) |
| PD2 (INT0) | Player 1 button |
| PD3 (INT1) | Player 2 button |
| PC4 (SDA) | OLED SDA |
| PC5 (SCL) | OLED SCL |

## Project Structure
src/
    main.cpp       — game logic and state machine
    uptime.cpp     — Timer2 CTC, 1ms system tick
    timers.cpp     — Timer1 CTC, reaction time measurement
    twi.cpp        — I2C/TWI driver
    ssd1306.cpp    — SSD1306 OLED driver with 5x7 font
    usart.cpp      — UART debug driver
include/
    uptime.h
    timers.h
    twi.h
    ssd1306.h
    usart.h
platformio.ini

## Build and Upload
Requirements: PlatformIO, VS Code, avrdude

```bash
# Build
pio run

# Upload (ATmega328P XMINI connected via USB)
pio run --target upload

# Serial monitor
pio device monitor
```

## Laboratories Used
- **Lab 2 — Interrupts**: Timer2 ISR for system tick, INT0/INT1 for false start detection
- **Lab 3 — Timers**: Timer1 CTC for millisecond reaction time measurement
- **Lab 6 — I2C**: TWI hardware module for SSD1306 OLED communication

## How to Play
1. Connect the board via USB
2. Press the on-board button (SW0) to start a round
3. Wait for the three red LEDs to light up one by one
4. React as fast as possible when all LEDs turn off
5. The first player to press their button wins
6. Pressing before the LEDs turn off results in disqualification

## License
Academic project — Faculty of Automatic Control and Computer Science, 2026