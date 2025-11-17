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
		
		// Main knobs (2x2 grid) - matching Diffusaire 4HP layout
		float knobLeftX = 13.16f;
		float knobRightX = 26.32f;
		float knobTopY = 30.48f;
		float knobBottomY = 55.8f;
		
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobTopY)), module, SirenConcreteModule::GRAIN_MORPH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobTopY)), module, SirenConcreteModule::SPECTRAL_SHIFT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobBottomY)), module, SirenConcreteModule::PHASE_DRIFT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobBottomY)), module, SirenConcreteModule::ECHO_BLOOM_PARAM));
		
		// Attenuverters (small knobs at Y=75mm and 85mm)
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 75.0f)), module, SirenConcreteModule::GRAIN_MORPH_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 75.0f)), module, SirenConcreteModule::SPECTRAL_SHIFT_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 85.0f)), module, SirenConcreteModule::PHASE_DRIFT_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 85.0f)), module, SirenConcreteModule::ECHO_BLOOM_ATTEN_PARAM));
		
		// CV inputs (4 jacks in a row at Y=95mm)
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0f, 95.0f)), module, SirenConcreteModule::GRAIN_MORPH_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5f, 95.0f)), module, SirenConcreteModule::SPECTRAL_SHIFT_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.0f, 95.0f)), module, SirenConcreteModule::PHASE_DRIFT_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.5f, 95.0f)), module, SirenConcreteModule::ECHO_BLOOM_CV_INPUT));
		
		// Trigger input and audio output (bottom)
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.16f, 108.41f)), module, SirenConcreteModule::PITCH_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.32f, 108.41f)), module, SirenConcreteModule::AUDIO_OUTPUT));

		// Labels
		auto sweepLabel = createWidget<ui::Label>(Vec(2, 106));
		sweepLabel->box.size.x = box.size.x / 2;
		sweepLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		sweepLabel->text = "Sweep";
		sweepLabel->fontSize = 10;
		sweepLabel->color = nvgRGB(200, 200, 200);
		addChild(sweepLabel);

		auto grainLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 106));
		grainLabel->box.size.x = box.size.x / 2;
		grainLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		grainLabel->text = "Grain";
		grainLabel->fontSize = 10;
		grainLabel->color = nvgRGB(200, 200, 200);
		addChild(grainLabel);

		auto decayLabel = createWidget<ui::Label>(Vec(2, 181));
		decayLabel->box.size.x = box.size.x / 2;
		decayLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		decayLabel->text = "Decay";
		decayLabel->fontSize = 10;
		decayLabel->color = nvgRGB(200, 200, 200);
		addChild(decayLabel);

		auto textureLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 181));
		textureLabel->box.size.x = box.size.x / 2;
		textureLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		textureLabel->text = "Texture";
		textureLabel->fontSize = 10;
		textureLabel->color = nvgRGB(200, 200, 200);
		addChild(textureLabel);

		auto trigLabel = createWidget<ui::Label>(Vec(2, 330));
		trigLabel->box.size.x = box.size.x / 2;
		trigLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		trigLabel->text = "Trig";
		trigLabel->fontSize = 11;
		trigLabel->color = nvgRGB(200, 200, 200);
		addChild(trigLabel);

		auto outLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 330));
		outLabel->box.size.x = box.size.x / 2;
		outLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		outLabel->text = "Out";
		outLabel->fontSize = 11;
		outLabel->color = nvgRGB(200, 200, 200);
		addChild(outLabel);
	}
};

Model* modelSirenConcrete = createModel<SirenConcreteModule, SirenConcreteWidget>("SirenConcrete");
