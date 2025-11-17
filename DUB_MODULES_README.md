# Dub/Musique-Concrète Module Series

A collection of 4 experimental VCV Rack modules oriented toward dub production and musique-concrète composition with 2/4 groove emphasis.

## Modules

### 1. SIREN CONCRÈTE (Red - #e83939)
**Dub Siren with Musique-Concrète Granular Sampling**

A unique oscillator that combines classic dub siren character with granular musique-concrète sampling techniques.

**Parameters:**
- **Grain Morph**: Controls granular texture scatter (8 grains with jittered playback rates)
- **Spectral Shift**: Harmonic frequency shifting (16 harmonics, 0-2x shift)
- **Phase Drift**: Tape-style instability via micro LFO modulation (0.5-3.5Hz)
- **Echo Bloom**: 8-tap delay network with spectral coloration (50-250ms)

**Inputs/Outputs:**
- V/Oct input for pitch control
- CV inputs with attenuverters for all 4 parameters
- Audio output

**DSP Features:**
- 2048-sample wavetable with sawtooth + harmonic overtones
- Hann windowed grain envelopes
- Per-tap spectral coloring in delay network
- Inharmonic phase drift for organic instability

---

### 2. DUBBOÎTE 2/4 (Orange - #e8a039)
**4-Channel Dub Mixer with 2/4 Groove Orientation**

A specialized 4-channel mixer designed for dub mixing with spatial effects and groove-oriented processing.

**Parameters:**
- **Send Diffusion**: Multi-path spatial diffusion (8 paths with 10-90ms delays)
- **Tape Scrub**: LFO-modulated delay for tape-style wobble (0.3-2.3Hz, 3 harmonics)
- **Low-End Drift**: Bass-only drift via 200Hz lowpass + 0.2Hz LFO
- **Saturation Bloom**: Harmonic saturation with feedback (1-4x drive, waveshaping)

**Inputs/Outputs:**
- 4 channel inputs
- CV inputs with attenuverters for all 4 parameters
- Mix output (stereo sum of all channels)
- Send output (diffused/processed signal)

**DSP Features:**
- Per-channel 16384-sample delay buffers
- 8 independent diffusion paths with spectral coloring
- Multi-harmonic tape scrub modulation
- Tanh saturation with sine waveshaping

---

### 3. OSCILLATEUR TRITONIQUE (Green - #39e87c)
**Tritone Stabs + Wandering Bass Engine**

A waveshaping oscillator built around tritone intervals with topology morphing and temporal manipulation.

**Parameters:**
- **Topology Warp**: Morph between sine → sawtooth → folded waveforms
- **Temporal Skew**: Micro-delay modulation for timing instability (0-10ms)
- **Spectral Bloom**: Harmonic expansion with tritone inflection (12 harmonics)
- **Tritone Glide**: Portamento with tritone modulation (1-500ms glide time)

**Inputs/Outputs:**
- V/Oct input for pitch control
- CV inputs with attenuverters for all 4 parameters
- Audio output

**DSP Features:**
- 2048-sample wavetable with dynamic regeneration
- 12-harmonic spectral bloom with tritone ratio (√2)
- 4096-sample circular buffer for temporal skew
- Modulated portamento with tritone frequency shifts

---

### 4. TEMPORALISTE (Teal - #39e8d4)
**Generative Polyrhythm Sequencer with 2/4 Orientation**

A probabilistic polyrhythm generator with 4 independent gate layers and micro-timing control.

**Parameters:**
- **Polyrhythmic Density**: Controls active layers (1-4) and their probability
- **Stochastic Drift**: Probabilistic timing variation and phase offset
- **Spectral Accent**: Harmonic shaping of gate events
- **Time-Space Shift**: Per-layer micro-timing offsets (0-20ms)

**Inputs/Outputs:**
- Clock input
- CV inputs with attenuverters for all 4 parameters
- 4 gate outputs (one per rhythm layer)

**DSP Features:**
- 4 independent rhythm layers with divisions: 1 (quarters), 2 (eighths), 3 (triplets), 4 (sixteenths)
- Probability-based gate generation with drift modulation
- Spectral harmonic content added to gates
- 8192-sample per-layer buffers for micro-timing

---

## Common Architecture

All 4 modules share a consistent design:
- **Width**: 4HP
- **Knob Layout**: 2×2 grid with large knobs
- **CV Control**: Full CV control with attenuverters for all parameters
- **Color Coding**: Red → Orange → Green → Teal
- **Decorative Elements**: Matching dot patterns and accent lines

## Installation

All modules are installed to:
```
C:\Users\maxpa\AppData\Local\Rack2\plugins-win-x64\
```

Each module in its own directory:
- `SirenConcrete/`
- `DubBoite/`
- `OscillateurTritonique/`
- `Temporaliste/`

## Development

Built with:
- VCV Rack SDK 2.0
- C++11
- Windows build environment (MinGW/MSYS)

Build individual modules:
```bash
cd <module-directory>
make devinstall
```

## Usage Notes

**Siren Concrète**: Best for dub sirens, alarm sounds, granular textures. Try high grain morph with moderate spectral shift.

**DubBoîte**: Use as final mixer for dub sessions. Send Diffusion creates space, Tape Scrub adds wobble, Low-End Drift for bass movement.

**Oscillateur Tritonique**: Excellent for bass stabs and wandering basslines. High topology warp + bloom creates complex tones.

**Temporaliste**: Clock it with your main tempo. Density controls polyrhythmic complexity, Drift adds human feel, Time-Space Shift creates swing.

## Design Philosophy

These modules embrace:
- **Dub aesthetics**: Space, echo, tape effects, bass emphasis
- **Musique-concrète**: Granular synthesis, spectral manipulation, tape techniques
- **2/4 groove orientation**: Rhythm structures suited to dub/reggae
- **Experimental processing**: Unconventional DSP for creative exploration

---

*Built by Max Paterson | RackDev 2024*
