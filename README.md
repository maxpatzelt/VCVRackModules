# VCV Rack Modules Collection

A collection of experimental VCV Rack modules featuring French-themed experimental synthesis and dub/musique-concrète oriented processors.

## Module Collections

### French Experimental Series

#### OBF (Oscillateur à Basse Fréquence)
**2HP Purple-themed LFO**
- Low-frequency oscillator with chaos parameter
- Gate input and output for synchronization
- Sine wave output
- Color scheme: Purple (#9b59d0)

#### SONOGENESE
**4HP Gold-themed Experimental VCO**
- Granular fragmentation with jittered grain synthesis
- Chebyshev polynomial topology warping (orders 2-9)
- Temporal skewing with nonlinear phase modulation
- Spectral bloom with up to 16 harmonics (9x spread)
- Color scheme: Gold (#e8b339)

#### DIFFUSAIRE
**4HP Cyan-themed Spectral Filter**
- Multi-pole cascaded filter with morphing contours
- Variable resonance modes (soft analog → metallic → fractalized)
- All-pass phase dispersion network (Écart)
- Cinematic wow and flutter modulation
- Color scheme: Cyan (#39c5e8)

---

### Dub/Musique-Concrète Series

#### SIREN CONCRÈTE
**4HP Red-themed Dub Siren with Musique-Concrète Granular Sampling**
- **Grain Morph**: Granular texture scatter (8 grains with jittered playback)
- **Spectral Shift**: Harmonic frequency shifting (16 harmonics, 0-2x)
- **Phase Drift**: Tape-style instability via micro LFO (0.5-3.5Hz)
- **Echo Bloom**: 8-tap delay network with spectral coloration (50-250ms)
- V/Oct input, audio output
- Color scheme: Red (#e83939)

#### DUBBOÎTE 2/4
**4HP Orange-themed 4-Channel Dub Mixer with 2/4 Groove Orientation**
- **Send Diffusion**: Multi-path spatial diffusion (8 paths, 10-90ms)
- **Tape Scrub**: LFO-modulated delay for tape wobble (0.3-2.3Hz)
- **Low-End Drift**: Bass-only drift via 200Hz lowpass + LFO
- **Saturation Bloom**: Harmonic saturation with feedback (1-4x drive)
- 4 channel inputs, mix output, send output
- Color scheme: Orange (#e8a039)

#### OSCILLATEUR TRITONIQUE
**4HP Green-themed Tritone Stabs + Wandering Bass Engine**
- **Topology Warp**: Morph sine → sawtooth → folded waveforms
- **Temporal Skew**: Micro-delay modulation for timing instability (0-10ms)
- **Spectral Bloom**: Harmonic expansion with tritone inflection (12 harmonics)
- **Tritone Glide**: Portamento with tritone modulation (1-500ms)
- V/Oct input, audio output
- Color scheme: Green (#39e87c)

#### TEMPORALISTE
**4HP Teal-themed Generative Polyrhythm Sequencer with 2/4 Orientation**
- **Polyrhythmic Density**: Controls active layers (1-4) and probability
- **Stochastic Drift**: Probabilistic timing variation and phase offset
- **Spectral Accent**: Harmonic shaping of gate events
- **Time-Space Shift**: Per-layer micro-timing offsets (0-20ms)
- Clock input, 4 gate outputs
- Color scheme: Teal (#39e8d4)

---

## Common Architecture

All dub/musique-concrète modules share:
- **Width**: 4HP
- **Knob Layout**: 2×2 grid with large knobs
- **CV Control**: Full CV control with attenuverters for all parameters
- **Color Coding**: Red → Orange → Green → Teal
- **Decorative Elements**: Matching dot patterns and accent lines

## Building

Each module can be built independently:

```bash
# French Experimental Series
cd OBF && make devinstall
cd Sonogenese && make devinstall
cd Diffusaire && make devinstall

# Dub/Musique-Concrète Series
cd SirenConcrete && make devinstall
cd DubBoite && make devinstall
cd OscillateurTritonique && make devinstall
cd Temporaliste && make devinstall
```

## Requirements

- VCV Rack SDK 2.x
- C++11 compatible compiler
- Make build system

## Design Philosophy

**French Experimental Series**: Granular synthesis, spectral manipulation, and experimental DSP with French nomenclature.

**Dub/Musique-Concrète Series**: Space, echo, tape effects, bass emphasis, granular synthesis, spectral manipulation, and 2/4 groove orientation for dub production and musique-concrète composition.

## License

MIT License - See individual LICENSE files in each module directory.
