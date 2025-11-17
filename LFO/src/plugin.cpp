#include "plugin.hpp"
#include "LfoModule.hpp"

using namespace rack;

Plugin *pluginInstance;

	struct LfoModuleWidget : ModuleWidget {
		LfoModuleWidget(LfoModule *module) {
			setModule(module);
			setPanel(createPanel(asset::plugin(pluginInstance, "res/LfoModule.svg")));

			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

			// Title label
			auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
			titleLabel->box.size.x = box.size.x;
			titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			titleLabel->text = "LFO";
			titleLabel->fontSize = 14;
			titleLabel->color = nvgRGB(255, 255, 255);
			addChild(titleLabel);

			// Freq label
			auto freqLabel = createWidget<ui::Label>(Vec(2, 106));
			freqLabel->box.size.x = box.size.x;
			freqLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			freqLabel->text = "Freq";
			freqLabel->fontSize = 11;
			freqLabel->color = nvgRGB(200, 200, 200);
			addChild(freqLabel);

			// Chaos label adjusted: down 3mm, left 1mm from previous
			auto chaosLabel = createWidget<ui::Label>(Vec(5.7, 181));
			chaosLabel->box.size.x = box.size.x;
			chaosLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			chaosLabel->text = "Chaos";
			chaosLabel->fontSize = 11;
			chaosLabel->color = nvgRGB(200, 200, 200);
			addChild(chaosLabel);

			// Gate In label
			auto gateInLabel = createWidget<ui::Label>(Vec(2, 249));
			gateInLabel->box.size.x = box.size.x;
			gateInLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			gateInLabel->text = "Gate In";
			gateInLabel->fontSize = 11;
			gateInLabel->color = nvgRGB(200, 200, 200);
			addChild(gateInLabel);

			// Gate Out label
			auto gateOutLabel = createWidget<ui::Label>(Vec(5, 289));
			gateOutLabel->box.size.x = box.size.x;
			gateOutLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			gateOutLabel->text = "Gate Out";
			gateOutLabel->fontSize = 11;
			gateOutLabel->color = nvgRGB(200, 200, 200);
			addChild(gateOutLabel);

			// Out label
			auto sineLabel = createWidget<ui::Label>(Vec(2, 330));
			sineLabel->box.size.x = box.size.x;
			sineLabel->box.size.y = 20;
			sineLabel->alignment = ui::Label::CENTER_ALIGNMENT;
			sineLabel->text = "Out";
			sineLabel->fontSize = 11;
			sineLabel->color = nvgRGB(200, 200, 200);
			addChild(sineLabel);

			addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 30.48)), module, LfoModule::FREQUENCY_PARAM));
			// Chaos knob moved down 15mm (40.8mm -> 55.8mm) to match shifted SVG circle (cy 120px -> 164px)
			addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 55.8)), module, LfoModule::CHAOS_PARAM));
			// Gate In input (above Gate Out)
			addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 81.0)), module, LfoModule::GATE_INPUT));
			// Gate Out output (above Sine Out)
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 94.7)), module, LfoModule::GATE_OUTPUT));
			// Sine Out output
			addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 108.41)), module, LfoModule::SINE_OUTPUT));
		}
	};Model *modelLfoModule = createModel<LfoModule, LfoModuleWidget>("LfoModule");

void init(Plugin *p) {
	pluginInstance = p;
	p->addModel(modelLfoModule);
}