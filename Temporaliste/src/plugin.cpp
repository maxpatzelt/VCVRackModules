#include "plugin.hpp"
#include "TemporalisteModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
    pluginInstance = p;
    p->addModel(modelTemporaliste);
}

struct TemporalisteWidget : ModuleWidget {
    TemporalisteWidget(TemporalisteModule* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/TemporalisteModule.svg")));

        // Screws
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<ScrewSilver>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // Knobs (2x2 grid)
        // Row 1
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.0, 24.0)), module, TemporalisteModule::DENSITY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.0, 24.0)), module, TemporalisteModule::DRIFT_PARAM));
        
        // Row 2
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.0, 44.0)), module, TemporalisteModule::ACCENT_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.0, 44.0)), module, TemporalisteModule::TIMESHIFT_PARAM));

        // Attenuverters (below each knob)
        addParam(createParamCentered<Trimpot>(mm2px(Vec(8.0, 35.0)), module, TemporalisteModule::DENSITY_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(22.0, 35.0)), module, TemporalisteModule::DRIFT_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(8.0, 55.0)), module, TemporalisteModule::ACCENT_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(22.0, 55.0)), module, TemporalisteModule::TIMESHIFT_ATTEN_PARAM));

        // CV inputs (below attenuverters)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 65.0)), module, TemporalisteModule::DENSITY_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.0, 65.0)), module, TemporalisteModule::DRIFT_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 85.0)), module, TemporalisteModule::ACCENT_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.0, 85.0)), module, TemporalisteModule::TIMESHIFT_CV_INPUT));

        // Clock input (centered)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.0, 95.0)), module, TemporalisteModule::CLOCK_INPUT));
        
        // Gate outputs (row at bottom)
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.0, 108.0)), module, TemporalisteModule::GATE1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.0, 108.0)), module, TemporalisteModule::GATE2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.0, 108.0)), module, TemporalisteModule::GATE3_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.0, 108.0)), module, TemporalisteModule::GATE4_OUTPUT));
    }
};

Model* modelTemporaliste = createModel<TemporalisteModule, TemporalisteWidget>("Temporaliste");
