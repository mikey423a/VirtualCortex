#pragma once
#include "PluginProcessor.h"
#include <array>

class CortexLookAndFeel final : public juce::LookAndFeel_V4 {
public:
 CortexLookAndFeel();
 void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&) override;
 void drawButtonText(juce::Graphics&,juce::TextButton&,bool,bool) override;
 void drawToggleButton(juce::Graphics&,juce::ToggleButton&,bool,bool) override;
 void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool) override;
};

class CortexBlockButton final : public juce::Button {
public:
 CortexBlockButton();
 void paintButton(juce::Graphics&,bool,bool) override;
 int type=0;
 bool selected=false, blockEnabled=true, stomp=false, rail=false;
 juce::String caption;
 static juce::Colour colourFor(int);
 static juce::String nameFor(int);
 static void drawIcon(juce::Graphics&,juce::Rectangle<float>,int,juce::Colour);
};

class VirtualCortexEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
 explicit VirtualCortexEditor(VirtualCortexProcessor&);
 ~VirtualCortexEditor() override;
 void paint(juce::Graphics&) override;
 void resized() override;
 // Used by the native UI review to exercise real selection and insertion.
 void selectSlot(int row,int col);
 void setEditorsVisible(bool visible);
 bool insertBlock(int type);
 bool toggleSelectedSlot();
private:
 VirtualCortexProcessor& processor;
 CortexLookAndFeel look;
 struct Knob {juce::Slider slider;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;juce::Label value;juce::String label;};
 struct Switch {juce::ToggleButton button;std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;};
 std::array<Knob,18> knobs;
 std::array<Switch,10> switches;
 std::array<std::array<CortexBlockButton,8>,4> gridCells;
 std::array<juce::TextButton,4> laneButtons;
 std::array<CortexBlockButton,8> stomps;
 std::array<CortexBlockButton,16> browserButtons;
 std::array<juce::TextButton,8> deviceEntries,quickSelectors;
 std::array<juce::TextButton,2> bankButtons;
 std::array<juce::Label,6> slotValues;
 juce::TextButton headerStompButton{""},previousPreset{"<"},nextPreset{">"},directoryButton{"DIRECTORY"},ioButton{"I/O"},stompModeButton{"STOMP"},presetMenuButton{"PRESET"};
 juce::TextButton editorsButton{"SHOW EDITORS"};
 juce::ComboBox rowSelector;
 std::array<juce::Slider,6> slotSliders;
 std::array<juce::String,6> slotLabels{{"DRIVE","MIX","TIME","FEEDBACK","RATE","DEPTH"}};
 juce::TextButton addButton{"ADD TO SLOT"},removeButton{"REMOVE"};
 juce::ToggleButton slotEnabled{"ENABLED"};
 juce::TextButton loadGridIRButton{"LOAD SLOT IR"},loadNeuralButton{"LOAD AMP MODEL"};
 juce::TextButton loadIRButton{"LOAD CAB IR"},savePresetButton{"SAVE"},loadPresetButton{"OPEN"};
 std::unique_ptr<juce::FileChooser> irChooser,neuralChooser,presetChooser;
 juce::Label irName,gridIRName,neuralName,statusLabel;
 juce::ComboBox presetBox;
 int selectedRow=0,selectedColumn=1,browserType=2,lastView=-1,rowBank=0,variant=0;
 float designScale=1.f,designOffsetX=0.f,designOffsetY=0.f;
 juce::StringArray deviceNames() const;
 void refreshDeviceList();
 void applyDeviceVariant(int);
 bool lastGridMode=false,masterPage=false,editorsVisible=false;
 juce::Rectangle<int> browserArea,gridArea,parameterArea,stompArea;
 bool gridMode() const;
 int currentType() const;
 std::vector<int> visibleKnobs() const;
 std::vector<int> visibleSlotControls() const;
 void timerCallback() override;
 void refreshGridControls();
 void refreshSlotControls();
 void commitSlotControls();
 void updateView();
 void applyPreset(int);
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VirtualCortexEditor)
};
