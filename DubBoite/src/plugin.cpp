#include "plugin.hpp"
#include "DubBoiteModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
    pluginInstance = p;
    p->addModel(modelDubBoite);
}

struct DubBoiteWidget : ModuleWidget {
    DubBoiteWidget(DubBoiteModule* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/DubBoiteModule.svg")));

        // Screws
        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<ScrewSilver>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // Channel faders (12HP = ~60.6mm width)
        float xBase = 10.f;
        float xSpacing = 12.f;
        float faderY = 60.f;
        
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(xBase, faderY)), module, DubBoiteModule::CH1_FADER));
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(xBase + xSpacing, faderY)), module, DubBoiteModule::CH2_FADER));
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(xBase + xSpacing * 2, faderY)), module, DubBoiteModule::CH3_FADER));
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(xBase + xSpacing * 3, faderY)), module, DubBoiteModule::CH4_FADER));
        
        // Channel lights above faders
        addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(xBase, 35.f)), module, DubBoiteModule::CH1_LIGHT));
        addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(xBase + xSpacing, 35.f)), module, DubBoiteModule::CH2_LIGHT));
        addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(xBase + xSpacing * 2, 35.f)), module, DubBoiteModule::CH3_LIGHT));
        addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(xBase + xSpacing * 3, 35.f)), module, DubBoiteModule::CH4_LIGHT));

        // Channel inputs below faders
        float inputY = 95.f;
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xBase, inputY)), module, DubBoiteModule::CH1_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xBase + xSpacing, inputY)), module, DubBoiteModule::CH2_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xBase + xSpacing * 2, inputY)), module, DubBoiteModule::CH3_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xBase + xSpacing * 3, inputY)), module, DubBoiteModule::CH4_INPUT));

        // Effect knobs (2x2 grid in lower section)
        float knobX1 = 12.f;
        float knobX2 = 30.f;
        float knobY1 = 25.f;
        float knobY2 = 40.f;
        
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(knobX1, knobY1)), module, DubBoiteModule::DIFFUSION_KNOB));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(knobX2, knobY1)), module, DubBoiteModule::SCRUB_KNOB));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(knobX1, knobY2)), module, DubBoiteModule::LOWDRIFT_KNOB));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(knobX2, knobY2)), module, DubBoiteModule::SATURATION_KNOB));

        // Master fader and VU
        float masterX = 48.f;
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(masterX, faderY)), module, DubBoiteModule::MASTER_FADER));
        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(masterX, 35.f)), module, DubBoiteModule::MASTER_LIGHTS));

        // Outputs at bottom
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(42.f, 108.f)), module, DubBoiteModule::MIX_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.f, 108.f)), module, DubBoiteModule::SEND_OUTPUT));
    }
};

Model* modelDubBoite = createModel<DubBoiteModule, DubBoiteWidget>("DubBoite");
