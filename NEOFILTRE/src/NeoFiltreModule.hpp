#pragma once
#include "plugin.hpp"

struct NeoFiltreModule : Module {
	enum ParamId {
		COUPURE_PARAM,
		RESONANCE_PARAM,
		GAUSS_PARAM,
		SATINE_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		AUDIO_INPUT,
		COUPURE_INPUT,
		RESONANCE_INPUT,
		GAUSS_INPUT,
		SATINE_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		NUM_LIGHTS
	};

	// State-variable filter state
	float lowpass = 0.f;
	float bandpass = 0.f;
	float highpass = 0.f;
	
	// Filter coefficients
	float filterFreq = 1000.f;
	float filterQ = 0.5f;

	NeoFiltreModule();
	void process(const ProcessArgs& args) override;

	// DSP helper functions
	float processStateVariableFilter(float input, float cutoff, float resonance, float sampleRate);
	float applyGaussShaping(float sample, float gauss);
	float applySatine(float sample, float satine);
};
