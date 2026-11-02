#include "PluginEditor.h"
VirtualCortexEditor::VirtualCortexEditor(VirtualCortexProcessor& p):AudioProcessorEditor(&p),processor(p){
 const std::array<const char*,18> ids={"gate","drive","tone","level","cab","delayTime","delayFeedback","delayMix","reverbMix","ampBias","inputHP","cabCut","tremRate","tremDepth","outputTrim","eqLow","eqMid","eqHigh"};
 const std::array<const char*,18> labels={"THRESHOLD","DRIVE","TONE","LEVEL","CAB MIX","TIME (MS)","FEEDBACK","DELAY MIX","REVERB MIX","BIAS","INPUT HP","CAB CUT","TREM RATE","TREM DEPTH","TRIM","BASS","MID","TREBLE"};
 for(size_t i=0;i<knobs.size();++i){
  auto& k=knobs[i];k.label=labels[i];
  k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,82,20);
  k.slider.setColour(juce::Slider::rotarySliderFillColourId,juce::Colour::fromRGB(148,115,242));
  k.slider.setColour(juce::Slider::thumbColourId,juce::Colours::white);
  k.slider.setColour(juce::Slider::textBoxTextColourId,juce::Colours::white);
  k.slider.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);
  addAndMakeVisible(k.slider);
  k.attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,ids[i],k.slider);
 }
 const std::array<const char*,10> switchesIds={"gateOn","ampOn","cabOn","delayOn","reverbOn","tremOn","bypass","eqOn","irOn","gridMode"};
 const std::array<const char*,10> switchesLabels={"GATE","AMP","CAB","DELAY","REVERB","TREM","BYPASS","EQ","IR","GRID ENGINE"};
 for(size_t i=0;i<switches.size();++i){
  auto& s=switches[i];s.button.setButtonText(switchesLabels[i]);
  s.button.setColour(juce::ToggleButton::textColourId,juce::Colours::white);
  s.button.setColour(juce::ToggleButton::tickColourId,juce::Colour::fromRGB(169,140,255));
  addAndMakeVisible(s.button);
  s.attachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.parameters,switchesIds[i],s.button);
 }
 presetLabel.setText("FACTORY PRESET",juce::dontSendNotification);
 presetLabel.setColour(juce::Label::textColourId,juce::Colours::white);
 addAndMakeVisible(presetLabel);
 presetBox.addItem("Select preset...",1);presetBox.addItem("Clean ambience",2);
 presetBox.addItem("Crunch rhythm",3);presetBox.addItem("Lead delay",4);
 presetBox.onChange=[this]{if(presetBox.getSelectedId()>1)applyPreset(presetBox.getSelectedId());};
 addAndMakeVisible(presetBox);
 loadIRButton.onClick=[this]{
  irChooser=std::make_unique<juce::FileChooser>("Choose a cabinet impulse response",juce::File{},"*.wav;*.aiff;*.aif");
  irChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
    [this](const juce::FileChooser& chooser){
      const auto file=chooser.getResult();
      if(file.existsAsFile() && processor.loadCabinetIR(file)) irName.setText(file.getFileName(),juce::dontSendNotification);
    });
 };
 loadGridIRButton.onClick=[this]{
  const auto row=static_cast<std::size_t>(selectedRow);
  const auto col=static_cast<std::size_t>(selectedColumn);
  irChooser=std::make_unique<juce::FileChooser>("Choose grid cabinet WAV IR",juce::File{},"*.wav");
  irChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
   [this,row,col](const juce::FileChooser& chooser){
    const auto file=chooser.getResult();
    if(file.existsAsFile() && processor.loadGridCabinetIR(file,row,col))
     gridIRName.setText(processor.getGridCabinetIRName(row,col),juce::dontSendNotification);
    else gridIRName.setText("IR rejected (WAV / 4096 taps max)",juce::dontSendNotification);
   });
 };
 loadNeuralButton.onClick=[this]{
  const auto row=static_cast<std::size_t>(selectedRow);
  const auto col=static_cast<std::size_t>(selectedColumn);
  neuralChooser=std::make_unique<juce::FileChooser>("Choose VCMLP1 amplifier model",juce::File{},"*.vcmlp;*.txt");
  neuralChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
   [this,row,col](const juce::FileChooser& chooser){
    const auto file=chooser.getResult();
    if(file.existsAsFile() && processor.loadGridNeuralModel(file,row,col))
     neuralName.setText(processor.getGridNeuralModelName(row,col),juce::dontSendNotification);
    else neuralName.setText("Select enabled AMP slot / valid VCMLP1",juce::dontSendNotification);
   });
 };
 addAndMakeVisible(loadNeuralButton);
 neuralName.setColour(juce::Label::textColourId,juce::Colours::white);
 addAndMakeVisible(neuralName);
 addAndMakeVisible(loadGridIRButton);
 gridIRName.setColour(juce::Label::textColourId,juce::Colours::white);
 addAndMakeVisible(gridIRName);
 addAndMakeVisible(loadIRButton);
 addAndMakeVisible(savePresetButton);
 savePresetButton.onClick=[this]{
  presetChooser=std::make_unique<juce::FileChooser>("Save Virtual Cortex preset",juce::File{},"*.vcpreset");
  presetChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting,
   [this](const juce::FileChooser& chooser){
    auto file=chooser.getResult();
    if(file==juce::File{})return;
    file=file.withFileExtension(".vcpreset");
    juce::MemoryBlock data;processor.getStateInformation(data);
    file.replaceWithData(data.getData(),data.getSize());
   });
 };
 addAndMakeVisible(loadPresetButton);
 loadPresetButton.onClick=[this]{
  presetChooser=std::make_unique<juce::FileChooser>("Load Virtual Cortex preset",juce::File{},"*.vcpreset");
  presetChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
   [this](const juce::FileChooser& chooser){
    const auto file=chooser.getResult();
    if(!file.existsAsFile() || file.getSize()>1024*1024)return;
    juce::MemoryBlock data;
    if(file.loadFileAsData(data)){
     processor.setStateInformation(data.getData(),static_cast<int>(data.getSize()));
     irName.setText(processor.getCabinetIRName(),juce::dontSendNotification);
    }
   });
 };

 for(std::size_t r=0;r<4;++r){
  auto& lane=laneButtons[r];lane.setButtonText("ROW "+juce::String(static_cast<int>(r+1)));
  lane.setToggleState(processor.isGridLaneEnabled(r),juce::dontSendNotification);
  lane.onClick=[this,r]{processor.setGridLaneEnabled(r,laneButtons[r].getToggleState());};
  addAndMakeVisible(lane);
  for(std::size_t c=0;c<8;++c){
   auto& cell=gridCells[r][c];
   const char* names[]={"Empty","Gate","Amp","Cab","EQ","Delay","Reverb","Tremolo"};
   for(int i=0;i<8;++i)cell.addItem(names[i],i+1);
   cell.onChange=[this,r,c]{selectedRow=static_cast<int>(r);selectedColumn=static_cast<int>(c);
    slotSelector.setSelectedId(1+static_cast<int>(r*8+c),juce::dontSendNotification);
    refreshSlotControls();
    const auto selected=gridCells[r][c].getSelectedId();
    if(selected>0)processor.setGridBlock(r,c,{static_cast<vc::BlockType>(selected-1),true});
   };
   addAndMakeVisible(cell);
  }
 }
 slotTitle.setText("SELECTED GRID SLOT PARAMETERS",juce::dontSendNotification);
 slotTitle.setColour(juce::Label::textColourId,juce::Colours::white);
 addAndMakeVisible(slotTitle);
 for(int r=0;r<4;++r)for(int c=0;c<8;++c)
  slotSelector.addItem("ROW "+juce::String(r+1)+" / SLOT "+juce::String(c+1),1+r*8+c);
 slotSelector.onChange=[this]{const int index=slotSelector.getSelectedId()-1;
  if(index>=0&&index<32){selectedRow=index/8;selectedColumn=index%8;refreshSlotControls();}};
 addAndMakeVisible(slotSelector);
 const char* names[]={"DRIVE","MIX","TIME","FEEDBACK","RATE","DEPTH"};
 for(int i=0;i<6;++i){
  slotLabels[i].setText(names[i],juce::dontSendNotification);
  slotLabels[i].setColour(juce::Label::textColourId,juce::Colours::white);
  addAndMakeVisible(slotLabels[i]);
  slotSliders[i].setRange(0.0,1.0,0.001);
  slotSliders[i].setSliderStyle(juce::Slider::LinearHorizontal);
  slotSliders[i].setTextBoxStyle(juce::Slider::TextBoxRight,false,60,20);
  slotSliders[i].onValueChange=[this]{commitSlotControls();};
  addAndMakeVisible(slotSliders[i]);
 }
 slotSelector.setSelectedId(1,juce::dontSendNotification);
 refreshSlotControls();
 refreshGridControls();
 irName.setText(processor.getCabinetIRName(),juce::dontSendNotification);
 irName.setColour(juce::Label::textColourId,juce::Colours::white);
 addAndMakeVisible(irName);
 setSize(1120,1350);
}
void VirtualCortexEditor::refreshGridControls(){
 const auto grid=processor.getEditableGrid();
 for(std::size_t r=0;r<4;++r)for(std::size_t c=0;c<8;++c)
  gridCells[r][c].setSelectedId(static_cast<int>(grid.get(r,c)->type)+1,juce::dontSendNotification);
}
void VirtualCortexEditor::refreshSlotControls(){
 gridIRName.setText(processor.getGridCabinetIRName(static_cast<std::size_t>(selectedRow),static_cast<std::size_t>(selectedColumn)),juce::dontSendNotification);
 neuralName.setText(processor.getGridNeuralModelName(static_cast<std::size_t>(selectedRow),static_cast<std::size_t>(selectedColumn)),juce::dontSendNotification);
 const auto p=processor.getGridSlotParameters(static_cast<size_t>(selectedRow),static_cast<size_t>(selectedColumn));
 const float v[]={p.drive,p.mix,p.time,p.feedback,p.rate,p.depth};
 for(int i=0;i<6;++i)slotSliders[i].setValue(v[i],juce::dontSendNotification);
}
void VirtualCortexEditor::commitSlotControls(){
 vc::SlotParameters p;
 p.drive=static_cast<float>(slotSliders[0].getValue());
 p.mix=static_cast<float>(slotSliders[1].getValue());
 p.time=static_cast<float>(slotSliders[2].getValue());
 p.feedback=static_cast<float>(slotSliders[3].getValue());
 p.rate=static_cast<float>(slotSliders[4].getValue());
 p.depth=static_cast<float>(slotSliders[5].getValue());
 processor.setGridSlotParameters(static_cast<size_t>(selectedRow),static_cast<size_t>(selectedColumn),p);
}
void VirtualCortexEditor::paint(juce::Graphics& g){
 const auto bg=juce::Colour::fromRGB(15,18,30);
 g.fillAll(bg);
 g.setGradientFill(juce::ColourGradient(juce::Colour::fromRGB(62,43,104),0,0,bg,(float)getWidth(),(float)getHeight(),false));g.fillAll();
 g.setColour(juce::Colours::white);g.setFont(juce::Font(juce::FontOptions(30.f,juce::Font::bold)));
 g.drawText("VIRTUAL CORTEX",28,14,600,45,juce::Justification::centredLeft);
 g.setColour(juce::Colours::lightgrey);g.setFont(14.f);
 g.drawText("v4.9  •  Per-slot parameters  •  Original DSP",28,60,950,25,juce::Justification::centredLeft);
 const std::array<const char*,5> sections={"01  GATE","02  AMP","03  CAB + EQ","04  DELAY","05  REVERB + TREM"};
 const std::array<int,5> xs={28,230,515,690,885};
 const std::array<int,5> widths={190,275,165,185,205};
 g.setColour(juce::Colours::white);g.setFont(16.f);g.drawText("SIGNAL GRID  •  Select effects by cell; enable GRID ENGINE to hear routing",28,145,1000,24,juce::Justification::centredLeft);
 for(size_t i=0;i<sections.size();++i){
  juce::Rectangle<float> rect((float)xs[i],390.f,(float)widths[i],555.f);
  g.setColour(juce::Colour::fromRGB(32,34,51));g.fillRoundedRectangle(rect,14.f);
  g.setColour(juce::Colour::fromRGB(90,75,132));g.drawRoundedRectangle(rect,14.f,1.f);
  g.setColour(juce::Colours::white);g.setFont(16.f);g.drawText(sections[i],xs[i]+12,405,widths[i]-20,28,juce::Justification::centredLeft);
 }
 for(size_t i=0;i<knobs.size();++i){
  const auto r=knobs[i].slider.getBounds();g.setColour(juce::Colours::lightgrey);g.setFont(12.f);
  g.drawText(knobs[i].label,r.getX(),r.getY()-20,r.getWidth(),19,juce::Justification::centred);
 }
 g.setColour(juce::Colours::lightgrey);g.setFont(13.f);
 g.drawText("Standalone: select your audio device from the JUCE audio settings. Use low monitoring volume on first launch.",28,1305,1060,24,juce::Justification::centredLeft);
}
void VirtualCortexEditor::resized(){
 slotTitle.setBounds(30,1040,390,28);
 slotSelector.setBounds(425,1040,220,28);
 for(int i=0;i<6;++i){
  const int row=i/2,col=i%2;
  slotLabels[i].setBounds(30+col*540,1080+row*68,110,26);
  slotSliders[i].setBounds(135+col*540,1080+row*68,365,30);
 }

 for(std::size_t r=0;r<4;++r){
  laneButtons[r].setBounds(28,183+static_cast<int>(r)*43,100,30);
  for(std::size_t c=0;c<8;++c)gridCells[r][c].setBounds(135+static_cast<int>(c)*119,183+static_cast<int>(r)*43,113,30);
 }
 presetLabel.setBounds(30,102,140,26);presetBox.setBounds(170,100,250,30);
 const std::array<juce::Rectangle<int>,18> bounds={
  juce::Rectangle<int>(65,265,110,120),
  {240,265,110,120},{365,265,110,120},{365,410,110,115},
  {535,265,110,120},
  {697,265,85,120},{790,265,85,120},{740,410,85,115},
  {898,265,100,120},
  {240,410,110,115},{65,410,110,115},{535,410,110,115},
  {995,410,85,115},{895,410,85,115},{990,530,85,75},
  {520,540,85,110},{610,540,85,110},{700,540,85,110}
 };
 for(size_t i=0;i<knobs.size();++i)knobs[i].slider.setBounds(bounds[i].translated(0,235));
 const std::array<juce::Rectangle<int>,10> switchesBounds={
  juce::Rectangle<int>(48,215,145,32),{248,215,115,32},{535,215,115,32},
  {700,215,115,32},{898,215,125,32},{900,555,100,32},{950,25,140,38},{540,670,100,32},{640,670,100,32},{910,665,160,32}
 };
 for(size_t i=0;i<switches.size();++i)switches[i].button.setBounds(i==6?switchesBounds[i]:switchesBounds[i].translated(0,235));
 savePresetButton.setBounds(440,100,135,30);loadPresetButton.setBounds(585,100,135,30);
 loadNeuralButton.setBounds(760,1095,245,32);neuralName.setBounds(760,1129,300,24);
 loadGridIRButton.setBounds(760,1025,245,32);gridIRName.setBounds(760,1059,300,24);
 loadIRButton.setBounds(760,900,145,34);irName.setBounds(760,937,290,26);
}
void VirtualCortexEditor::applyPreset(int index){
 auto set=[this](const char* id,float value){
  if(auto* p=processor.parameters.getParameter(id))
   p->setValueNotifyingHost(p->convertTo0to1(value));
 };
 // Factory presets set parameters through the host-aware API, so changes remain automatable.
 if(index==2){set("drive",0.12f);set("tone",0.7f);set("level",0.65f);set("delayMix",0.12f);set("reverbMix",0.25f);set("ampBias",0.f);set("delayOn",1.f);set("reverbOn",1.f);}
 if(index==3){set("drive",0.48f);set("tone",0.56f);set("level",0.48f);set("delayMix",0.f);set("reverbMix",0.04f);set("ampBias",0.08f);set("delayOn",0.f);set("reverbOn",0.f);}
 if(index==4){set("drive",0.73f);set("tone",0.62f);set("level",0.38f);set("delayTime",390.f);set("delayFeedback",0.32f);set("delayMix",0.22f);set("reverbMix",0.15f);set("delayOn",1.f);set("reverbOn",1.f);}
}
juce::AudioProcessorEditor* VirtualCortexProcessor::createEditor(){return new VirtualCortexEditor(*this);}
