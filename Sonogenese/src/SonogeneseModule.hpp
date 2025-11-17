#pragma once
#include "plugin.hpp"

struct SonogeneseModule : Module {
	enum ParamId {
		FRAGMENTATION_PARAM,
		TOPOLOGY_PARAM,
		SKEW_PARAM,
		BLOOM_PARAM,
		FRAGMENTATION_ATTEN_PARAM,
		TOPOLOGY_ATTEN_PARAM,
		SKEW_ATTEN_PARAM,
		BLOOM_ATTEN_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		VOCT_INPUT,
		FRAGMENTATION_INPUT,
		TOPOLOGY_INPUT,
		SKEW_INPUT,
		BLOOM_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		NUM_LIGHTS
	};

	// Oscillator state
	float phase = 0.f;
	float frequency = 261.626f;  // C4
	
	// Wavetable grain engine
	static const int WAVETABLE_SIZE = 2048;
	float wavetable[WAVETABLE_SIZE];
	int grainPos = 0;
	float grainPhase = 0.f;
	float grainLength = 512.f;
	float grainCrossfade = 0.f;
	
	// Delay line for temporal skew
	static const int DELAY_SIZE = 4096;
	float delayLine[DELAY_SIZE] = {};
	int delayWritePos = 0;
	
	// Harmonic state for spectral bloom
	static const int MAX_HARMONICS = 16;
	float harmonicPhases[MAX_HARMONICS] = {};
	float harmonicAmps[MAX_HARMONICS] = {};

	SonogeneseModule();
	void process(const ProcessArgs& args) override;

	// DSP functions
	void generateWavetable(float topology);
	float processFragmentation(float phase, float fragAmount);
	float applyTopologyWarp(float sample, float topology);
	float applyTemporalSkew(float sample, float skew, float sampleRate);
	float applySpectralBloom(float baseFreq, float bloom, float sampleRate);
	float chebyshevPolynomial(int n, float x);
};
