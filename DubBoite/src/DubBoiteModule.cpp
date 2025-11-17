#include "DubBoiteModule.hpp"
#include <cmath>

using namespace rack;

DubBoiteModule::DubBoiteModule() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	
	// Main knobs
	configParam(SEND_DIFFUSION_PARAM, 0.0f, 1.0f, 0.0f, "Send Diffusion", "%", 0.f, 100.f);
	configParam(TAPE_SCRUB_PARAM, 0.0f, 1.0f, 0.0f, "Tape Scrub", "%", 0.f, 100.f);
	configParam(LOW_DRIFT_PARAM, 0.0f, 1.0f, 0.0f, "Low-End Drift", "%", 0.f, 100.f);
	configParam(SATURATION_BLOOM_PARAM, 0.0f, 1.0f, 0.0f, "Saturation Bloom", "%", 0.f, 100.f);
	
	// Attenuverters
	configParam(SEND_DIFFUSION_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Send Diffusion CV", "%", 0.f, 100.f);
	configParam(TAPE_SCRUB_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Tape Scrub CV", "%", 0.f, 100.f);
	configParam(LOW_DRIFT_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Low-End Drift CV", "%", 0.f, 100.f);
	configParam(SATURATION_BLOOM_ATTEN_PARAM, -1.0f, 1.0f, 0.0f, "Saturation Bloom CV", "%", 0.f, 100.f);
	
	// Inputs
	configInput(CHANNEL_1_INPUT, "Channel 1");
	configInput(CHANNEL_2_INPUT, "Channel 2");
	configInput(CHANNEL_3_INPUT, "Channel 3");
	configInput(CHANNEL_4_INPUT, "Channel 4");
	configInput(SEND_DIFFUSION_CV_INPUT, "Send Diffusion CV");
	configInput(TAPE_SCRUB_CV_INPUT, "Tape Scrub CV");
	configInput(LOW_DRIFT_CV_INPUT, "Low-End Drift CV");
	configInput(SATURATION_BLOOM_CV_INPUT, "Saturation Bloom CV");
	
	// Outputs
	configOutput(MIX_OUTPUT, "Mix");
	configOutput(SEND_OUTPUT, "Send");
	
	// Initialize state
	scrubPhase = 0.0f;
	lowDriftPhase = 0.0f;
	rng = 54321;
	
	for (int ch = 0; ch < NUM_CHANNELS; ch++) {
		delayWritePos[ch] = 0;
		lowpassState[ch] = 0.0f;
		saturationMemory[ch] = 0.0f;
		for (int i = 0; i < DELAY_SIZE; i++) {
			delayBuffers[ch][i] = 0.0f;
		}
	}
	
	for (int p = 0; p < NUM_PATHS; p++) {
		diffusionWritePos[p] = 0;
		pathAmps[p] = 0.7f + random() * 0.3f;
		for (int i = 0; i < DELAY_SIZE; i++) {
			diffusionPaths[p][i] = 0.0f;
		}
	}
}

void DubBoiteModule::process(const ProcessArgs& args) {
	// Get CV-modulated parameters
	float sendDiffusion = params[SEND_DIFFUSION_PARAM].getValue();
	if (inputs[SEND_DIFFUSION_CV_INPUT].isConnected()) {
		sendDiffusion += inputs[SEND_DIFFUSION_CV_INPUT].getVoltage() * params[SEND_DIFFUSION_ATTEN_PARAM].getValue() * 0.1f;
	}
	sendDiffusion = clamp(sendDiffusion, 0.0f, 1.0f);
	
	float tapeScrub = params[TAPE_SCRUB_PARAM].getValue();
	if (inputs[TAPE_SCRUB_CV_INPUT].isConnected()) {
		tapeScrub += inputs[TAPE_SCRUB_CV_INPUT].getVoltage() * params[TAPE_SCRUB_ATTEN_PARAM].getValue() * 0.1f;
	}
	tapeScrub = clamp(tapeScrub, 0.0f, 1.0f);
	
	float lowDrift = params[LOW_DRIFT_PARAM].getValue();
	if (inputs[LOW_DRIFT_CV_INPUT].isConnected()) {
		lowDrift += inputs[LOW_DRIFT_CV_INPUT].getVoltage() * params[LOW_DRIFT_ATTEN_PARAM].getValue() * 0.1f;
	}
	lowDrift = clamp(lowDrift, 0.0f, 1.0f);
	
	float saturationBloom = params[SATURATION_BLOOM_PARAM].getValue();
	if (inputs[SATURATION_BLOOM_CV_INPUT].isConnected()) {
		saturationBloom += inputs[SATURATION_BLOOM_CV_INPUT].getVoltage() * params[SATURATION_BLOOM_ATTEN_PARAM].getValue() * 0.1f;
	}
	saturationBloom = clamp(saturationBloom, 0.0f, 1.0f);
	
	// Process all channels
	float mixSum = 0.0f;
	float sendSum = 0.0f;
	
	Input* channelInputs[4] = {
		&inputs[CHANNEL_1_INPUT],
		&inputs[CHANNEL_2_INPUT],
		&inputs[CHANNEL_3_INPUT],
		&inputs[CHANNEL_4_INPUT]
	};
	
	for (int ch = 0; ch < NUM_CHANNELS; ch++) {
		if (!channelInputs[ch]->isConnected()) continue;
		
		float input = channelInputs[ch]->getVoltage();
		
		// Process each effect stage
		float processed = input;
		
		// Stage 1: Low-End Drift
		processed = processLowDrift(processed, ch, lowDrift, args.sampleRate);
		
		// Stage 2: Tape Scrub
		processed = processTapeScrub(processed, ch, tapeScrub, args.sampleRate);
		
		// Stage 3: Saturation Bloom
		processed = processSaturationBloom(processed, ch, saturationBloom);
		
		// Add to mix
		mixSum += processed * 0.25f; // Normalize by channel count
		
		// Send to diffusion network
		float diffused = processSendDiffusion(processed, ch, sendDiffusion, args.sampleRate);
		sendSum += diffused * 0.25f;
	}
	
	outputs[MIX_OUTPUT].setVoltage(clamp(mixSum, -10.0f, 10.0f));
	outputs[SEND_OUTPUT].setVoltage(clamp(sendSum, -10.0f, 10.0f));
}

// ================================================================
// DSP HELPER FUNCTIONS
// ================================================================

float DubBoiteModule::processSendDiffusion(float input, int channel, float diffusion, float sampleRate) {
	if (diffusion < 0.01f) return input;
	
	// Spread signal across multiple micro-paths for spatial diffusion
	float output = input * (1.0f - diffusion * 0.8f);
	
	for (int p = 0; p < NUM_PATHS; p++) {
		// Each path has different delay time
		float pathDelay = 0.01f + (float)p * 0.008f + diffusion * 0.05f;
		int delaySamples = (int)(pathDelay * sampleRate);
		
		// Write to path
		diffusionPaths[p][diffusionWritePos[p]] = input;
		diffusionWritePos[p] = (diffusionWritePos[p] + 1) % DELAY_SIZE;
		
		// Read from path with modulation
		int readPos = (diffusionWritePos[p] - delaySamples + DELAY_SIZE) % DELAY_SIZE;
		float pathSample = diffusionPaths[p][readPos];
		
		// Apply path-specific amplitude and coloration
		float pathOutput = pathSample * pathAmps[p] * diffusion;
		
		// Spectral coloration per path
		if (p % 3 == 0) {
			pathOutput *= 0.8f; // Darker
		} else if (p % 3 == 1) {
			pathOutput *= 1.2f; // Brighter
		}
		
		output += pathOutput / (float)NUM_PATHS;
	}
	
	return output;
}

float DubBoiteModule::processTapeScrub(float input, int channel, float scrub, float sampleRate) {
	if (scrub < 0.01f) return input;
	
	// Tape-style delay modulation for imperfection
	float scrubLFOFreq = 0.3f + scrub * 2.0f; // 0.3 - 2.3 Hz
	scrubPhase += scrubLFOFreq / sampleRate;
	if (scrubPhase >= 1.0f) scrubPhase -= 1.0f;
	
	// Create wobble with multiple harmonics
	float wobble = std::sin(2.0f * M_PI * scrubPhase);
	wobble += 0.4f * std::sin(2.0f * M_PI * scrubPhase * 2.7f);
	wobble += 0.2f * std::sin(2.0f * M_PI * scrubPhase * 5.3f);
	
	// Write to delay
	delayBuffers[channel][delayWritePos[channel]] = input;
	delayWritePos[channel] = (delayWritePos[channel] + 1) % DELAY_SIZE;
	
	// Modulated delay time
	float baseDelay = 0.005f + scrub * 0.02f; // 5-25ms
	float modulatedDelay = baseDelay * (1.0f + wobble * scrub * 0.15f);
	int delaySamples = (int)(modulatedDelay * sampleRate);
	delaySamples = clamp(delaySamples, 1, DELAY_SIZE - 1);
	
	int readPos = (delayWritePos[channel] - delaySamples + DELAY_SIZE) % DELAY_SIZE;
	float delayedSample = delayBuffers[channel][readPos];
	
	// Blend with dry
	return input * (1.0f - scrub * 0.5f) + delayedSample * scrub * 0.5f;
}

float DubBoiteModule::processLowDrift(float input, int channel, float drift, float sampleRate) {
	if (drift < 0.01f) return input;
	
	// Detune bass frequencies with slow LFO
	lowDriftPhase += 0.2f / sampleRate; // Very slow drift (0.2 Hz)
	if (lowDriftPhase >= 1.0f) lowDriftPhase -= 1.0f;
	
	float driftLFO = std::sin(2.0f * M_PI * lowDriftPhase);
	
	// Lowpass filter to isolate bass
	float cutoff = 200.0f / sampleRate;
	float f = 2.0f * std::sin(M_PI * cutoff);
	f = clamp(f, 0.0001f, 1.5f);
	
	lowpassState[channel] += f * (input - lowpassState[channel]);
	lowpassState[channel] = clamp(lowpassState[channel], -10.0f, 10.0f);
	
	// Apply drift to low end only
	float driftedLow = lowpassState[channel] * (1.0f + driftLFO * drift * 0.1f);
	float high = input - lowpassState[channel];
	
	return driftedLow + high;
}

float DubBoiteModule::processSaturationBloom(float input, int channel, float bloom) {
	if (bloom < 0.01f) return input;
	
	// Dynamic spectral saturation with soft clipping
	float drive = 1.0f + bloom * 3.0f;
	float driven = input * drive;
	
	// Soft saturation using tanh
	float saturated = std::tanh(driven);
	
	// Add harmonic bloom through waveshaping
	float bloom2 = saturated + bloom * 0.3f * std::sin(saturated * 3.0f * M_PI);
	
	// Apply gentle feedback for warmth
	saturationMemory[channel] = bloom2 * 0.1f * bloom + saturationMemory[channel] * 0.9f;
	saturationMemory[channel] = clamp(saturationMemory[channel], -1.0f, 1.0f);
	
	float output = bloom2 + saturationMemory[channel] * bloom * 0.2f;
	
	// Normalize and blend
	output = output / drive * 0.8f;
	return input * (1.0f - bloom * 0.6f) + output * bloom * 0.6f;
}
