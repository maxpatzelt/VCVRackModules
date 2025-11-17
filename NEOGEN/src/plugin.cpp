#include "plugin.hpp"
#include "NeoGenModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
    pluginInstance = p;
    p->addModel(modelNeoGen);
}

// ==================================================================
// NEOGEN WIDGET
// ==================================================================

struct NeoGenWidget : ModuleWidget {
    NeoGenWidget(NeoGenModule* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/NeoGenModule.svg")));
        
        // Screws
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        // Title label
        auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
        titleLabel->box.size.x = box.size.x;
        titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        titleLabel->text = "NEOGEN";
        titleLabel->fontSize = 14;
        titleLabel->color = nvgRGB(255, 255, 255);
        addChild(titleLabel);
        
        // Shape label (top-left knob)
        auto shapeLabel = createWidget<ui::Label>(Vec(2, 106));
        shapeLabel->box.size.x = box.size.x / 2;
        shapeLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        shapeLabel->text = "Shape";
        shapeLabel->fontSize = 11;
        shapeLabel->color = nvgRGB(200, 200, 200);
        addChild(shapeLabel);
        
        // Artifact label (top-right knob)
        auto artifactLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 106));
        artifactLabel->box.size.x = box.size.x / 2;
        artifactLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        artifactLabel->text = "Artifact";
        artifactLabel->fontSize = 11;
        artifactLabel->color = nvgRGB(200, 200, 200);
        addChild(artifactLabel);
        
        // Harmonics label (bottom-left knob)
        auto harmonicsLabel = createWidget<ui::Label>(Vec(2, 181));
        harmonicsLabel->box.size.x = box.size.x / 2;
        harmonicsLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        harmonicsLabel->text = "Harm";
        harmonicsLabel->fontSize = 11;
        harmonicsLabel->color = nvgRGB(200, 200, 200);
        addChild(harmonicsLabel);
        
        // Drive label (bottom-right knob)
        auto driveLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 181));
        driveLabel->box.size.x = box.size.x / 2;
        driveLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        driveLabel->text = "Drive";
        driveLabel->fontSize = 11;
        driveLabel->color = nvgRGB(200, 200, 200);
        addChild(driveLabel);
        
        // V/Oct label (left jack)
        auto voctLabel = createWidget<ui::Label>(Vec(2, 330));
        voctLabel->box.size.x = box.size.x / 2;
        voctLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        voctLabel->text = "V/Oct";
        voctLabel->fontSize = 11;
        voctLabel->color = nvgRGB(200, 200, 200);
        addChild(voctLabel);
        
        // Out label (right jack)
        auto outLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 330));
        outLabel->box.size.x = box.size.x / 2;
        outLabel->alignment = ui::Label::CENTER_ALIGNMENT;
        outLabel->text = "Out";
        outLabel->fontSize = 11;
        outLabel->color = nvgRGB(200, 200, 200);
        addChild(outLabel);
        
        // 2x2 Knob grid - left column at 13.16mm, right column at 26.32mm
        // Top row at 30.48mm (matching LFO freq), bottom row at 55.8mm (matching LFO chaos)
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.16, 30.48)), module, NeoGenModule::SHAPE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(26.32, 30.48)), module, NeoGenModule::ARTIFACT_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.16, 55.8)), module, NeoGenModule::HARMONICS_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(26.32, 55.8)), module, NeoGenModule::DRIVE_PARAM));
        
        // CV inputs - 4 jacks in a row above V/Oct and Audio output
        // Positioned at 95mm Y, spacing approximately 8mm apart
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 95.0)), module, NeoGenModule::SHAPE_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 95.0)), module, NeoGenModule::ARTIFACT_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.0, 95.0)), module, NeoGenModule::HARMONICS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.5, 95.0)), module, NeoGenModule::DRIVE_INPUT));
        
        // V/Oct input and Audio output at bottom (108.41mm matching LFO output position)
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.16, 108.41)), module, NeoGenModule::VOCT_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.32, 108.41)), module, NeoGenModule::AUDIO_OUTPUT));
    }
};

Model* modelNeoGen = createModel<NeoGenModule, NeoGenWidget>("NeoGen");
