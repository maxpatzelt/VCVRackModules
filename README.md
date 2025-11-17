# VCV Rack Modules Collection

A collection of experimental French-themed VCV Rack modules with unique DSP processing.

## Modules

### OBF (Oscillateur à Basse Fréquence)
**2HP Purple-themed LFO**
- Low-frequency oscillator with chaos parameter
- Gate input and output for synchronization
- Sine wave output
- Color scheme: Purple (#9b59d0)

### SONOGENESE
**4HP Gold-themed Experimental VCO**
- Granular fragmentation with jittered grain synthesis
- Chebyshev polynomial topology warping (orders 2-9)
- Temporal skewing with nonlinear phase modulation
- Spectral bloom with up to 16 harmonics (9x spread)
- Color scheme: Gold (#e8b339)

### DIFFUSAIRE
**4HP Cyan-themed Spectral Filter**
- Multi-pole cascaded filter with morphing contours
- Variable resonance modes (soft analog → metallic → fractalized)
- All-pass phase dispersion network (Écart)
- Cinematic wow and flutter modulation
- Color scheme: Cyan (#39c5e8)

## Building

Each module can be built independently:

```bash
cd OBF && make devinstall
cd Sonogenese && make devinstall
cd Diffusaire && make devinstall
```

## Requirements

- VCV Rack SDK 2.x
- C++11 compatible compiler
- Make build system

## License

MIT License - See individual LICENSE files in each module directory.
