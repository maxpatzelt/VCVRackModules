#pragma once
#include "rack.hpp"

struct SirenConcreteModule : rack::Module {
	enum ParamIds {
		GRAIN_MORPH_PARAM,
		SPECTRAL_SHIFT_PARAM,
		PHASE_DRIFT_PARAM,
		ECHO_BLOOM_PARAM,
		GRAIN_MORPH_ATTEN_PARAM,
		SPECTRAL_SHIFT_ATTEN_PARAM,
		PHASE_DRIFT_ATTEN_PARAM,
		ECHO_BLOOM_ATTEN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		GRAIN_MORPH_CV_INPUT,
		SPECTRAL_SHIFT_CV_INPUT,
		PHASE_DRIFT_CV_INPUT,
		ECHO_BLOOM_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};

	// Granular synthesis state
	static constexpr int WAVETABLE_SIZE = 2048;
	static constexpr int MAX_GRAINS = 8;
	static constexpr int DELAY_SIZE = 8192;
	
	float wavetable[WAVETABLE_SIZE];
	float grainPhases[MAX_GRAINS];
	float grainAmps[MAX_GRAINS];
	float grainRates[MAX_GRAINS];
	int activeGrains;
	
	// Spectral shift state
	float harmonicPhases[16];
	float harmonicAmps[16];
	
	// Phase drift LFO
	float driftPhase;
	float driftAmount;
	
	// Echo bloom delay
	float delayBuffer[DELAY_SIZE];
	int delayWritePos;
	
	// Base oscillator
	float basePhase;
	float baseFreq;
	
	// RNG
	uint32_t rng;

	SirenConcreteModule();
	void process(const ProcessArgs& args) override;
	
	// DSP helper functions
	float processGrainMorph(float input, float morph, float sampleRate);
	float processSpectralShift(float input, float shift, float freq);
	float processPhaseDrift(float input, float drift, float sampleRate);
	float processEchoBloom(float input, float bloom, float sampleRate);
	
	uint32_t xorshift32() {
		rng ^= rng << 13;
		rng ^= rng >> 17;
		rng ^= rng << 5;
		return rng;
	}
	
	float random() {
		return (float)xorshift32() / (float)0xFFFFFFFF;
	}
};
