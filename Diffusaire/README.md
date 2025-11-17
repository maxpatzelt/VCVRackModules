# DIFFUSAIRE

## Overview
DIFFUSAIRE is a 4HP spectral motion filter for VCV Rack with cascaded filtering, variable resonance modes, phase dispersion, and cinematic modulation effects.

## Features

### Contours
Multi-pole cascaded lowpass filter with morphing cutoff distribution. Each pole operates at a different frequency creating evolving frequency boundaries. Smooth crossfade prevents instability at high frequencies.

### Résonance Variable
Three distinct resonance modes:
- **Soft Analog** (0.0-0.33): Gentle feedback for warm peaks
- **Metallic** (0.33-0.66): Ringing oscillation with bell-like character
- **Fractalized** (0.66-1.0): Granular resonance with aggressive feedback

### Écart (Phase Dispersion)
6-stage all-pass filter network creating stereo-like spatial effects through phase manipulation. Enhanced coefficients for pronounced audibility.

### Cinétiques (Motion)
Tape-style modulation combining:
- Wow: 0.3Hz slow drift
- Flutter: 2.7Hz fast vibrato
- Dual-LFO modulated delay line

## Technical Details
- Cyan color scheme (#39c5e8)
- 4HP width with 2x2 knob grid
- CV inputs with attenuverters
- 4-pole state-variable filters
- 8192-sample delay buffer
- Adaptive damping for stability

## Building
```bash
make devinstall
```

## Usage
Use as a resonant lowpass filter with the Contours knob. Add Écart for width and space. Cinétiques creates organic motion. Experiment with Résonance Variable at different ranges for different filter characters.

## License
MIT License
