#include "plugin.hpp"
#include "NeoFiltreModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelNeoFiltreModule);
}

// Widget for NEOFILTRE module
struct NeoFiltreModuleWidget : ModuleWidget {
	NeoFiltreModuleWidget(NeoFiltreModule* module) {
		setModule(module);
		
		// 4HP module (matching NEOGEN width)
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/NeoFiltreModule.svg"),
			asset::plugin(pluginInstance, "res/NeoFiltreModule-dark.svg")
		));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Title label
		auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
		titleLabel->box.size.x = box.size.x;
		titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		titleLabel->text = "NEOFILTRE";
		titleLabel->fontSize = 14;
		titleLabel->color = nvgRGB(255, 255, 255);
		addChild(titleLabel);

		// Layout: 2x2 grid of knobs with CV inputs below (matching NEOGEN)
		float knobLeftX = 13.16f;
		float knobRightX = 26.32f;
		float knobTopY = 30.48f;
		float knobBottomY = 55.8f;
		
		// COUPURE (top-left)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobTopY)), module, NeoFiltreModule::COUPURE_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0f, 95.0f)), module, NeoFiltreModule::COUPURE_INPUT));
		
		// RÉSONANCE (top-right)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobTopY)), module, NeoFiltreModule::RESONANCE_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5f, 95.0f)), module, NeoFiltreModule::RESONANCE_INPUT));
		
		// GAUSS (bottom-left)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobBottomY)), module, NeoFiltreModule::GAUSS_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.0f, 95.0f)), module, NeoFiltreModule::GAUSS_INPUT));
		
		// SATINE (bottom-right)
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobBottomY)), module, NeoFiltreModule::SATINE_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.5f, 95.0f)), module, NeoFiltreModule::SATINE_INPUT));

		// Audio I/O jacks at bottom
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.16f, 108.41f)), module, NeoFiltreModule::AUDIO_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.32f, 108.41f)), module, NeoFiltreModule::AUDIO_OUTPUT));

		// Labels for knobs
		auto coupureLabel = createWidget<ui::Label>(Vec(2, 106));
		coupureLabel->box.size.x = box.size.x / 2;
		coupureLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		coupureLabel->text = "Coup";
		coupureLabel->fontSize = 11;
		coupureLabel->color = nvgRGB(200, 200, 200);
		addChild(coupureLabel);

		auto resonanceLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 106));
		resonanceLabel->box.size.x = box.size.x / 2;
		resonanceLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		resonanceLabel->text = "Rés";
		resonanceLabel->fontSize = 11;
		resonanceLabel->color = nvgRGB(200, 200, 200);
		addChild(resonanceLabel);

		auto gaussLabel = createWidget<ui::Label>(Vec(2, 181));
		gaussLabel->box.size.x = box.size.x / 2;
		gaussLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		gaussLabel->text = "Gauss";
		gaussLabel->fontSize = 11;
		gaussLabel->color = nvgRGB(200, 200, 200);
		addChild(gaussLabel);

		auto satineLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 181));
		satineLabel->box.size.x = box.size.x / 2;
		satineLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		satineLabel->text = "Satine";
		satineLabel->fontSize = 11;
		satineLabel->color = nvgRGB(200, 200, 200);
		addChild(satineLabel);

		// I/O labels
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

Model* modelNeoFiltreModule = createModel<NeoFiltreModule, NeoFiltreModuleWidget>("NEOFILTRE");
