#include "plugin.hpp"
#include "OscillanceModule.hpp"

using namespace rack;

Plugin *pluginInstance;

	struct OscillanceModuleWidget : ModuleWidget {
		OscillanceModuleWidget(OscillanceModule *module) {
			setModule(module);
			setPanel(createPanel(asset::plugin(pluginInstance, "res/OscillanceModule.svg")));

			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

			// Title label
			auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
			titleLabel->box.size.x = box.size.x;
			titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			titleLabel->text = "OSCILLANCE";
			titleLabel->fontSize = 12;
			titleLabel->color = nvgRGB(200, 150, 255);
			addChild(titleLabel);

			// Fréquence label (French)
			auto freqLabel = createWidget<ui::Label>(Vec(2, 106));
			freqLabel->box.size.x = box.size.x;
			freqLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			freqLabel->text = "Fréquence";
			freqLabel->fontSize = 10;
			freqLabel->color = nvgRGB(180, 180, 180);
			addChild(freqLabel);

			// Chaos label
			auto chaosLabel = createWidget<ui::Label>(Vec(5.7, 181));
			chaosLabel->box.size.x = box.size.x;
			chaosLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			chaosLabel->text = "Chaos";
			chaosLabel->fontSize = 11;
			chaosLabel->color = nvgRGB(180, 180, 180);
			addChild(chaosLabel);

			// Entrée label (French for input)
			auto gateInLabel = createWidget<ui::Label>(Vec(2, 249));
			gateInLabel->box.size.x = box.size.x;
			gateInLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			gateInLabel->text = "Entrée";
			gateInLabel->fontSize = 11;
			gateInLabel->color = nvgRGB(180, 180, 180);
			addChild(gateInLabel);

			// Porte label (French for gate)
			auto gateOutLabel = createWidget<ui::Label>(Vec(5, 289));
			gateOutLabel->box.size.x = box.size.x;
			gateOutLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			gateOutLabel->text = "Porte";
			gateOutLabel->fontSize = 11;
			gateOutLabel->color = nvgRGB(180, 180, 180);
			addChild(gateOutLabel);

			// Sortie label (French for output)
			auto sineLabel = createWidget<ui::Label>(Vec(2, 330));
			sineLabel->box.size.x = box.size.x;
			sineLabel->box.size.y = 20;
			sineLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			sineLabel->text = "Sortie";
			sineLabel->fontSize = 11;
			sineLabel->color = nvgRGB(180, 180, 180);
			addChild(sineLabel);

			addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 30.48)), module, OscillanceModule::FREQUENCY_PARAM));
			addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 55.8)), module, OscillanceModule::CHAOS_PARAM));
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 81.0)), module, OscillanceModule::GATE_INPUT));
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 94.7)), module, OscillanceModule::GATE_OUTPUT));
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 108.41)), module, OscillanceModule::SINE_OUTPUT));
		}
	};Model *modelOscillanceModule = createModel<OscillanceModule, OscillanceModuleWidget>("OscillanceModule");

void init(Plugin *p) {
	pluginInstance = p;
	p->addModel(modelOscillanceModule);
}