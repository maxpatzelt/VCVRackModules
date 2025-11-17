#include "plugin.hpp"
#include "OscillateurTritoniqueModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
    pluginInstance = p;
    p->addModel(modelOscillateurTritonique);
}

struct OscillateurTritoniqueWidget : ModuleWidget {
    OscillateurTritoniqueWidget(OscillateurTritoniqueModule* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/OscillateurTritoniqueModule.svg")));

        // Screws
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<ScrewSilver>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // Knobs (2x2 grid)
        // Row 1
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.0, 24.0)), module, OscillateurTritoniqueModule::TOPOLOGY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.0, 24.0)), module, OscillateurTritoniqueModule::SKEW_PARAM));
        
        // Row 2
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.0, 44.0)), module, OscillateurTritoniqueModule::BLOOM_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.0, 44.0)), module, OscillateurTritoniqueModule::GLIDE_PARAM));

        // Attenuverters (below each knob)
        addParam(createParamCentered<Trimpot>(mm2px(Vec(8.0, 35.0)), module, OscillateurTritoniqueModule::TOPOLOGY_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(22.0, 35.0)), module, OscillateurTritoniqueModule::SKEW_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(8.0, 55.0)), module, OscillateurTritoniqueModule::BLOOM_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(22.0, 55.0)), module, OscillateurTritoniqueModule::GLIDE_ATTEN_PARAM));

        // CV inputs (below attenuverters)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 65.0)), module, OscillateurTritoniqueModule::TOPOLOGY_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.0, 65.0)), module, OscillateurTritoniqueModule::SKEW_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 85.0)), module, OscillateurTritoniqueModule::BLOOM_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.0, 85.0)), module, OscillateurTritoniqueModule::GLIDE_CV_INPUT));

        // V/Oct input
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 100.0)), module, OscillateurTritoniqueModule::VOCT_INPUT));
        
        // Audio output
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.0, 100.0)), module, OscillateurTritoniqueModule::AUDIO_OUTPUT));
    }
};

Model* modelOscillateurTritonique = createModel<OscillateurTritoniqueModule, OscillateurTritoniqueWidget>("OscillateurTritonique");
