#pragma once
#include "rack.hpp"

struct LfoModule : rack::Module {
    enum ParamIds {
        FREQUENCY_PARAM,
        CHAOS_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        GATE_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        SINE_OUTPUT,
        GATE_OUTPUT,
        NUM_OUTPUTS
    };

    float phase = 0.0f;
    float chaosValue = 0.0f;
    float sampleTimer = 0.0f;
    uint32_t rng = 12345u;
    bool lastGateHigh = false;

    LfoModule();
    void process(const ProcessArgs& args) override;
};;