#pragma once
#include "PluginProcessor.h"
#include <array>
class VirtualCortexEditor final : public juce::AudioProcessorEditor {
public:
 explicit VirtualCortexEditor(VirtualCortexProcessor&);
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 VirtualCortexProcessor& processor;
 struct Knob {juce::Slider slider;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;juce::String label;};
 struct Switch {juce::ToggleButton button;std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;};
 std::array<Knob,18> knobs;
 std::array<Switch,10> switches;
 std::array<std::array<juce::ComboBox,8>,4> gridCells;
 std::array<juce::ToggleButton,4> laneButtons;
 void refreshGridControls();
 void refreshSlotControls();
 void commitSlotControls();
 int selectedRow=0, selectedColumn=0;
 juce::Label slotTitle;
 juce::ComboBox slotSelector;
 std::array<juce::Slider,6> slotSliders;
 std::array<juce::Label,6> slotLabels;
 juce::TextButton loadGridIRButton {"LOAD SELECTED SLOT IR"};
 juce::Label gridIRName;
 juce::TextButton loadNeuralButton {"LOAD AMP MODEL"};
 juce::Label neuralName;
 std::unique_ptr<juce::FileChooser> neuralChooser;
 juce::TextButton loadIRButton {"LOAD CAB IR"};
 juce::TextButton savePresetButton {"SAVE PRESET"};
 juce::TextButton loadPresetButton {"LOAD PRESET"};
 std::unique_ptr<juce::FileChooser> presetChooser;
 juce::Label irName;
 std::unique_ptr<juce::FileChooser> irChooser;
 juce::ComboBox presetBox;
 juce::Label presetLabel;
 void applyPreset(int index);
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VirtualCortexEditor)
};
