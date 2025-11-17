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

        // Title label
        auto titleLabel = createWidget<ui::Label>(Vec(0, 10));
        titleLabel->box.size.x = box.size.x;
        titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        titleLabel->text = "OSCILLATEUR";
        titleLabel->fontSize = 11;
        titleLabel->color = nvgRGB(255, 255, 255);
        addChild(titleLabel);

        auto titleLabel2 = createWidget<ui::Label>(Vec(0, 22));
        titleLabel2->box.size.x = box.size.x;
        titleLabel2->alignment = ui::Label::CENTER_ALIGNMENT;
        titleLabel2->text = "TRITONIQUE";
        titleLabel2->fontSize = 11;
        titleLabel2->color = nvgRGB(255, 255, 255);
        addChild(titleLabel2);

        // Main knobs (2x2 grid) - matching Diffusaire 4HP layout
        float knobLeftX = 13.16f;
        float knobRightX = 26.32f;
        float knobTopY = 30.48f;
        float knobBottomY = 55.8f;
        
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobTopY)), module, OscillateurTritoniqueModule::TOPOLOGY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobTopY)), module, OscillateurTritoniqueModule::SKEW_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobBottomY)), module, OscillateurTritoniqueModule::BLOOM_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobBottomY)), module, OscillateurTritoniqueModule::GLIDE_PARAM));

        // Attenuverters (small knobs at Y=75mm and 85mm)
        addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 75.0f)), module, OscillateurTritoniqueModule::TOPOLOGY_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 75.0f)), module, OscillateurTritoniqueModule::SKEW_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 85.0f)), module, OscillateurTritoniqueModule::BLOOM_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 85.0f)), module, OscillateurTritoniqueModule::GLIDE_ATTEN_PARAM));

        // CV inputs (4 jacks in a row at Y=95mm)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0f, 95.0f)), module, OscillateurTritoniqueModule::TOPOLOGY_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5f, 95.0f)), module, OscillateurTritoniqueModule::SKEW_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.0f, 95.0f)), module, OscillateurTritoniqueModule::BLOOM_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.5f, 95.0f)), module, OscillateurTritoniqueModule::GLIDE_CV_INPUT));

        // V/Oct input and Audio output (bottom)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.16f, 108.41f)), module, OscillateurTritoniqueModule::VOCT_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.32f, 108.41f)), module, OscillateurTritoniqueModule::AUDIO_OUTPUT));

        // Labels
        auto topoLabel = createWidget<ui::Label>(Vec(2, 106));
        topoLabel->box.size.x = box.size.x / 2;
        topoLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        topoLabel->text = "Topology";
        topoLabel->fontSize = 10;
        topoLabel->color = nvgRGB(200, 200, 200);
        addChild(topoLabel);

        auto skewLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 106));
        skewLabel->box.size.x = box.size.x / 2;
        skewLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        skewLabel->text = "Skew";
        skewLabel->fontSize = 10;
        skewLabel->color = nvgRGB(200, 200, 200);
        addChild(skewLabel);

        auto bloomLabel = createWidget<ui::Label>(Vec(2, 181));
        bloomLabel->box.size.x = box.size.x / 2;
        bloomLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        bloomLabel->text = "Bloom";
        bloomLabel->fontSize = 10;
        bloomLabel->color = nvgRGB(200, 200, 200);
        addChild(bloomLabel);

        auto glideLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 181));
        glideLabel->box.size.x = box.size.x / 2;
        glideLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        glideLabel->text = "Glide";
        glideLabel->fontSize = 10;
        glideLabel->color = nvgRGB(200, 200, 200);
        addChild(glideLabel);

        auto voctLabel = createWidget<ui::Label>(Vec(2, 330));
        voctLabel->box.size.x = box.size.x / 2;
        voctLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        voctLabel->text = "V/Oct";
        voctLabel->fontSize = 11;
        voctLabel->color = nvgRGB(200, 200, 200);
        addChild(voctLabel);

        auto outLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 330));
        outLabel->box.size.x = box.size.x / 2;
        outLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        outLabel->text = "Out";
        outLabel->fontSize = 11;
        outLabel->color = nvgRGB(200, 200, 200);
        addChild(outLabel);
    }
};

Model* modelOscillateurTritonique = createModel<OscillateurTritoniqueModule, OscillateurTritoniqueWidget>("OscillateurTritonique");
