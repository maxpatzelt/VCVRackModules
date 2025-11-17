#include "DiffusaireModule.hpp"
#include <cmath>

DiffusaireModule::DiffusaireModule() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	
	configParam(CONTOURS_PARAM, 0.0f, 1.0f, 0.5f, "Contours", " Hz", 0.f, 20000.f, 20.f);
	configParam(RESONANCE_PARAM, 0.0f, 1.0f, 0.0f, "Résonance Variable", "%", 0.f, 100.f);
	configParam(ECART_PARAM, 0.0f, 1.0f, 0.0f, "Écart (Dispersion)", "%", 0.f, 100.f);
	configParam(CINETIQUES_PARAM, 0.0f, 1.0f, 0.0f, "Cinétiques (Motion)", "%", 0.f, 100.f);
	
	configParam(CONTOURS_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Contours CV", "%", 0.f, 100.f);
	configParam(RESONANCE_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Résonance CV", "%", 0.f, 100.f);
	configParam(ECART_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Écart CV", "%", 0.f, 100.f);
	configParam(CINETIQUES_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Cinétiques CV", "%", 0.f, 100.f);
	
	configInput(AUDIO_INPUT, "Audio");
	configInput(CONTOURS_INPUT, "Contours CV");
	configInput(RESONANCE_INPUT, "Résonance CV");
	configInput(ECART_INPUT, "Écart CV");
	configInput(CINETIQUES_INPUT, "Cinétiques CV");
	configOutput(AUDIO_OUTPUT, "Audio");
}

void DiffusaireModule::process(const ProcessArgs& args) {
	if (!inputs[AUDIO_INPUT].isConnected() || !outputs[AUDIO_OUTPUT].isConnected()) {
		outputs[AUDIO_OUTPUT].setVoltage(0.f);
		return;
	}
	
	float input = inputs[AUDIO_INPUT].getVoltage();
	
	// Get parameters with CV and attenuverters
	float contours = params[CONTOURS_PARAM].getValue();
	if (inputs[CONTOURS_INPUT].isConnected()) {
		float cv = inputs[CONTOURS_INPUT].getVoltage() / 10.0f;
		float atten = params[CONTOURS_ATTEN_PARAM].getValue();
		contours = clamp(contours + cv * atten, 0.0f, 1.0f);
	}
	
	float resonance = params[RESONANCE_PARAM].getValue();
	if (inputs[RESONANCE_INPUT].isConnected()) {
		float cv = inputs[RESONANCE_INPUT].getVoltage() / 10.0f;
		float atten = params[RESONANCE_ATTEN_PARAM].getValue();
		resonance = clamp(resonance + cv * atten, 0.0f, 1.0f);
	}
	
	float ecart = params[ECART_PARAM].getValue();
	if (inputs[ECART_INPUT].isConnected()) {
		float cv = inputs[ECART_INPUT].getVoltage() / 10.0f;
		float atten = params[ECART_ATTEN_PARAM].getValue();
		ecart = clamp(ecart + cv * atten, 0.0f, 1.0f);
	}
	
	float cinetiques = params[CINETIQUES_PARAM].getValue();
	if (inputs[CINETIQUES_INPUT].isConnected()) {
		float cv = inputs[CINETIQUES_INPUT].getVoltage() / 10.0f;
		float atten = params[CINETIQUES_ATTEN_PARAM].getValue();
		cinetiques = clamp(cinetiques + cv * atten, 0.0f, 1.0f);
	}
	
	// ================================================================
	// SIGNAL PROCESSING CHAIN
	// ================================================================
	
	float output = input;
	
	// Stage 1: Cinétiques (micro-motion modulation)
	output = processCinetiques(output, cinetiques, args.sampleRate);
	
	// Stage 2: Contours (morphing multi-band cutoff)
	float cutoffFreq = 20.0f * std::pow(1000.0f, contours);
	output = processContours(output, contours, args.sampleRate);
	
	// Stage 3: Résonance Variable (character-changing resonance)
	output = processResonanceVariable(output, resonance, cutoffFreq, args.sampleRate);
	
	// Stage 4: Écart (phase dispersion)
	output = processEcart(output, ecart);
	
	outputs[AUDIO_OUTPUT].setVoltage(clamp(output, -10.0f, 10.0f));
}

// ================================================================
// DSP HELPER FUNCTIONS
// ================================================================

float DiffusaireModule::processContours(float input, float contours, float sampleRate) {
	// Morphing multi-band cutoff with cascaded filters
	// Poles move in relation to create shifting boundaries
	
	float baseFreq = 20.0f * std::pow(1000.0f, contours);
	
	// Smooth crossfade to bypass at very high frequencies
	float maxSafeFreq = sampleRate * 0.33f;
	float bypassBlend = 0.0f;
	if (baseFreq > maxSafeFreq * 0.85f) {
		bypassBlend = (baseFreq - maxSafeFreq * 0.85f) / (maxSafeFreq * 0.15f);
		bypassBlend = clamp(bypassBlend, 0.0f, 1.0f);
	}
	
	float output = input;
	
	// Process through cascaded poles with spread positions
	for (int i = 0; i < NUM_POLES; i++) {
		// Each pole has a different offset frequency
		float poleSpread = 1.0f + (float)i * 0.15f * contours;
		float poleFreq = baseFreq * poleSpread;
		
		// Clamp to stable range with smoother limiting
		poleFreq = clamp(poleFreq, 20.0f, sampleRate * 0.38f);
		float freq = clamp(poleFreq / sampleRate, 0.0001f, 0.38f);
		
		// Smoother coefficient calculation
		float f = 2.0f * std::sin(M_PI * freq);
		f = clamp(f, 0.0001f, 1.5f);
		
		// Adaptive damping - more damping at high frequencies
		float q = 0.7f + (1.0f - freq) * 0.2f;
		
		// State-variable filter
		lowpass[i] += f * bandpass[i];
		highpass[i] = output - lowpass[i] - q * bandpass[i];
		bandpass[i] += f * highpass[i];
		
		// Clamp states to prevent runaway
		lowpass[i] = clamp(lowpass[i], -10.0f, 10.0f);
		bandpass[i] = clamp(bandpass[i], -10.0f, 10.0f);
		highpass[i] = clamp(highpass[i], -10.0f, 10.0f);
		
		output = lowpass[i];
	}
	
	// Smooth crossfade to dry signal at extreme high frequencies
	return output * (1.0f - bypassBlend) + input * bypassBlend;
}

float DiffusaireModule::processResonanceVariable(float input, float resonance, float freq, float sampleRate) {
	// Character-changing resonance: soft → metallic → fractalized
	
	if (resonance < 0.01f) return input;
	
	float output = input;
	
	if (resonance < 0.33f) {
		// Soft analog peak (0.0 - 0.33)
		float resAmount = resonance / 0.33f;
		float feedback = resonanceMemory * resAmount * 0.8f;
		output = input + feedback;
		resonanceMemory = output * 0.99f;
	}
	else if (resonance < 0.66f) {
		// Metallic ping (0.33 - 0.66)
		float resAmount = (resonance - 0.33f) / 0.33f;
		// Ringing oscillation at cutoff frequency
		float ringFreq = clamp(freq / sampleRate, 0.001f, 0.45f);
		float ring = std::sin(2.0f * M_PI * lfoPhase * ringFreq * 100.0f);
		output = input + ring * resAmount * 0.6f;
		
		// Feedback with metallic character
		resonanceMemory = (output + resonanceMemory * 0.95f) * 0.5f;
		output += resonanceMemory * resAmount;
	}
	else {
		// Fractalized/granular edge (0.66 - 1.0)
		float resAmount = (resonance - 0.66f) / 0.34f;
		
		// Create granular resonance with waveshaping
		float shaped = std::tanh(input * (1.0f + resAmount * 3.0f));
		float fractal = shaped + std::sin(shaped * 20.0f * M_PI) * resAmount * 0.4f;
		
		// Add aggressive feedback
		resonanceMemory = fractal * 0.98f + resonanceMemory * resAmount * 0.7f;
		output = input * (1.0f - resAmount) + (fractal + resonanceMemory) * resAmount;
	}
	
	return clamp(output, -10.0f, 10.0f);
}

float DiffusaireModule::processEcart(float input, float ecart) {
	// Spatial/phase dispersion using all-pass networks
	
	if (ecart < 0.01f) return input;
	
	float output = input;
	
	// Cascade of all-pass filters with stronger coefficients for audible effect
	for (int i = 0; i < NUM_ALLPASS; i++) {
		float coeff = 0.5f + (float)i * 0.15f * ecart;
		coeff = clamp(coeff, 0.0f, 0.95f);
		output = allpassFilter(output, coeff, i);
	}
	
	// More aggressive blend for pronounced spatial effect
	return input * (1.0f - ecart * 0.85f) + output * (ecart * 0.85f);
}

float DiffusaireModule::processCinetiques(float input, float cinetiques, float sampleRate) {
	// Micro-motion: wow, flutter, drift via modulated delay
	
	if (cinetiques < 0.01f) return input;
	
	// Write to delay line
	delayLine[delayWritePos] = input;
	delayWritePos = (delayWritePos + 1) % DELAY_SIZE;
	
	// Generate organic LFO modulation (multiple rates)
	lfoPhase += 0.3f / sampleRate;  // Slow drift
	lfoPhase2 += 2.7f / sampleRate; // Flutter
	if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
	if (lfoPhase2 >= 1.0f) lfoPhase2 -= 1.0f;
	
	// Combine LFO rates for tape-like character
	float wow = std::sin(2.0f * M_PI * lfoPhase) * cinetiques * 30.0f;
	float flutter = std::sin(2.0f * M_PI * lfoPhase2) * cinetiques * 8.0f;
	float modulation = wow + flutter;
	
	// Variable delay time
	float delayTime = 50.0f + modulation;
	delayTime = clamp(delayTime, 1.0f, (float)(DELAY_SIZE - 2));
	
	// Read from delay with interpolation
	int readPos = (delayWritePos - (int)delayTime + DELAY_SIZE) % DELAY_SIZE;
	float frac = delayTime - (int)delayTime;
	int nextReadPos = (readPos - 1 + DELAY_SIZE) % DELAY_SIZE;
	
	float delayed = delayLine[readPos] * (1.0f - frac) + delayLine[nextReadPos] * frac;
	
	// Blend with dry signal
	return input * (1.0f - cinetiques * 0.4f) + delayed * (cinetiques * 0.4f);
}

float DiffusaireModule::allpassFilter(float input, float coeff, int stage) {
	// First-order all-pass filter
	// H(z) = (coeff + z^-1) / (1 + coeff * z^-1)
	
	float output = -coeff * input + allpassState[stage][0];
	allpassState[stage][0] = input + coeff * output;
	
	return output;
}
