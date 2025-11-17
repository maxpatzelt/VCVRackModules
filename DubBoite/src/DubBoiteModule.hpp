#pragma once
#include "rack.hpp"

struct DubBoiteModule : rack::Module {
	enum ParamIds {
		SEND_DIFFUSION_PARAM,
		TAPE_SCRUB_PARAM,
		LOW_DRIFT_PARAM,
		SATURATION_BLOOM_PARAM,
		SEND_DIFFUSION_ATTEN_PARAM,
		TAPE_SCRUB_ATTEN_PARAM,
		LOW_DRIFT_ATTEN_PARAM,
		SATURATION_BLOOM_ATTEN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CHANNEL_1_INPUT,
		CHANNEL_2_INPUT,
		CHANNEL_3_INPUT,
		CHANNEL_4_INPUT,
		SEND_DIFFUSION_CV_INPUT,
		TAPE_SCRUB_CV_INPUT,
		LOW_DRIFT_CV_INPUT,
		SATURATION_BLOOM_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		MIX_OUTPUT,
		SEND_OUTPUT,
		NUM_OUTPUTS
	};

	// Delay lines for tape scrub and send diffusion
	static constexpr int DELAY_SIZE = 16384;
	static constexpr int NUM_CHANNELS = 4;
	static constexpr int NUM_PATHS = 8;
	
	float delayBuffers[NUM_CHANNELS][DELAY_SIZE];
	int delayWritePos[NUM_CHANNELS];
	
	// Multi-path diffusion state
	float diffusionPaths[NUM_PATHS][DELAY_SIZE];
	int diffusionWritePos[NUM_PATHS];
	float pathAmps[NUM_PATHS];
	
	// Low-end filter state
	float lowpassState[NUM_CHANNELS];
	float lowDriftPhase;
	
	// Saturation state
	float saturationMemory[NUM_CHANNELS];
	
	// Tape scrub LFO
	float scrubPhase;
	
	// RNG
	uint32_t rng;

	DubBoiteModule();
	void process(const ProcessArgs& args) override;
	
	// DSP helper functions
	float processSendDiffusion(float input, int channel, float diffusion, float sampleRate);
	float processTapeScrub(float input, int channel, float scrub, float sampleRate);
	float processLowDrift(float input, int channel, float drift, float sampleRate);
	float processSaturationBloom(float input, int channel, float bloom);
	
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
