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
		
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Title label
		auto titleLabel = createWidget<ui::Label>(Vec(0, 15));
		titleLabel->box.size.x = box.size.x;
		titleLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		titleLabel->text = "DUBBOÎTE 2/4";
		titleLabel->fontSize = 11;
		titleLabel->color = nvgRGB(255, 255, 255);
		addChild(titleLabel);
		
		// 2x2 knob grid
		float knobLeftX = 13.16f;
		float knobRightX = 51.36f;
		float knobTopY = 23.88f;
		float knobBottomY = 48.68f;
		
		// Top row
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobTopY)), module, DubBoiteModule::SEND_DIFFUSION_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobTopY)), module, DubBoiteModule::TAPE_SCRUB_PARAM));
		
		// Bottom row
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobLeftX, knobBottomY)), module, DubBoiteModule::LOW_DRIFT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(knobRightX, knobBottomY)), module, DubBoiteModule::SATURATION_BLOOM_PARAM));
		
		// Attenuverters
		float attenTopY = 75.0f;
		float attenBottomY = 85.0f;
		
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, attenTopY)), module, DubBoiteModule::SEND_DIFFUSION_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, attenTopY)), module, DubBoiteModule::TAPE_SCRUB_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobLeftX, attenBottomY)), module, DubBoiteModule::LOW_DRIFT_ATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(knobRightX, attenBottomY)), module, DubBoiteModule::SATURATION_BLOOM_ATTEN_PARAM));
		
		// CV inputs
		float cvTopY = 92.0f;
		float cvBottomY = 100.0f;
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(knobLeftX, cvTopY)), module, DubBoiteModule::SEND_DIFFUSION_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(knobRightX, cvTopY)), module, DubBoiteModule::TAPE_SCRUB_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(knobLeftX, cvBottomY)), module, DubBoiteModule::LOW_DRIFT_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(knobRightX, cvBottomY)), module, DubBoiteModule::SATURATION_BLOOM_CV_INPUT));
		
		// Channel inputs (left column)
		float ch_x_left = 13.16f;
		float ch1_y = 110.0f;
		float ch2_y = 118.0f;
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ch_x_left, ch1_y)), module, DubBoiteModule::CHANNEL_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ch_x_left, ch2_y)), module, DubBoiteModule::CHANNEL_2_INPUT));
		
		// Channel inputs (right column)
		float ch_x_right = 51.36f;
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ch_x_right, ch1_y)), module, DubBoiteModule::CHANNEL_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ch_x_right, ch2_y)), module, DubBoiteModule::CHANNEL_4_INPUT));
		
		// Outputs at bottom center
		float out_y1 = 126.0f;
		float out_y2 = 134.0f;
		
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.26f, out_y1)), module, DubBoiteModule::MIX_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.26f, out_y2)), module, DubBoiteModule::SEND_OUTPUT));
		
		// Labels
		auto sendLabel = createWidget<ui::Label>(Vec(0, 160));
		sendLabel->box.size.x = box.size.x / 2;
		sendLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		sendLabel->text = "Diffusion";
		sendLabel->fontSize = 9;
		sendLabel->color = nvgRGB(180, 180, 180);
		addChild(sendLabel);
		
		auto tapeLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 160));
		tapeLabel->box.size.x = box.size.x / 2;
		tapeLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		tapeLabel->text = "Scrub";
		tapeLabel->fontSize = 9;
		tapeLabel->color = nvgRGB(180, 180, 180);
		addChild(tapeLabel);
		
		auto lowLabel = createWidget<ui::Label>(Vec(0, 254));
		lowLabel->box.size.x = box.size.x / 2;
		lowLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		lowLabel->text = "Low Drift";
		lowLabel->fontSize = 9;
		lowLabel->color = nvgRGB(180, 180, 180);
		addChild(lowLabel);
		
		auto satLabel = createWidget<ui::Label>(Vec(box.size.x / 2, 254));
		satLabel->box.size.x = box.size.x / 2;
		satLabel->alignment = ui::Label::CENTER_ALIGNMENT;
		satLabel->text = "Saturation";
		satLabel->fontSize = 9;
		satLabel->color = nvgRGB(180, 180, 180);
		addChild(satLabel);
	}
};

Model* modelDubBoite = createModel<DubBoiteModule, DubBoiteWidget>("DubBoiteModule");
