#include "SirenConcreteModule.hpp"
#include <cmath>

using namespace rack;

SirenConcreteModule::SirenConcreteModule() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	
	// Main knobs
	configParam(GRAIN_MORPH_PARAM, 0.0f, 1.0f, 0.0f, "Grain Morph", "%", 0.f, 100.f);
	configParam(SPECTRAL_SHIFT_PARAM, 0.0f, 1.0f, 0.0f, "Spectral Shift", "%", 0.f, 100.f);
	configParam(PHASE_DRIFT_PARAM, 0.0f, 1.0f, 0.0f, "Phase Drift", "%", 0.f, 100.f);
	configParam(ECHO_BLOOM_PARAM, 0.0f, 1.0f, 0.0f, "Echo Bloom", "%", 0.f, 100.f);
	
	// Attenuverters
	configParam(GRAIN_MORPH_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Grain Morph CV", "%", 0.f, 100.f);
	configParam(SPECTRAL_SHIFT_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Spectral Shift CV", "%", 0.f, 100.f);
	configParam(PHASE_DRIFT_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Phase Drift CV", "%", 0.f, 100.f);
	configParam(ECHO_BLOOM_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Echo Bloom CV", "%", 0.f, 100.f);
	
	// Inputs
	configInput(PITCH_INPUT, "V/Oct");
	configInput(GRAIN_MORPH_CV_INPUT, "Grain Morph CV");
	configInput(SPECTRAL_SHIFT_CV_INPUT, "Spectral Shift CV");
	configInput(PHASE_DRIFT_CV_INPUT, "Phase Drift CV");
	configInput(ECHO_BLOOM_CV_INPUT, "Echo Bloom CV");
	
	// Output
	configOutput(AUDIO_OUTPUT, "Audio");
	
	// Initialize wavetable with dub siren waveform (sawtooth + overtones)
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		float phase = (float)i / WAVETABLE_SIZE;
		wavetable[i] = 2.0f * phase - 1.0f; // Sawtooth base
		wavetable[i] += 0.3f * std::sin(2.0f * M_PI * phase * 2.0f); // 2nd harmonic
		wavetable[i] += 0.2f * std::sin(2.0f * M_PI * phase * 3.0f); // 3rd harmonic
	}
	
	// Initialize state
	basePhase = 0.0f;
	baseFreq = 220.0f;
	driftPhase = 0.0f;
	driftAmount = 0.0f;
	delayWritePos = 0;
	activeGrains = MAX_GRAINS;
	rng = 12345;
	
	for (int i = 0; i < MAX_GRAINS; i++) {
		grainPhases[i] = (float)i / MAX_GRAINS;
		grainAmps[i] = 0.0f;
		grainRates[i] = 1.0f;
	}
	
	for (int i = 0; i < 16; i++) {
		harmonicPhases[i] = 0.0f;
		harmonicAmps[i] = 1.0f / (float)(i + 1);
	}
	
	for (int i = 0; i < DELAY_SIZE; i++) {
		delayBuffer[i] = 0.0f;
	}
}

void SirenConcreteModule::process(const ProcessArgs& args) {
	// Get CV-modulated parameters
	float grainMorph = params[GRAIN_MORPH_PARAM].getValue();
	if (inputs[GRAIN_MORPH_CV_INPUT].isConnected()) {
		grainMorph += inputs[GRAIN_MORPH_CV_INPUT].getVoltage() * params[GRAIN_MORPH_ATTEN_PARAM].getValue() * 0.1f;
	}
	grainMorph = clamp(grainMorph, 0.0f, 1.0f);
	
	float spectralShift = params[SPECTRAL_SHIFT_PARAM].getValue();
	if (inputs[SPECTRAL_SHIFT_CV_INPUT].isConnected()) {
		spectralShift += inputs[SPECTRAL_SHIFT_CV_INPUT].getVoltage() * params[SPECTRAL_SHIFT_ATTEN_PARAM].getValue() * 0.1f;
	}
	spectralShift = clamp(spectralShift, 0.0f, 1.0f);
	
	float phaseDrift = params[PHASE_DRIFT_PARAM].getValue();
	if (inputs[PHASE_DRIFT_CV_INPUT].isConnected()) {
		phaseDrift += inputs[PHASE_DRIFT_CV_INPUT].getVoltage() * params[PHASE_DRIFT_ATTEN_PARAM].getValue() * 0.1f;
	}
	phaseDrift = clamp(phaseDrift, 0.0f, 1.0f);
	
	float echoBloom = params[ECHO_BLOOM_PARAM].getValue();
	if (inputs[ECHO_BLOOM_CV_INPUT].isConnected()) {
		echoBloom += inputs[ECHO_BLOOM_CV_INPUT].getVoltage() * params[ECHO_BLOOM_ATTEN_PARAM].getValue() * 0.1f;
	}
	echoBloom = clamp(echoBloom, 0.0f, 1.0f);
	
	// Get pitch from V/Oct input
	float pitch = 0.0f;
	if (inputs[PITCH_INPUT].isConnected()) {
		pitch = inputs[PITCH_INPUT].getVoltage();
	}
	baseFreq = 261.626f * std::pow(2.0f, pitch); // C4 = 261.626 Hz
	
	// Base oscillator
	float deltaPhase = baseFreq / args.sampleRate;
	basePhase += deltaPhase;
	if (basePhase >= 1.0f) basePhase -= 1.0f;
	
	int tableIndex = (int)(basePhase * WAVETABLE_SIZE) % WAVETABLE_SIZE;
	float output = wavetable[tableIndex];
	
	// Stage 1: Grain Morph
	output = processGrainMorph(output, grainMorph, args.sampleRate);
	
	// Stage 2: Spectral Shift
	output = processSpectralShift(output, spectralShift, baseFreq);
	
	// Stage 3: Phase Drift
	output = processPhaseDrift(output, phaseDrift, args.sampleRate);
	
	// Stage 4: Echo Bloom
	output = processEchoBloom(output, echoBloom, args.sampleRate);
	
	outputs[AUDIO_OUTPUT].setVoltage(clamp(output * 5.0f, -10.0f, 10.0f));
}

// ================================================================
// DSP HELPER FUNCTIONS
// ================================================================

float SirenConcreteModule::processGrainMorph(float input, float morph, float sampleRate) {
	if (morph < 0.01f) return input;
	
	// Granular resynthesis: scatter playback across grains
	float output = input * (1.0f - morph);
	
	for (int i = 0; i < activeGrains; i++) {
		// Update grain phases with jittered rates
		float rateVariation = 1.0f + (random() - 0.5f) * morph * 0.3f;
		grainPhases[i] += baseFreq / sampleRate * rateVariation;
		if (grainPhases[i] >= 1.0f) {
			grainPhases[i] -= 1.0f;
			// Regenerate grain on wrap
			grainAmps[i] = 0.5f + random() * 0.5f;
			grainRates[i] = rateVariation;
		}
		
		// Grain envelope (Hann window)
		float envelope = 0.5f * (1.0f - std::cos(2.0f * M_PI * grainPhases[i]));
		
		// Sample from wavetable
		int idx = (int)(grainPhases[i] * WAVETABLE_SIZE) % WAVETABLE_SIZE;
		float grainSample = wavetable[idx] * envelope * grainAmps[i];
		
		output += grainSample * morph / (float)activeGrains;
	}
	
	return output;
}

float SirenConcreteModule::processSpectralShift(float input, float shift, float freq) {
	if (shift < 0.01f) return input;
	
	// Spectral shifting: modulate harmonics with shifted frequencies
	float output = input * (1.0f - shift * 0.7f);
	
	for (int i = 0; i < 16; i++) {
		// Shift harmonic frequencies by shift amount
		float shiftedHarmonic = (float)(i + 1) * (1.0f + shift * 2.0f);
		harmonicPhases[i] += shiftedHarmonic * freq / 48000.0f;
		if (harmonicPhases[i] >= 1.0f) harmonicPhases[i] -= 1.0f;
		
		// Generate harmonic
		float harmonic = std::sin(2.0f * M_PI * harmonicPhases[i]);
		harmonic *= harmonicAmps[i] * shift * 0.5f;
		
		output += harmonic;
	}
	
	return output;
}

float SirenConcreteModule::processPhaseDrift(float input, float drift, float sampleRate) {
	if (drift < 0.01f) return input;
	
	// Micro LFO for tape-style instability
	float lfoFreq = 0.5f + drift * 3.0f; // 0.5 - 3.5 Hz
	driftPhase += lfoFreq / sampleRate;
	if (driftPhase >= 1.0f) driftPhase -= 1.0f;
	
	// Create drift modulation
	float lfo = std::sin(2.0f * M_PI * driftPhase);
	lfo += 0.3f * std::sin(2.0f * M_PI * driftPhase * 2.3f); // Add inharmonic component
	
	driftAmount = lfo * drift * 0.05f;
	
	// Apply as phase modulation
	float driftedPhase = basePhase + driftAmount;
	if (driftedPhase >= 1.0f) driftedPhase -= 1.0f;
	if (driftedPhase < 0.0f) driftedPhase += 1.0f;
	
	int idx = (int)(driftedPhase * WAVETABLE_SIZE) % WAVETABLE_SIZE;
	float driftedSample = wavetable[idx];
	
	return input * (1.0f - drift * 0.5f) + driftedSample * drift * 0.5f;
}

float SirenConcreteModule::processEchoBloom(float input, float bloom, float sampleRate) {
	if (bloom < 0.01f) return input;
	
	// Write to delay buffer
	delayBuffer[delayWritePos] = input;
	delayWritePos = (delayWritePos + 1) % DELAY_SIZE;
	
	// Read multiple taps with spectral spreading
	float output = input;
	int numTaps = 8;
	float delayTime = 0.05f + bloom * 0.2f; // 50-250ms base
	
	for (int i = 0; i < numTaps; i++) {
		float tapDelay = delayTime * (1.0f + (float)i * 0.15f);
		int tapSamples = (int)(tapDelay * sampleRate);
		int readPos = (delayWritePos - tapSamples + DELAY_SIZE) % DELAY_SIZE;
		
		float tapSample = delayBuffer[readPos];
		float tapAmp = bloom * 0.6f / (float)(i + 1);
		
		// Apply spectral coloration per tap
		float coloredSample = tapSample;
		if (i % 2 == 0) {
			coloredSample *= 1.0f - 0.3f * bloom; // Darken even taps
		} else {
			coloredSample *= 1.0f + 0.2f * bloom; // Brighten odd taps
		}
		
		output += coloredSample * tapAmp;
	}
	
	return output * 0.6f; // Normalize to prevent clipping
}
