#include "plugin.hpp"
#include "DiffusaireModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;
	p->addModel(modelDiffusaire);
}

struct DiffusaireWidget : ModuleWidget {
	DiffusaireWidget(DiffusaireModule* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/DiffusaireModule.svg")));
		
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Title label
		auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
		titleLabel->box.size.x = box.size.x;
		titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		titleLabel->text = "DIFFUSAIRE";
		titleLabel->fontSize = 13;
		titleLabel->color = nvgRGB(255, 255, 255);
		addChild(titleLabel);
		
		// 2x2 knob grid (matching NEOFILTRE layout)
		float knobLeftX = 13.16f;
		float knobRightX = 26.32f;
		float knobTopY = 30.48f;
		float knobBottomY = 55.8f;
		
		// Contours (top-left)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobTopY)), module, DiffusaireModule::CONTOURS_PARAM));
		
		// Résonance (top-right)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobTopY)), module, DiffusaireModule::RESONANCE_PARAM));
		
		// Écart (bottom-left)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobBottomY)), module, DiffusaireModule::ECART_PARAM));
		
		// Cinétiques (bottom-right)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobBottomY)), module, DiffusaireModule::CINETIQUES_PARAM));
		
		// Attenuverters (small knobs at Y=75mm and 85mm)
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 75.0f)), module, DiffusaireModule::CONTOURS_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 75.0f)), module, DiffusaireModule::RESONANCE_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, 85.0f)), module, DiffusaireModule::ECART_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, 85.0f)), module, DiffusaireModule::CINETIQUES_ATTEN_PARAM));
		
		// CV inputs (4 jacks in a row at Y=95mm)
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0f, 95.0f)), module, DiffusaireModule::CONTOURS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5f, 95.0f)), module, DiffusaireModule::RESONANCE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.0f, 95.0f)), module, DiffusaireModule::ECART_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.5f, 95.0f)), module, DiffusaireModule::CINETIQUES_INPUT));
		
		// Audio I/O at bottom
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.16f, 108.41f)), module, DiffusaireModule::AUDIO_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.32f, 108.41f)), module, DiffusaireModule::AUDIO_OUTPUT));

		// Labels
		auto contoursLabel = createWidget<ui::Label>(Vec(2, 106));
		contoursLabel->box.size.x = box.size.x / 2;
		contoursLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		contoursLabel->text = "Contours";
		contoursLabel->fontSize = 10;
		contoursLabel->color = nvgRGB(200, 200, 200);
		addChild(contoursLabel);

		auto resLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 106));
		resLabel->box.size.x = box.size.x / 2;
		resLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		resLabel->text = "Rés Var";
		resLabel->fontSize = 10;
		resLabel->color = nvgRGB(200, 200, 200);
		addChild(resLabel);

		auto ecartLabel = createWidget<ui::Label>(Vec(2, 181));
		ecartLabel->box.size.x = box.size.x / 2;
		ecartLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		ecartLabel->text = "Écart";
		ecartLabel->fontSize = 10;
		ecartLabel->color = nvgRGB(200, 200, 200);
		addChild(ecartLabel);

		auto cinetLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 181));
		cinetLabel->box.size.x = box.size.x / 2;
		cinetLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		cinetLabel->text = "Cinét";
		cinetLabel->fontSize = 10;
		cinetLabel->color = nvgRGB(200, 200, 200);
		addChild(cinetLabel);

		auto inLabel = createWidget<ui::Label>(Vec(2, 330));
		inLabel->box.size.x = box.size.x / 2;
		inLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		inLabel->text = "In";
		inLabel->fontSize = 11;
		inLabel->color = nvgRGB(200, 200, 200);
		addChild(inLabel);

		auto outLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 330));
		outLabel->box.size.x = box.size.x / 2;
		outLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		outLabel->text = "Out";
		outLabel->fontSize = 11;
		outLabel->color = nvgRGB(200, 200, 200);
		addChild(outLabel);
	}
};

Model* modelDiffusaire = createModel<DiffusaireModule, DiffusaireWidget>("Diffusaire");
