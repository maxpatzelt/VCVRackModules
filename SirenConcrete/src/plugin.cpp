#include "plugin.hpp"
#include "SirenConcreteModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;
	p->addModel(modelSirenConcrete);
}

struct SirenConcreteWidget : ModuleWidget {
	SirenConcreteWidget(SirenConcreteModule* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SirenConcreteModule.svg")));
		
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Title label
		auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
		titleLabel->box.size.x = box.size.x;
		titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		titleLabel->text = "SIREN CONCRÈTE";
		titleLabel->fontSize = 11;
		titleLabel->color = nvgRGB(255, 255, 255);
		addChild(titleLabel);
		
		// 2x2 knob grid
		float knobLeftX = 13.16f;
		float knobRightX = 51.36f;
		float knobTopY = 23.88f;
		float knobBottomY = 48.68f;
		
		// Top row
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobTopY)), module, SirenConcreteModule::GRAIN_MORPH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobTopY)), module, SirenConcreteModule::SPECTRAL_SHIFT_PARAM));
		
		// Bottom row
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobBottomY)), module, SirenConcreteModule::PHASE_DRIFT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobBottomY)), module, SirenConcreteModule::ECHO_BLOOM_PARAM));
		
		// Attenuverters below knobs
		float attenTopY = 75.0f;
		float attenBottomY = 85.0f;
		
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, attenTopY)), module, SirenConcreteModule::GRAIN_MORPH_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, attenTopY)), module, SirenConcreteModule::SPECTRAL_SHIFT_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, attenBottomY)), module, SirenConcreteModule::PHASE_DRIFT_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, attenBottomY)), module, SirenConcreteModule::ECHO_BLOOM_ATTEN_PARAM));
		
		// CV inputs
		float cvTopY = 92.0f;
		float cvBottomY = 100.0f;
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(knobLeftX, cvTopY)), module, SirenConcreteModule::GRAIN_MORPH_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(knobRightX, cvTopY)), module, SirenConcreteModule::SPECTRAL_SHIFT_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(knobLeftX, cvBottomY)), module, SirenConcreteModule::PHASE_DRIFT_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(knobRightX, cvBottomY)), module, SirenConcreteModule::ECHO_BLOOM_CV_INPUT));
		
		// V/Oct and output at bottom
		float voct_y = 110.0f;
		float out_y = 118.0f;
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.26f, voct_y)), module, SirenConcreteModule::PITCH_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.26f, out_y)), module, SirenConcreteModule::AUDIO_OUTPUT));
		
		// Labels
		auto grainLabel = createWidget<ui::Label>(Vec(0, 160));
		grainLabel->box.size.x = box.size.x / 2;
		grainLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		grainLabel->text = "Grain";
		grainLabel->fontSize = 10;
		grainLabel->color = nvgRGB(180, 180, 180);
		addChild(grainLabel);
		
		auto spectralLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 160));
		spectralLabel->box.size.x = box.size.x / 2;
		spectralLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		spectralLabel->text = "Spectral";
		spectralLabel->fontSize = 10;
		spectralLabel->color = nvgRGB(180, 180, 180);
		addChild(spectralLabel);
		
		auto phaseLabel = createWidget<ui::Label>(Vec(0, 254));
		phaseLabel->box.size.x = box.size.x / 2;
		phaseLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		phaseLabel->text = "Drift";
		phaseLabel->fontSize = 10;
		phaseLabel->color = nvgRGB(180, 180, 180);
		addChild(phaseLabel);
		
		auto echoLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 254));
		echoLabel->box.size.x = box.size.x / 2;
		echoLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		echoLabel->text = "Echo";
		echoLabel->fontSize = 10;
		echoLabel->color = nvgRGB(180, 180, 180);
		addChild(echoLabel);
		
		auto voctLabel = createWidget<ui::Label>(Vec(0, 342));
		voctLabel->box.size.x = box.size.x;
		voctLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		voctLabel->text = "V/Oct";
		voctLabel->fontSize = 10;
		voctLabel->color = nvgRGB(180, 180, 180);
		addChild(voctLabel);
		
		auto outLabel = createWidget<ui::Label>(Vec(0, 368));
		outLabel->box.size.x = box.size.x;
		outLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		outLabel->text = "Out";
		outLabel->fontSize = 10;
		outLabel->color = nvgRGB(180, 180, 180);
		addChild(outLabel);
	}
};

Model* modelSirenConcrete = createModel<SirenConcreteModule, SirenConcreteWidget>("SirenConcreteModule");
