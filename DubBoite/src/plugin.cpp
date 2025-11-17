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

        // Title label
        auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
        titleLabel->box.size.x = box.size.x;
        titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        titleLabel->text = "DUBBOÎTE 2/4";
        titleLabel->fontSize = 14;
        titleLabel->color = nvgRGB(255, 255, 255);
        addChild(titleLabel);

        // Effect knobs (horizontal row) - 20mm spacing between each knob
        float totalWidth = 243.84f / 5.08f;  // 48mm total width
        float knobY = 131.0f / 5.08f;  // 25.8mm
        float spacing = 20.0f;  // 20mm between knobs
        
        // Calculate starting position to center the 4 knobs with 20mm gaps
        // Total knob span = 3 gaps of 20mm = 60mm
        // Center point = 24mm, so first knob at 24 - 30 = -6mm (need to shift right)
        // Let's start at a reasonable left margin instead
        float startX = 12.0f;  // Start 12mm from left edge (8mm + 4mm shift)
        
        float knob1X = startX + 2.0f;  // Diffusion - right 2mm
        float knob2X = startX + spacing * 1.0f + 1.0f;  // Scrub - right 1mm
        float knob3X = startX + spacing * 2.0f;  // LowDrift - no adjustment
        float knob4X = startX + spacing * 3.0f - 1.0f;  // Saturation - left 1mm
        
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(knob1X, knobY)), module, DubBoiteModule::DIFFUSION_KNOB));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(knob2X, knobY)), module, DubBoiteModule::SCRUB_KNOB));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(knob3X, knobY)), module, DubBoiteModule::LOWDRIFT_KNOB));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(knob4X, knobY)), module, DubBoiteModule::SATURATION_KNOB));

        // Channel faders (centered and spread out)
        float faderY = 60.f;
        float ch1X = 55.0f / 5.08f;   // Align to SVG cx=55
        float ch2X = 110.0f / 5.08f;  // Align to SVG cx=110
        float ch3X = 165.0f / 5.08f;  // Align to SVG cx=165
        float ch4X = 220.0f / 5.08f;  // Align to SVG cx=220
        
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(ch1X, faderY)), module, DubBoiteModule::CH1_FADER));
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(ch2X, faderY)), module, DubBoiteModule::CH2_FADER));
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(ch3X, faderY)), module, DubBoiteModule::CH3_FADER));
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(ch4X, faderY)), module, DubBoiteModule::CH4_FADER));
        
        // Channel lights above faders - moved down 10mm
        addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(ch1X, 45.f)), module, DubBoiteModule::CH1_LIGHT));
        addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(ch2X, 45.f)), module, DubBoiteModule::CH2_LIGHT));
        addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(ch3X, 45.f)), module, DubBoiteModule::CH3_LIGHT));
        addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(ch4X, 45.f)), module, DubBoiteModule::CH4_LIGHT));

        // Channel inputs - align to SVG circles at cy=303, lowered 20mm
        float inputY = 303.0f / 5.08f + 20.0f;  // 59.6mm + 20mm = 79.6mm
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ch1X, inputY)), module, DubBoiteModule::CH1_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ch2X, inputY)), module, DubBoiteModule::CH2_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ch3X, inputY)), module, DubBoiteModule::CH3_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ch4X, inputY)), module, DubBoiteModule::CH4_INPUT));

        // Master fader and VU - moved more to the right
        float masterX = 75.f;  // Moved from 48mm to 75mm
        addParam(createParamCentered<VCVSlider>(mm2px(Vec(masterX, faderY)), module, DubBoiteModule::MASTER_FADER));
        addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(masterX, 45.f)), module, DubBoiteModule::MASTER_LIGHTS));

        // Outputs at bottom right - up 4mm total, mix left 1mm, send right 1mm additional
        float outputY = 320.0f / 5.08f + 50.0f - 3.0f - 1.0f;  // 63mm + 50mm - 3mm - 1mm = 109mm
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(180.0f/5.08f + 25.0f + 1.0f - 1.0f, outputY)), module, DubBoiteModule::MIX_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(220.0f/5.08f + 25.0f + 5.0f + 1.0f, outputY)), module, DubBoiteModule::SEND_OUTPUT));

        // Labels
        auto diffusionLabel = createWidget<ui::Label>(Vec(10, 100));
        diffusionLabel->box.size.x = 60;
        diffusionLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        diffusionLabel->text = "Diffusion";
        diffusionLabel->fontSize = 9;
        diffusionLabel->color = nvgRGB(200, 200, 200);
        addChild(diffusionLabel);

        auto scrubLabel = createWidget<ui::Label>(Vec(90, 100));
        scrubLabel->box.size.x = 60;
        scrubLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        scrubLabel->text = "Scrub";
        scrubLabel->fontSize = 9;
        scrubLabel->color = nvgRGB(200, 200, 200);
        addChild(scrubLabel);

        auto lowdriftLabel = createWidget<ui::Label>(Vec(10, 155));
        lowdriftLabel->box.size.x = 60;
        lowdriftLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        lowdriftLabel->text = "LowDrift";
        lowdriftLabel->fontSize = 9;
        lowdriftLabel->color = nvgRGB(200, 200, 200);
        addChild(lowdriftLabel);

        auto saturationLabel = createWidget<ui::Label>(Vec(90, 155));
        saturationLabel->box.size.x = 60;
        saturationLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        saturationLabel->text = "Saturation";
        saturationLabel->fontSize = 9;
        saturationLabel->color = nvgRGB(200, 200, 200);
        addChild(saturationLabel);

        auto ch1Label = createWidget<ui::Label>(Vec(10, 282));
        ch1Label->box.size.x = 40;
        ch1Label->alignment = ui::Label::CENTER_ALIGNMENT;
        ch1Label->text = "1";
        ch1Label->fontSize = 10;
        ch1Label->color = nvgRGB(200, 200, 200);
        addChild(ch1Label);

        auto ch2Label = createWidget<ui::Label>(Vec(55, 282));
        ch2Label->box.size.x = 40;
        ch2Label->alignment = ui::Label::CENTER_ALIGNMENT;
        ch2Label->text = "2";
        ch2Label->fontSize = 10;
        ch2Label->color = nvgRGB(200, 200, 200);
        addChild(ch2Label);

        auto ch3Label = createWidget<ui::Label>(Vec(100, 282));
        ch3Label->box.size.x = 40;
        ch3Label->alignment = ui::Label::CENTER_ALIGNMENT;
        ch3Label->text = "3";
        ch3Label->fontSize = 10;
        ch3Label->color = nvgRGB(200, 200, 200);
        addChild(ch3Label);

        auto ch4Label = createWidget<ui::Label>(Vec(145, 282));
        ch4Label->box.size.x = 40;
        ch4Label->alignment = ui::Label::CENTER_ALIGNMENT;
        ch4Label->text = "4";
        ch4Label->fontSize = 10;
        ch4Label->color = nvgRGB(200, 200, 200);
        addChild(ch4Label);

        auto mixLabel = createWidget<ui::Label>(Vec(140, 330));
        mixLabel->box.size.x = 50;
        mixLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        mixLabel->text = "Mix";
        mixLabel->fontSize = 11;
        mixLabel->color = nvgRGB(200, 200, 200);
        addChild(mixLabel);

        auto sendLabel = createWidget<ui::Label>(Vec(190, 330));
        sendLabel->box.size.x = 50;
        sendLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        sendLabel->text = "Send";
        sendLabel->fontSize = 11;
        sendLabel->color = nvgRGB(200, 200, 200);
        addChild(sendLabel);
    }
};

Model* modelDubBoite = createModel<DubBoiteModule, DubBoiteWidget>("DubBoite");
