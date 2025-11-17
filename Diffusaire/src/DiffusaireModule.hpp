#pragma once
#include "plugin.hpp"

struct DiffusaireModule : Module {
	enum ParamId {
		CONTOURS_PARAM,
		RESONANCE_PARAM,
		ECART_PARAM,
		CINETIQUES_PARAM,
		CONTOURS_ATTEN_PARAM,
		RESONANCE_ATTEN_PARAM,
		ECART_ATTEN_PARAM,
		CINETIQUES_ATTEN_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		AUDIO_INPUT,
		CONTOURS_INPUT,
		RESONANCE_INPUT,
		ECART_INPUT,
		CINETIQUES_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		NUM_LIGHTS
	};

	// Multi-pole filter state (cascaded filters)
	static const int NUM_POLES = 4;
	float lowpass[NUM_POLES] = {};
	float bandpass[NUM_POLES] = {};
	float highpass[NUM_POLES] = {};
	
	// All-pass networks for phase dispersion
	static const int NUM_ALLPASS = 6;
	float allpassState[NUM_ALLPASS][2] = {};  // [stage][z1, z2]
	
	// Delay line for cinétiques (micro-motion)
	static const int DELAY_SIZE = 8192;
	float delayLine[DELAY_SIZE] = {};
	int delayWritePos = 0;
	
	// LFO for organic fluctuations
	float lfoPhase = 0.0f;
	float lfoPhase2 = 0.0f;
	
	// Resonance character state
	float resonanceMemory = 0.0f;

	DiffusaireModule();
	void process(const ProcessArgs& args) override;

	// DSP functions
	float processContours(float input, float contours, float sampleRate);
	float processResonanceVariable(float input, float resonance, float freq, float sampleRate);
	float processEcart(float input, float ecart);
	float processCinetiques(float input, float cinetiques, float sampleRate);
	float allpassFilter(float input, float coeff, int stage);
};
