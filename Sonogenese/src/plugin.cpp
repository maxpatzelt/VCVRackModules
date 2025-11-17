#include "plugin.hpp"
#include "SonogeneseModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;
	p->addModel(modelSonogenese);
}

struct SonogeneseWidget : ModuleWidget {
	SonogeneseWidget(SonogeneseModule* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SonogeneseModule.svg")));
		
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Title label
		auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
		titleLabel->box.size.x = box.size.x;
		titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		titleLabel->text = "SONOGENESE";
		titleLabel->fontSize = 13;
		titleLabel->color = nvgRGB(255, 255, 255);
		addChild(titleLabel);
		
		// 2x2 knob grid (matching NEOGEN layout)
		float knobLeftX = 13.16f;
		float knobRightX = 26.32f;
		float knobTopY = 30.48f;
		float knobBottomY = 55.8f;
		
		// Fragmentation (top-left)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobTopY)), module, SonogeneseModule::FRAGMENTATION_PARAM));
		
		// Topology (top-right)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobTopY)), module, SonogeneseModule::TOPOLOGY_PARAM));
		
		// Skew (bottom-left)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobBottomY)), module, SonogeneseModule::SKEW_PARAM));
		
		// Bloom (bottom-right)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobBottomY)), module, SonogeneseModule::BLOOM_PARAM));
		
		// Attenuverters (small knobs below main knobs at Y=75mm)
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 75.0f)), module, SonogeneseModule::FRAGMENTATION_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 75.0f)), module, SonogeneseModule::TOPOLOGY_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 85.0f)), module, SonogeneseModule::SKEW_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 85.0f)), module, SonogeneseModule::BLOOM_ATTEN_PARAM));
		
		// CV inputs (4 jacks in a row at Y=95mm)
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0f, 95.0f)), module, SonogeneseModule::FRAGMENTATION_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5f, 95.0f)), module, SonogeneseModule::TOPOLOGY_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.0f, 95.0f)), module, SonogeneseModule::SKEW_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.5f, 95.0f)), module, SonogeneseModule::BLOOM_INPUT));
		
		// V/Oct input and Audio output at bottom
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.16f, 108.41f)), module, SonogeneseModule::VOCT_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.32f, 108.41f)), module, SonogeneseModule::AUDIO_OUTPUT));

		// Labels
		auto fragLabel = createWidget<ui::Label>(Vec(2, 106));
		fragLabel->box.size.x = box.size.x / 2;
		fragLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		fragLabel->text = "Frag";
		fragLabel->fontSize = 11;
		fragLabel->color = nvgRGB(200, 200, 200);
		addChild(fragLabel);

		auto topoLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 106));
		topoLabel->box.size.x = box.size.x / 2;
		topoLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		topoLabel->text = "Topologie";
		topoLabel->fontSize = 10;
		topoLabel->color = nvgRGB(200, 200, 200);
		addChild(topoLabel);

		auto skewLabel = createWidget<ui::Label>(Vec(2, 181));
		skewLabel->box.size.x = box.size.x / 2;
		skewLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		skewLabel->text = "Écart";
		skewLabel->fontSize = 11;
		skewLabel->color = nvgRGB(200, 200, 200);
		addChild(skewLabel);

		auto bloomLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 181));
		bloomLabel->box.size.x = box.size.x / 2;
		bloomLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		bloomLabel->text = "Floraison";
		bloomLabel->fontSize = 10;
		bloomLabel->color = nvgRGB(200, 200, 200);
		addChild(bloomLabel);

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

Model* modelSonogenese = createModel<SonogeneseModule, SonogeneseWidget>("Sonogenese");
