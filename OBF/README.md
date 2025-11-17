# OBF (Oscillateur à Basse Fréquence)

## Overview
OBF is a French-themed 2HP LFO module for VCV Rack with chaos modulation and gate synchronization.

## Features
- **Fréq**: Frequency control (0.1Hz - 10Hz)
- **Chaos**: Adds random jitter to the oscillation
- **Entrée**: Gate input for reset/sync
- **Porte**: Gate output (square wave)
- **Sortie**: Sine wave LFO output

## Technical Details
- Purple color scheme (#9b59d0) with decorative accents
- 2HP width module
- Gate threshold: 1V
- Chaos uses XOR-shift PRNG for phase modulation

## Building
```bash
make devinstall
```

## Usage
Connect the sine output to modulation destinations. Use the chaos knob to add organic variation. Gate input resets the phase when triggered, and gate output provides a square wave sync signal.

## License
MIT License