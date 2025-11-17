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

        // Title label
        auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
        titleLabel->box.size.x = box.size.x;
        titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        titleLabel->text = "TEMPORALISTE";
        titleLabel->fontSize = 12;
        titleLabel->color = nvgRGB(255, 255, 255);
        addChild(titleLabel);

        // Knobs (2x2 grid) - matching SVG coordinates (38.48px, 76.8px at cy=90, 164)
        float knobLeftX = 13.16f;   // 38.48/5.08 * 1.742
        float knobRightX = 26.32f;  // 76.8/5.08 * 1.742
        float knobTopY = 30.48f;
        float knobBottomY = 55.8f;
        
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobTopY)), module, TemporalisteModule::DENSITY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobTopY)), module, TemporalisteModule::DRIFT_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobBottomY)), module, TemporalisteModule::ACCENT_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobBottomY)), module, TemporalisteModule::TIMESHIFT_PARAM));

        // Attenuverters
        addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 75.0f)), module, TemporalisteModule::DENSITY_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 75.0f)), module, TemporalisteModule::DRIFT_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX - 4.0f, 85.0f)), module, TemporalisteModule::ACCENT_ATTEN_PARAM));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX + 4.0f, 85.0f)), module, TemporalisteModule::TIMESHIFT_ATTEN_PARAM));

        // CV inputs (4 in a row)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0f, 103.0f)), module, TemporalisteModule::DENSITY_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5f, 103.0f)), module, TemporalisteModule::DRIFT_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.0f, 103.0f)), module, TemporalisteModule::ACCENT_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.5f, 103.0f)), module, TemporalisteModule::TIMESHIFT_CV_INPUT));

        // Clock input (centered, matching SVG cy=250)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.64f, 84.8f)), module, TemporalisteModule::CLOCK_INPUT));
        
        // Gate outputs (row at bottom, matching SVG cy=320)
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.8f, 112.41f)), module, TemporalisteModule::GATE1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24f, 112.41f)), module, TemporalisteModule::GATE2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.76f, 112.41f)), module, TemporalisteModule::GATE3_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.2f, 112.41f)), module, TemporalisteModule::GATE4_OUTPUT));

        // Labels
        auto densityLabel = createWidget<ui::Label>(Vec(2, 106));
        densityLabel->box.size.x = box.size.x / 2;
        densityLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        densityLabel->text = "Density";
        densityLabel->fontSize = 10;
        densityLabel->color = nvgRGB(200, 200, 200);
        addChild(densityLabel);

        auto driftLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 106));
        driftLabel->box.size.x = box.size.x / 2;
        driftLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        driftLabel->text = "Drift";
        driftLabel->fontSize = 10;
        driftLabel->color = nvgRGB(200, 200, 200);
        addChild(driftLabel);

        auto accentLabel = createWidget<ui::Label>(Vec(2, 181));
        accentLabel->box.size.x = box.size.x / 2;
        accentLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        accentLabel->text = "Accent";
        accentLabel->fontSize = 10;
        accentLabel->color = nvgRGB(200, 200, 200);
        addChild(accentLabel);

        auto timeshiftLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 181));
        timeshiftLabel->box.size.x = box.size.x / 2;
        timeshiftLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        timeshiftLabel->text = "TimeShift";
        timeshiftLabel->fontSize = 9;
        timeshiftLabel->color = nvgRGB(200, 200, 200);
        addChild(timeshiftLabel);

        auto clockLabel = createWidget<ui::Label>(Vec(0, 268));
        clockLabel->box.size.x = box.size.x;
        clockLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        clockLabel->text = "Clock";
        clockLabel->fontSize = 11;
        clockLabel->color = nvgRGB(200, 200, 200);
        addChild(clockLabel);

        auto g1Label = createWidget<ui::Label>(Vec(0, 337));
        g1Label->box.size.x = box.size.x / 4;
        g1Label->alignment = ui::Label::CENTER_ALIGNMENT;
        g1Label->text = "G1";
        g1Label->fontSize = 10;
        g1Label->color = nvgRGB(200, 200, 200);
        addChild(g1Label);

        auto g2Label = createWidget<ui::Label>(Vec(box.size.x / 4, 337));
        g2Label->box.size.x = box.size.x / 4;
        g2Label->alignment = ui::Label::CENTER_ALIGNMENT;
        g2Label->text = "G2";
        g2Label->fontSize = 10;
        g2Label->color = nvgRGB(200, 200, 200);
        addChild(g2Label);

        auto g3Label = createWidget<ui::Label>(Vec(box.size.x / 2, 337));
        g3Label->box.size.x = box.size.x / 4;
        g3Label->alignment = ui::Label::CENTER_ALIGNMENT;
        g3Label->text = "G3";
        g3Label->fontSize = 10;
        g3Label->color = nvgRGB(200, 200, 200);
        addChild(g3Label);

        auto g4Label = createWidget<ui::Label>(Vec(3 * box.size.x / 4, 337));
        g4Label->box.size.x = box.size.x / 4;
        g4Label->alignment = ui::Label::CENTER_ALIGNMENT;
        g4Label->text = "G4";
        g4Label->fontSize = 10;
        g4Label->color = nvgRGB(200, 200, 200);
        addChild(g4Label);
    }
};

Model* modelTemporaliste = createModel<TemporalisteModule, TemporalisteWidget>("Temporaliste");
