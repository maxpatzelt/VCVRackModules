#include "SonogeneseModule.hpp"
#include <cmath>

SonogeneseModule::SonogeneseModule() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	
	configParam(FRAGMENTATION_PARAM, 0.0f, 1.0f, 0.0f, "Fragmentation", "%", 0.f, 100.f);
	configParam(TOPOLOGY_PARAM, 0.0f, 1.0f, 0.0f, "Topology Warp", "%", 0.f, 100.f);
	configParam(SKEW_PARAM, 0.0f, 1.0f, 0.5f, "Temporal Skew", "%", 0.f, 100.f);
	configParam(BLOOM_PARAM, 0.0f, 1.0f, 0.5f, "Spectral Bloom", "%", 0.f, 100.f);
	
	configParam(FRAGMENTATION_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Fragmentation CV", "%", 0.f, 100.f);
	configParam(TOPOLOGY_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Topology CV", "%", 0.f, 100.f);
	configParam(SKEW_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Skew CV", "%", 0.f, 100.f);
	configParam(BLOOM_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Bloom CV", "%", 0.f, 100.f);
	
	configInput(VOCT_INPUT, "V/Oct");
	configInput(FRAGMENTATION_INPUT, "Fragmentation CV");
	configInput(TOPOLOGY_INPUT, "Topology CV");
	configInput(SKEW_INPUT, "Skew CV");
	configInput(BLOOM_INPUT, "Bloom CV");
	configOutput(AUDIO_OUTPUT, "Audio");
	
	// Initialize wavetable
	generateWavetable(0.0f);
	
	// Initialize harmonic amplitudes (natural harmonic series)
	for (int i = 0; i < MAX_HARMONICS; i++) {
		harmonicAmps[i] = 1.0f / (i + 1.0f);
	}
}

void SonogeneseModule::process(const ProcessArgs& args) {
	if (!outputs[AUDIO_OUTPUT].isConnected()) return;
	
	// Get V/Oct input and calculate frequency
	float pitch = 0.0f;
	if (inputs[VOCT_INPUT].isConnected()) {
		pitch = inputs[VOCT_INPUT].getVoltage();
	}
	frequency = dsp::FREQ_C4 * std::pow(2.0f, pitch);
	
	// Get parameters with CV and attenuverters
	float fragmentation = params[FRAGMENTATION_PARAM].getValue();
	if (inputs[FRAGMENTATION_INPUT].isConnected()) {
		float cv = inputs[FRAGMENTATION_INPUT].getVoltage() / 10.0f;
		float atten = params[FRAGMENTATION_ATTEN_PARAM].getValue();
		fragmentation = clamp(fragmentation + cv * atten, 0.0f, 1.0f);
	}
	
	float topology = params[TOPOLOGY_PARAM].getValue();
	if (inputs[TOPOLOGY_INPUT].isConnected()) {
		float cv = inputs[TOPOLOGY_INPUT].getVoltage() / 10.0f;
		float atten = params[TOPOLOGY_ATTEN_PARAM].getValue();
		topology = clamp(topology + cv * atten, 0.0f, 1.0f);
	}
	
	float skew = params[SKEW_PARAM].getValue();
	if (inputs[SKEW_INPUT].isConnected()) {
		float cv = inputs[SKEW_INPUT].getVoltage() / 10.0f;
		float atten = params[SKEW_ATTEN_PARAM].getValue();
		skew = clamp(skew + cv * atten, 0.0f, 1.0f);
	}
	
	float bloom = params[BLOOM_PARAM].getValue();
	if (inputs[BLOOM_INPUT].isConnected()) {
		float cv = inputs[BLOOM_INPUT].getVoltage() / 10.0f;
		float atten = params[BLOOM_ATTEN_PARAM].getValue();
		bloom = clamp(bloom + cv * atten, 0.0f, 1.0f);
	}
	
	// Update wavetable if topology changed significantly
	static float lastTopology = 0.0f;
	if (std::abs(topology - lastTopology) > 0.01f) {
		generateWavetable(topology);
		lastTopology = topology;
	}
	
	// ================================================================
	// SIGNAL GENERATION CHAIN
	// ================================================================
	
	float output = 0.0f;
	
	// Stage 1: Spectral Bloom (additive synthesis)
	if (bloom > 0.05f) {
		output = applySpectralBloom(frequency, bloom, args.sampleRate);
	} else {
		// Simple sine base when bloom is off
		output = std::sin(2.0f * M_PI * phase);
	}
	
	// Stage 2: Fragmentation (granular micro-segmentation)
	if (fragmentation > 0.05f) {
		output = processFragmentation(phase, fragmentation);
	}
	
	// Stage 3: Topology Warp (mathematical waveshaping)
	output = applyTopologyWarp(output, topology);
	
	// Stage 4: Temporal Skew (phase distortion)
	output = applyTemporalSkew(output, skew, args.sampleRate);
	
	// Update phase
	phase += frequency * args.sampleTime;
	while (phase >= 1.0f) phase -= 1.0f;
	
	// Output with appropriate scaling
	outputs[AUDIO_OUTPUT].setVoltage(output * 5.0f);
}

// ================================================================
// DSP HELPER FUNCTIONS
// ================================================================

void SonogeneseModule::generateWavetable(float topology) {
	// Generate morphing wavetable based on topology parameter
	// sine → Chebyshev → folded → impulses
	
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		float t = (float)i / WAVETABLE_SIZE;
		float sample = 0.0f;
		
		if (topology < 0.33f) {
			// Sine to Chebyshev blend (0.0 - 0.33)
			float blend = topology / 0.33f;
			float sine = std::sin(2.0f * M_PI * t);
			float cheby = chebyshevPolynomial(5, std::sin(2.0f * M_PI * t));
			sample = sine * (1.0f - blend) + cheby * blend;
		}
		else if (topology < 0.66f) {
			// Chebyshev to folded (0.33 - 0.66)
			float blend = (topology - 0.33f) / 0.33f;
			float cheby = chebyshevPolynomial(5, std::sin(2.0f * M_PI * t));
			float folded = std::sin(2.0f * M_PI * t) * (1.0f + blend * 2.0f);
			while (folded > 1.0f) folded = 2.0f - folded;
			while (folded < -1.0f) folded = -2.0f - folded;
			sample = cheby * (1.0f - blend) + folded * blend;
		}
		else {
			// Folded to impulses (0.66 - 1.0)
			float blend = (topology - 0.66f) / 0.34f;
			float folded = std::sin(2.0f * M_PI * t) * 3.0f;
			while (folded > 1.0f) folded = 2.0f - folded;
			while (folded < -1.0f) folded = -2.0f - folded;
			
			// Discontinuous impulses
			float impulse = (t < 0.1f) ? 1.0f : -0.1f;
			sample = folded * (1.0f - blend) + impulse * blend;
		}
		
		wavetable[i] = sample;
	}
}

float SonogeneseModule::processFragmentation(float phase, float fragAmount) {
	// Granular micro-segmentation with jittered grain lengths
	
	// Grain length varies with fragmentation (long grains → short grains)
	float baseGrainLength = 1024.0f * (1.0f - fragAmount * 0.9f); // 1024 → 102 samples
	
	// Add jitter to grain position
	float jitter = fragAmount * 0.3f;
	int jitterOffset = (int)(random::uniform() * jitter * baseGrainLength);
	
	// Calculate grain read position with crossfade
	int readPos = (grainPos + jitterOffset) % WAVETABLE_SIZE;
	int nextReadPos = (readPos + 1) % WAVETABLE_SIZE;
	
	// Crossfade between grains
	float grainWindow = grainPhase / baseGrainLength;
	float window = 0.5f - 0.5f * std::cos(M_PI * 2.0f * grainWindow);
	
	float sample = wavetable[readPos] * (1.0f - grainCrossfade) + 
	               wavetable[nextReadPos] * grainCrossfade;
	sample *= window;
	
	// Advance grain
	grainPhase += 1.0f;
	if (grainPhase >= baseGrainLength) {
		grainPhase = 0.0f;
		grainPos = (int)(phase * WAVETABLE_SIZE) % WAVETABLE_SIZE;
		grainCrossfade = random::uniform();
	}
	
	return sample;
}

float SonogeneseModule::applyTopologyWarp(float sample, float topology) {
	// Additional waveshaping on top of wavetable morphing
	
	if (topology < 0.01f) return sample;
	
	// Apply Chebyshev polynomial warping
	int order = (int)(topology * 7.0f) + 2; // Orders 2-9
	float warped = chebyshevPolynomial(order, sample);
	
	return sample * (1.0f - topology * 0.5f) + warped * (topology * 0.5f);
}

float SonogeneseModule::applyTemporalSkew(float sample, float skew, float sampleRate) {
	// Nonlinear phase distortion using delay line scrubbing
	
	// Write to delay line
	delayLine[delayWritePos] = sample;
	delayWritePos = (delayWritePos + 1) % DELAY_SIZE;
	
	if (skew < 0.01f) return sample;
	
	// Calculate warped read position
	float skewAmount = (skew - 0.5f) * 2.0f; // -1 to +1
	float delayTime = std::abs(skewAmount) * 100.0f; // Up to 100 samples delay
	
	// Nonlinear time warping
	float warpedDelay = delayTime * (1.0f + std::sin(phase * 2.0f * M_PI) * skewAmount);
	warpedDelay = clamp(warpedDelay, 1.0f, (float)(DELAY_SIZE - 1));
	
	int readPos = (delayWritePos - (int)warpedDelay + DELAY_SIZE) % DELAY_SIZE;
	float frac = warpedDelay - (int)warpedDelay;
	int nextReadPos = (readPos - 1 + DELAY_SIZE) % DELAY_SIZE;
	
	float skewed = delayLine[readPos] * (1.0f - frac) + delayLine[nextReadPos] * frac;
	
	return sample * (1.0f - std::abs(skewAmount)) + skewed * std::abs(skewAmount);
}

float SonogeneseModule::applySpectralBloom(float baseFreq, float bloom, float sampleRate) {
	// Additive harmonic synthesis with extreme dynamic spreading
	
	float output = 0.0f;
	
	// Bloom controls harmonic spread and amplitude distribution
	float spread = 1.0f + bloom * 8.0f; // Spread harmonics outward (up to 9x)
	
	for (int i = 0; i < MAX_HARMONICS; i++) {
		float harmonicFreq = baseFreq * (i + 1) * spread;
		
		// Don't exceed Nyquist
		if (harmonicFreq > sampleRate * 0.45f) break;
		
		// Update harmonic phase
		harmonicPhases[i] += harmonicFreq / sampleRate;
		while (harmonicPhases[i] >= 1.0f) harmonicPhases[i] -= 1.0f;
		
		// Amplitude varies dramatically with bloom
		float amplitude = harmonicAmps[i];
		if (bloom > 0.5f) {
			// Expand: boost higher harmonics dramatically
			amplitude *= (1.0f + (bloom - 0.5f) * i * 1.5f);
		} else {
			// Collapse: emphasize fundamentals, reduce higher
			amplitude *= (1.0f - (0.5f - bloom) * i * 0.8f);
		}
		
		// Add slight detuning for organic character
		float detune = bloom * 0.02f * (i % 3 - 1);
		output += std::sin(2.0f * M_PI * (harmonicPhases[i] + detune)) * amplitude;
	}
	
	// Normalize with bloom-dependent scaling
	return output / (MAX_HARMONICS * 0.3f * (1.0f + bloom));
}

float SonogeneseModule::chebyshevPolynomial(int n, float x) {
	// Chebyshev polynomial of first kind
	// T_0(x) = 1
	// T_1(x) = x
	// T_n(x) = 2x * T_{n-1}(x) - T_{n-2}(x)
	
	if (n == 0) return 1.0f;
	if (n == 1) return x;
	
	float T0 = 1.0f;
	float T1 = x;
	float Tn = 0.0f;
	
	for (int i = 2; i <= n; i++) {
		Tn = 2.0f * x * T1 - T0;
		T0 = T1;
		T1 = Tn;
	}
	
	return Tn;
}
