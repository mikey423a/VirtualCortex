#include "PluginEditor.h"
#include <cmath>

namespace {
const juce::Colour background(0xff090a0b), panel(0xff1c1c1c), muted(0xff8a8e93), mint(0xff61fbd8);
void drawCaption(juce::Graphics& g,const juce::String& s,juce::Rectangle<int> r,float size,juce::Colour c=juce::Colours::white,int align=juce::Justification::centredLeft,bool bold=false){
 g.setColour(c);g.setFont(juce::Font(juce::FontOptions(size,bold?juce::Font::bold:juce::Font::plain)));g.drawFittedText(s,r,align,1);
}
}
CortexLookAndFeel::CortexLookAndFeel(){
 setColour(juce::ResizableWindow::backgroundColourId,background);
 setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff202123));
 setColour(juce::ComboBox::outlineColourId,juce::Colour(0xff36383b));
 setColour(juce::ComboBox::textColourId,juce::Colours::white);
 setColour(juce::PopupMenu::backgroundColourId,panel);
 setColour(juce::PopupMenu::highlightedBackgroundColourId,juce::Colour(0xff28483f));
 setColour(juce::ToggleButton::textColourId,juce::Colours::white);
 setColour(juce::ToggleButton::tickColourId,mint);
 setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);
 setColour(juce::Slider::textBoxTextColourId,juce::Colours::white);
 setColour(juce::Slider::textBoxBackgroundColourId,juce::Colours::transparentBlack);
}
void CortexLookAndFeel::drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour&,bool over,bool){
 const auto id=b.getComponentID();auto r=b.getLocalBounds().toFloat();
 if(id=="directory"||id=="edit-menu"){return;}
 if(id.startsWith("device-")){if(b.getToggleState()||over){g.setColour(juce::Colour(0xff111212));g.fillRoundedRectangle(r,6.f);}return;}
 g.setColour(juce::Colour(b.getToggleState()&&id.startsWith("quick-")?0xffffd545:(over?0xff292929:0xff1d1d1d)));g.fillRoundedRectangle(r.reduced(.5f),id.startsWith("lane-")?16.f:5.f);
}
void CortexLookAndFeel::drawButtonText(juce::Graphics& g,juce::TextButton& b,bool,bool){
 const auto id=b.getComponentID();auto r=b.getLocalBounds();
 if(id=="pedal"){g.setColour(juce::Colours::white);juce::Path foot;foot.startNewSubPath(20,23);foot.lineTo(25,15);foot.lineTo(35,12);foot.lineTo(38,7);foot.lineTo(43,10);foot.lineTo(45,18);foot.lineTo(36,22);foot.lineTo(33,26);foot.lineTo(20,26);foot.closeSubPath();g.strokePath(foot,juce::PathStrokeType(1.f));g.drawLine(17,29,46,29,1.f);return;}
 if(id=="save"){g.setColour(muted);auto box=r.toFloat().reduced(9.f);g.drawRect(box,1.5f);g.drawRect(box.reduced(4.f,0).withHeight(7.f),1.f);g.drawRect(box.reduced(4.f,0).withY(box.getBottom()-8).withHeight(8.f),1.f);return;}
 if(id=="open"){drawCaption(g,"...",r,21.f,muted,juce::Justification::centred);return;}
 const auto c=b.getToggleState()&&id.startsWith("quick-")?juce::Colours::black:(id.startsWith("device-")&&b.getToggleState()?juce::Colour(0xff8bf879):(id=="directory"?muted:juce::Colours::white));
 drawCaption(g,b.getButtonText(),id.startsWith("device-")?r.reduced(9,0):r,id.startsWith("quick-")?16.f:(id.startsWith("device-")?15.f:14.f),c,id.startsWith("device-")?juce::Justification::centredLeft:juce::Justification::centred);
}
void CortexLookAndFeel::drawToggleButton(juce::Graphics& g,juce::ToggleButton& b,bool over,bool){
 if(b.getComponentID()=="power"||b.getComponentID()=="bypass"){
  const auto r=b.getLocalBounds().toFloat().reduced(8.f);const auto c=b.getToggleState()?mint:muted;
  g.setColour(c);juce::Path arc;arc.addCentredArc(r.getCentreX(),r.getCentreY(),r.getWidth()*.4f,r.getWidth()*.4f,0,.55f,5.73f,true);g.strokePath(arc,juce::PathStrokeType(1.4f));g.drawLine(r.getCentreX(),r.getY()+1,r.getCentreX(),r.getCentreY(),1.4f);return;
 }
 juce::LookAndFeel_V4::drawToggleButton(g,b,over,false);
}
void CortexLookAndFeel::drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float start,float end,juce::Slider& s){
 const float size=juce::jmin((float)w,(float)h)-10.f;
 const auto r=juce::Rectangle<float>(size,size).withCentre({x+w*.5f,y+h*.5f});
 const float radius=size*.5f,angle=start+pos*(end-start);
 g.setColour(juce::Colour(0xff080909));g.fillEllipse(r.expanded(2.f));
 g.setColour(juce::Colour(0xff26282a));g.fillEllipse(r.reduced(7.f));
 juce::Path track;track.addCentredArc(r.getCentreX(),r.getCentreY(),radius,radius,0,start,end,true);
 g.setColour(juce::Colour(0xff35383b));g.strokePath(track,juce::PathStrokeType(3.f));
 juce::Path arc;arc.addCentredArc(r.getCentreX(),r.getCentreY(),radius,radius,0,start,angle,true);
 g.setColour(s.findColour(juce::Slider::rotarySliderFillColourId));g.strokePath(arc,juce::PathStrokeType(3.f));
 juce::Path pointer;pointer.startNewSubPath(0,-radius+12.f);pointer.lineTo(0,-radius*.24f);
 g.setColour(juce::Colours::white);g.strokePath(pointer,juce::PathStrokeType(2.f),juce::AffineTransform::rotation(angle).translated(r.getCentreX(),r.getCentreY()));
}
CortexBlockButton::CortexBlockButton():juce::Button("Signal slot"){}
juce::Colour CortexBlockButton::colourFor(int type){
 const juce::uint32 values[]={0xff41454a,0xffd8a944,0xfff25562,0xff57d3bf,0xff4097ef,0xff43d8c9,0xff61fbd8,0xff9265ff,0xffe6ad55,0xffee8641,0xffe6546d,0xffa0a7ad};
 return juce::Colour(values[juce::jlimit(0,11,type)]);
}
juce::String CortexBlockButton::nameFor(int type){
 const char* names[]={"Empty","Gate","Amp","Cabinet","EQ","Delay","Reverb","Tremolo","Compressor","Overdrive","Distortion","Output"};
 return names[juce::jlimit(0,11,type)];
}
void CortexBlockButton::drawIcon(juce::Graphics& g,juce::Rectangle<float> r,int type,juce::Colour colour){
 g.setColour(colour);const auto cx=r.getCentreX(),cy=r.getCentreY();const auto u=r.getWidth()/24.f;
 juce::Path p;
 if(type==0){g.drawLine(cx-5*u,cy,cx+5*u,cy,1.2f);g.drawLine(cx,cy-5*u,cx,cy+5*u,1.2f);return;}
 if(type==8){g.drawLine(cx-10*u,cy+7*u,cx-3*u,cy-6*u,1.5f);g.drawLine(cx-3*u,cy-6*u,cx+10*u,cy-9*u,1.5f);return;}
 if(type==9||type==10){g.drawRoundedRectangle(r.reduced(3*u,1*u),2*u,1.4f);g.drawLine(cx-5*u,cy,cx+5*u,cy,1.2f);g.fillEllipse(cx-2*u,cy+5*u,4*u,4*u);return;}
 if(type==2){g.drawRoundedRectangle(r.reduced(2*u,5*u),1.f,1.4f);g.drawLine(r.getX()+5*u,cy,r.getRight()-5*u,cy,1.2f);return;}
 if(type==3){g.drawEllipse(cx-8*u,cy-8*u,16*u,16*u,1.4f);g.drawEllipse(cx-3*u,cy-3*u,6*u,6*u,1.2f);return;}
 if(type==4){for(int i=0;i<3;++i){auto xx=cx+(i-1)*7*u;auto yy=cy+(i==1?3.f:-3.f)*u;g.drawLine(xx,cy-9*u,xx,cy+9*u,1.2f);g.fillRoundedRectangle(xx-2*u,yy-2*u,4*u,4*u,1.f);}return;}
 if(type==5){for(int wave=0;wave<3;++wave){juce::Path line;for(int i=0;i<=40;++i){float t=i/40.f;float xx=cx-9*u+6*u*t+wave*6*u;float yy=cy+std::sin(t*6.2831853f)*10*u;if(i==0)line.startNewSubPath(xx,yy);else line.lineTo(xx,yy);}g.strokePath(line,juce::PathStrokeType(1.f));}return;}
 if(type==6){p.startNewSubPath(cx-8*u,cy-4*u);p.lineTo(cx+2*u,cy-4*u);p.lineTo(cx+2*u,cy+7*u);p.lineTo(cx-8*u,cy+7*u);p.closeSubPath();p.startNewSubPath(cx-8*u,cy-4*u);p.lineTo(cx-2*u,cy-10*u);p.lineTo(cx+8*u,cy-10*u);p.lineTo(cx+8*u,cy+1*u);p.lineTo(cx+2*u,cy+7*u);p.startNewSubPath(cx+2*u,cy-4*u);p.lineTo(cx+8*u,cy-10*u);g.strokePath(p,juce::PathStrokeType(1.2f));return;}
 p.startNewSubPath(r.getX(),cy);
 for(int i=1;i<=40;++i){float t=i/40.f;float yy=type==1?(t<.45f?-.55f:.55f):std::sin(t*6.2831853f*(type==7?1.f:2.f))*.7f;p.lineTo(r.getX()+t*r.getWidth(),cy+yy*r.getHeight()*.45f);}
 g.strokePath(p,juce::PathStrokeType(1.5f));
}
void CortexBlockButton::paintButton(juce::Graphics& g,bool over,bool){
 const auto bounds=getLocalBounds().toFloat();const auto colour=colourFor(type);
 if(rail){
  if(selected){g.setColour(mint);g.fillRect(0,12,2,getHeight()-24);}
  drawIcon(g,{bounds.getCentreX()-12,bounds.getCentreY()-12,24,24},type,selected?juce::Colours::white:juce::Colour(0xff66696b));return;
 }
 if(stomp){
  const juce::uint32 tileColours[]={0xff292929,0xff241a12,0xff241a12,0xff162522,0xff131a22,0xff162522,0xff292929,0xff131a22};
  g.setColour(selected?mint:juce::Colour(tileColours[juce::jlimit(0,7,type)]));g.fillRect(bounds);
  g.setColour(juce::Colour(0xff050505));g.drawRect(bounds,1.f);
  const auto ink=selected?juce::Colours::black:juce::Colours::white;
  drawCaption(g,caption,{getWidth()-27,6,23,25},20.f,selected?juce::Colour(0xff316f63):muted,juce::Justification::centred);
  auto box=juce::Rectangle<float>(48,48).withCentre({bounds.getCentreX(),43.f});g.setColour(selected?juce::Colour(0xff073d34):colour);g.drawRoundedRectangle(box,12.f,selected?3.f:1.5f);drawIcon(g,box.reduced(10),type,ink);
  drawCaption(g,type?nameFor(type):"Empty slot",{8,75,getWidth()-16,31},16.f,ink,juce::Justification::centred);return;
 }
 if(type==0&&!selected&&!over)return;
 const float side=juce::jmin(62.f,bounds.getHeight()-16.f);auto box=juce::Rectangle<float>(side,side).withCentre({bounds.getCentreX(),bounds.getCentreY()});
 g.setColour(selected?colour.withAlpha(.13f):juce::Colour(0xff101111));g.fillRoundedRectangle(box,16.f);
 g.setColour(blockEnabled?colour:colour.withAlpha(.3f));g.drawRoundedRectangle(box,16.f,selected?5.f:1.7f);
 drawIcon(g,box.reduced(side*.24f),type,blockEnabled?juce::Colours::white:muted.withAlpha(.55f));
 if(type){const auto badge=juce::Rectangle<float>(side*.38f,side*.38f).withCentre({box.getX()-1,box.getY()-1});g.setColour(juce::Colours::white);g.fillEllipse(badge);drawCaption(g,caption,badge.toNearestInt(),side*.24f,juce::Colours::black,juce::Justification::centred);}
}
VirtualCortexEditor::VirtualCortexEditor(VirtualCortexProcessor& p):AudioProcessorEditor(&p),processor(p){
 setLookAndFeel(&look);
 const std::array<const char*,18> ids={"gate","drive","tone","level","cab","delayTime","delayFeedback","delayMix","reverbMix","ampBias","inputHP","cabCut","tremRate","tremDepth","outputTrim","eqLow","eqMid","eqHigh"};
 const std::array<const char*,18> labels={"THRESHOLD","DRIVE","TONE","LEVEL","CAB MIX","TIME","FEEDBACK","MIX","MIX","BIAS","INPUT HIGH PASS","LOW PASS","RATE","DEPTH","OUTPUT TRIM","BASS","MID","TREBLE"};
 for(size_t i=0;i<knobs.size();++i){auto& k=knobs[i];k.label=labels[i];k.slider.setComponentID(ids[i]);k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);k.slider.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);addChildComponent(k.slider);k.attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,ids[i],k.slider);
  k.slider.valueFromTextFunction=[i](const juce::String& text){double v=text.getDoubleValue();return (i==0||i>=15||i==5||i==10||i==11||i==12||i==9)?v:v/100.;};
  k.slider.textFromValueFunction=[i](double v){if(i==0||i>=15)return juce::String(v,1)+" dB";if(i==5)return juce::String(v,0)+" ms";if(i==10||i==11)return juce::String(v,0)+" Hz";if(i==12)return juce::String(v,2)+" Hz";if(i==9)return juce::String(v,2);return juce::String(v*100,1)+" %";};
  k.value.setComponentID(juce::String(ids[i])+"Value");k.value.setFont(juce::Font(juce::FontOptions(15.f)));k.value.setBorderSize(juce::BorderSize<int>(0));k.value.setColour(juce::Label::textColourId,juce::Colours::white);k.value.setEditable(false,true,false);addChildComponent(k.value);
  k.slider.onValueChange=[this,i]{auto& item=knobs[i];if(!item.value.isBeingEdited())item.value.setText(item.slider.getTextFromValue(item.slider.getValue()),juce::dontSendNotification);};
  k.value.onTextChange=[this,i]{auto& item=knobs[i];item.slider.setValue(item.slider.getValueFromText(item.value.getText()),juce::sendNotificationSync);};
  k.value.setText(k.slider.getTextFromValue(k.slider.getValue()),juce::dontSendNotification);
 }
 const std::array<const char*,10> switchesIds={"gateOn","ampOn","cabOn","delayOn","reverbOn","tremOn","bypass","eqOn","irOn","gridMode"};
 const std::array<const char*,10> switchesLabels={"ENABLED","ENABLED","ENABLED","ENABLED","ENABLED","ENABLED","BYPASS","ENABLED","USE IR","GRID ENGINE"};
 for(size_t i=0;i<switches.size();++i){auto& s=switches[i];s.button.setButtonText(switchesLabels[i]);addChildComponent(s.button);s.attachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.parameters,switchesIds[i],s.button);}
 presetBox.addItem("Clean ambience",2);presetBox.addItem("Crunch rhythm",3);presetBox.addItem("Lead delay",4);presetBox.setTextWhenNothingSelected("1A Virtual Cortex MAIN");presetBox.onChange=[this]{applyPreset(presetBox.getSelectedId());};addAndMakeVisible(presetBox);
 for(auto* b:{&savePresetButton,&loadPresetButton,&addButton,&removeButton,&loadIRButton,&loadGridIRButton,&loadNeuralButton})addAndMakeVisible(*b);
 for(auto* l:{&irName,&gridIRName,&neuralName,&statusLabel}){l->setColour(juce::Label::textColourId,muted);l->setFont(juce::Font(juce::FontOptions(11.f)));addAndMakeVisible(*l);}
 for(int i=0;i<11;++i){auto& b=browserButtons[(size_t)i];b.rail=true;b.type=i+1;b.setTooltip(CortexBlockButton::nameFor(i+1));b.onClick=[this,i]{browserType=i+1;variant=0;masterPage=(i==10);lastView=-1;updateView();};addAndMakeVisible(b);}
 addButton.onClick=[this]{insertBlock(browserType);};removeButton.setButtonText("");removeButton.setComponentID("edit-menu");removeButton.setTooltip("Selected block options");
 removeButton.onClick=[this]{juce::PopupMenu menu;menu.addItem(1,"Remove selected block");menu.addItem(2,"Toggle selected block bypass");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&removeButton),[safe](int item){if(!safe)return;if(item==1)safe->insertBlock(0);if(item==2)safe->toggleSelectedSlot();});};
 slotEnabled.onClick=[this]{toggleSelectedSlot();};addAndMakeVisible(slotEnabled);
 for(int r=0;r<4;++r){auto& lane=laneButtons[(size_t)r];lane.setButtonText("ROW "+juce::String(r+1));lane.setComponentID("lane-"+juce::String(r));lane.setTooltip("Enable parallel row: same input, averaged with other enabled rows in Grid mode");lane.setClickingTogglesState(true);lane.onClick=[this,r]{processor.setGridLaneEnabled((size_t)r,laneButtons[(size_t)r].getToggleState());refreshGridControls();};addAndMakeVisible(lane);
  for(int c=0;c<8;++c){auto& b=gridCells[(size_t)r][(size_t)c];b.setComponentID("grid-"+juce::String(r)+"-"+juce::String(c));b.onClick=[this,r,c]{selectSlot(r,c);};addAndMakeVisible(b);}
 }
 for(int c=0;c<8;++c){auto& b=stomps[(size_t)c];b.stomp=true;b.caption=juce::String::charToString((juce::juce_wchar)('A'+c));b.onClick=[this,c]{selectSlot(selectedRow,c);toggleSelectedSlot();};addAndMakeVisible(b);}
 for(int i=0;i<6;++i){auto& s=slotSliders[(size_t)i];s.setRange(0.,1.,.001);s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);s.setComponentID("slot-param-"+juce::String(i));s.valueFromTextFunction=[this,i](const juce::String& text){double v=text.getDoubleValue();
   if(currentType()==8){if(i==0)return (v+48.)/48.;if(i==3)return (v-1.)/19.;if(i==4)return (v-.1)/99.9;if(i==5)return (v-10.)/990.;if(i==2)return v/24.;return v/100.;}
   if(currentType()==9||currentType()==10){if(i==0)return v/36.;if(i==4)return (v-500.)/9500.;if(i==5)return v/200.;return v/100.;}if(i==2)return (v-20.)/480.;if(i==4)return (v-.1)/9.9;return v/(i==3?90.:100.);};s.textFromValueFunction=[this,i](double v){
   if(currentType()==8){if(i==0)return juce::String(-48.+48.*v,1)+" dB";if(i==3)return juce::String(1.+19.*v,1)+":1";if(i==4)return juce::String(.1+99.9*v,1)+" ms";if(i==5)return juce::String(10.+990.*v,0)+" ms";if(i==2)return juce::String(24.*v,1)+" dB";return juce::String(v*100.,1)+" %";}
   if(currentType()==9||currentType()==10){if(i==0)return juce::String(v*36.,1)+" dB";if(i==4)return juce::String(500.+9500.*v,0)+" Hz";if(i==5)return juce::String(v*200.,1)+" %";return juce::String(v*100.,1)+" %";}if(i==2)return juce::String(20.+480.*v,0)+" ms";if(i==4)return juce::String(.1+9.9*v,2)+" Hz";return juce::String(v*(i==3?90.:100.),1)+" %";};s.onValueChange=[this]{commitSlotControls();};addChildComponent(s);
  auto& value=slotValues[(size_t)i];value.setFont(juce::Font(juce::FontOptions(15.f)));value.setBorderSize(juce::BorderSize<int>(0));value.setColour(juce::Label::textColourId,juce::Colours::white);value.setEditable(false,true,false);value.onTextChange=[this,i]{slotSliders[(size_t)i].setValue(slotSliders[(size_t)i].getValueFromText(slotValues[(size_t)i].getText()),juce::sendNotificationSync);};addChildComponent(value);}
 loadIRButton.onClick=[this]{irChooser=std::make_unique<juce::FileChooser>("Load cabinet IR",juce::File{},"*.wav;*.aiff;*.aif");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);irChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe](const juce::FileChooser& c){if(safe&&c.getResult().existsAsFile()){safe->statusLabel.setText(safe->processor.loadCabinetIR(c.getResult())?"Cabinet IR loaded":"Could not load IR",juce::dontSendNotification);safe->refreshSlotControls();}});};
 loadGridIRButton.onClick=[this]{const auto r=selectedRow,c=selectedColumn;irChooser=std::make_unique<juce::FileChooser>("Load selected cabinet WAV IR",juce::File{},"*.wav");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);irChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe,r,c](const juce::FileChooser& chooser){if(safe&&chooser.getResult().existsAsFile()){bool ok=safe->processor.loadGridCabinetIR(chooser.getResult(),(size_t)r,(size_t)c);safe->statusLabel.setText(ok?"Slot IR loaded":"IR rejected: WAV, max 4096 taps",juce::dontSendNotification);safe->refreshSlotControls();}});};
 loadNeuralButton.onClick=[this]{const auto r=selectedRow,c=selectedColumn;neuralChooser=std::make_unique<juce::FileChooser>("Load VCMLP1 amplifier model",juce::File{},"*.vcmlp;*.txt");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);neuralChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe,r,c](const juce::FileChooser& chooser){if(safe&&chooser.getResult().existsAsFile()){bool ok=safe->processor.loadGridNeuralModel(chooser.getResult(),(size_t)r,(size_t)c);safe->statusLabel.setText(ok?"Model loaded":"Choose an Amp slot and VCMLP1 model",juce::dontSendNotification);safe->refreshSlotControls();}});};
 savePresetButton.onClick=[this]{presetChooser=std::make_unique<juce::FileChooser>("Save preset",juce::File{},"*.vcpreset");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);presetChooser->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles|juce::FileBrowserComponent::warnAboutOverwriting,[safe](const juce::FileChooser& c){if(!safe||c.getResult()==juce::File{})return;juce::MemoryBlock data;safe->processor.getStateInformation(data);bool ok=c.getResult().withFileExtension(".vcpreset").replaceWithData(data.getData(),data.getSize());safe->statusLabel.setText(ok?"Preset saved":"Preset save failed",juce::dontSendNotification);});};
 loadPresetButton.onClick=[this]{presetChooser=std::make_unique<juce::FileChooser>("Open preset",juce::File{},"*.vcpreset");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);presetChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe](const juce::FileChooser& c){if(!safe||!c.getResult().existsAsFile()||c.getResult().getSize()>1024*1024)return;juce::MemoryBlock data;if(c.getResult().loadFileAsData(data)){safe->processor.setStateInformation(data.getData(),(int)data.getSize());safe->refreshSlotControls();safe->lastView=-1;safe->updateView();safe->statusLabel.setText("Preset loaded",juce::dontSendNotification);}});};
 savePresetButton.setComponentID("save");loadPresetButton.setComponentID("open");directoryButton.setComponentID("directory");
 for(size_t i=0;i<switches.size();++i)if(i!=8&&i!=9){switches[i].button.setComponentID(i==6?"bypass":"power");switches[i].button.setTooltip(i==6?"Global bypass":"Enable selected effect");}
 slotEnabled.setComponentID("power");
 for(int i=0;i<8;++i){auto& b=deviceEntries[(size_t)i];b.setComponentID("device-"+juce::String(i));b.onClick=[this,i]{applyDeviceVariant(i);};addChildComponent(b);
  auto& q=quickSelectors[(size_t)i];q.setComponentID("quick-"+juce::String(i));q.setButtonText(juce::String::charToString((juce::juce_wchar)('A'+i)));q.setTooltip("Select slot "+juce::String(i+1));q.onClick=[this,i]{selectSlot(selectedRow,i);};addAndMakeVisible(q);}
 for(int i=0;i<2;++i){auto& b=bankButtons[(size_t)i];b.setButtonText(i==0?"I":"II");b.onClick=[this,i]{rowBank=i;selectSlot(i*2,selectedColumn);};addAndMakeVisible(b);}
 previousPreset.onClick=[this]{int id=presetBox.getSelectedId();presetBox.setSelectedId(id<=2?4:id-1);};nextPreset.onClick=[this]{int id=presetBox.getSelectedId();presetBox.setSelectedId(id>=4?2:juce::jmax(2,id+1));};
 directoryButton.onClick=[this]{loadPresetButton.triggerClick();};presetMenuButton.onClick=[this]{presetBox.showPopup();};stompModeButton.onClick=[this]{processor.parameters.getParameter("gridMode")->setValueNotifyingHost(1.f);lastView=-1;updateView();};
 ioButton.onClick=[this]{statusLabel.setText("I/O: "+juce::String(processor.getTotalNumInputChannels())+" inputs / "+juce::String(processor.getTotalNumOutputChannels())+" outputs. Standalone driver settings are in the application's Audio Settings menu.",juce::dontSendNotification);};
 headerStompButton.setComponentID("pedal");headerStompButton.setTooltip("Stomp mode / enable Grid engine");headerStompButton.onClick=[this]{stompModeButton.triggerClick();};
 for(auto* b:{&headerStompButton,&previousPreset,&nextPreset,&directoryButton,&ioButton,&presetMenuButton,&stompModeButton})addAndMakeVisible(*b);
 for(int r=0;r<4;++r)rowSelector.addItem("ROW "+juce::String(r+1),r+1);rowSelector.setSelectedId(1,juce::dontSendNotification);rowSelector.onChange=[this]{selectSlot(rowSelector.getSelectedId()-1,selectedColumn);};addAndMakeVisible(rowSelector);
 editorsButton.setComponentID("editors-toggle");editorsButton.onClick=[this]{setEditorsVisible(!editorsVisible);};addAndMakeVisible(editorsButton);
 setResizable(true,true);setResizeLimits(800,584,1776,1296);getConstrainer()->setFixedAspectRatio(1184.0/864.0);
 int width=1184,height=864;if(auto* display=juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()){height=juce::jlimit(584,864,display->userArea.getHeight()-100);width=(int)std::round(height*1184./864.);if(width>display->userArea.getWidth()-60){width=juce::jmax(800,display->userArea.getWidth()-60);height=(int)std::round(width*864./1184.);}}
 setSize(width,height);refreshSlotControls();updateView();startTimerHz(12);
}
VirtualCortexEditor::~VirtualCortexEditor(){stopTimer();setLookAndFeel(nullptr);}
bool VirtualCortexEditor::gridMode() const{return processor.parameters.getRawParameterValue("gridMode")->load()>.5f;}
int VirtualCortexEditor::currentType() const{if(masterPage)return 11;if(!gridMode())return browserType;const auto grid=processor.getEditableGrid();return (int)grid.get((size_t)selectedRow,(size_t)selectedColumn)->type;}
std::vector<int> VirtualCortexEditor::visibleKnobs() const{
 switch(currentType()){case 1:return {0,10};case 2:return {1,2,9,3};case 3:return {4,11};case 4:return {15,16,17};case 5:return {5,6,7};case 6:return {8};case 7:return {12,13};case 11:return {14};default:return {};}
}
std::vector<int> VirtualCortexEditor::visibleSlotControls() const{
 switch(currentType()){case 2:return {0};case 5:return {1,2,3};case 6:return {1};case 7:return {4,5};case 8:return {0,3,4,5,2,1};case 9:case 10:return {0,4,5,1};default:return {};}
}
void VirtualCortexEditor::selectSlot(int row,int col){if(!vc::SignalGrid::valid((size_t)row,(size_t)col))return;selectedRow=row;selectedColumn=col;rowBank=row/2;rowSelector.setSelectedId(row+1,juce::dontSendNotification);masterPage=false;const auto grid=processor.getEditableGrid();const int type=(int)grid.get((size_t)row,(size_t)col)->type;if(type)browserType=type;lastView=-1;refreshSlotControls();updateView();}
bool VirtualCortexEditor::insertBlock(int type){if(type<0||type>10)return false;bool ok=processor.setGridBlock((size_t)selectedRow,(size_t)selectedColumn,{(vc::BlockType)type,true});masterPage=false;lastView=-1;updateView();statusLabel.setText(gridMode()?"Block updated":"Block updated. Enable GRID ENGINE to hear the grid.",juce::dontSendNotification);return ok;}
bool VirtualCortexEditor::toggleSelectedSlot(){const auto grid=processor.getEditableGrid();const auto slot=*grid.get((size_t)selectedRow,(size_t)selectedColumn);if(slot.type==vc::BlockType::Empty)return false;bool ok=processor.setGridBlock((size_t)selectedRow,(size_t)selectedColumn,{slot.type,!slot.enabled});refreshGridControls();return ok;}
void VirtualCortexEditor::refreshGridControls(){const auto grid=processor.getEditableGrid();for(int r=0;r<4;++r){laneButtons[(size_t)r].setToggleState(processor.isGridLaneEnabled((size_t)r),juce::dontSendNotification);for(int c=0;c<8;++c){const auto slot=*grid.get((size_t)r,(size_t)c);auto& b=gridCells[(size_t)r][(size_t)c];b.type=(int)slot.type;b.blockEnabled=slot.enabled&&processor.isGridLaneEnabled((size_t)r);b.selected=r==selectedRow&&c==selectedColumn;b.caption=juce::String::charToString((juce::juce_wchar)('A'+c));b.repaint();}}
 for(int c=0;c<8;++c){const auto slot=*grid.get((size_t)selectedRow,(size_t)c);auto& b=stomps[(size_t)c];b.type=(int)slot.type;b.blockEnabled=slot.enabled;b.selected=c==selectedColumn;b.repaint();}
 for(int c=0;c<8;++c)quickSelectors[(size_t)c].setToggleState(c==selectedColumn,juce::dontSendNotification);
 slotEnabled.setToggleState(grid.get((size_t)selectedRow,(size_t)selectedColumn)->enabled,juce::dontSendNotification);
}
void VirtualCortexEditor::refreshSlotControls(){const auto p=processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn);const float values[]={p.drive,p.mix,p.time,p.feedback,p.rate,p.depth};for(int i=0;i<6;++i)if(!slotSliders[(size_t)i].isMouseButtonDown())slotSliders[(size_t)i].setValue(values[i],juce::dontSendNotification);irName.setText(processor.getCabinetIRName(),juce::dontSendNotification);gridIRName.setText(processor.getGridCabinetIRName((size_t)selectedRow,(size_t)selectedColumn),juce::dontSendNotification);neuralName.setText(processor.getGridNeuralModelName((size_t)selectedRow,(size_t)selectedColumn),juce::dontSendNotification);
 for(auto& k:knobs)if(!k.value.isBeingEdited())k.value.setText(k.slider.getTextFromValue(k.slider.getValue()),juce::dontSendNotification);
 for(size_t i=0;i<6;++i)if(!slotValues[i].isBeingEdited())slotValues[i].setText(slotSliders[i].getTextFromValue(slotSliders[i].getValue()),juce::dontSendNotification);
}
void VirtualCortexEditor::commitSlotControls(){vc::SlotParameters p;p.drive=(float)slotSliders[0].getValue();p.mix=(float)slotSliders[1].getValue();p.time=(float)slotSliders[2].getValue();p.feedback=(float)slotSliders[3].getValue();p.rate=(float)slotSliders[4].getValue();p.depth=(float)slotSliders[5].getValue();processor.setGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn,p);}
void VirtualCortexEditor::timerCallback(){updateView();refreshSlotControls();}
void VirtualCortexEditor::updateView(){refreshGridControls();const int type=currentType();const bool gm=gridMode();if(type==lastView&&gm==lastGridMode)return;if(gm!=lastGridMode)statusLabel.setText({},juce::dontSendNotification);lastView=type;lastGridMode=gm;for(auto& k:knobs){k.slider.setVisible(false);k.value.setVisible(false);}for(auto& s:slotSliders)s.setVisible(false);for(auto& value:slotValues)value.setVisible(false);for(auto& s:switches)s.button.setVisible(false);
 switches[6].button.setVisible(true);switches[9].button.setVisible(true);
 slotLabels={"DRIVE","MIX","TIME","FEEDBACK","RATE","DEPTH"};
 if(type==8)slotLabels={"THRESHOLD","MIX","MAKEUP","RATIO","ATTACK","RELEASE"};
 if(type==9||type==10)slotLabels={"DRIVE","MIX","TIME","FEEDBACK","TONE","LEVEL"};
 const auto colour=CortexBlockButton::colourFor(type);
 if(!gm||masterPage){for(int i:visibleKnobs()){knobs[(size_t)i].slider.setVisible(true);knobs[(size_t)i].value.setVisible(true);knobs[(size_t)i].slider.setColour(juce::Slider::rotarySliderFillColourId,colour);knobs[(size_t)i].slider.setEnabled(true);knobs[(size_t)i].value.setEnabled(true);}int enable=-1;switch(type){case 1:enable=0;break;case 2:enable=1;break;case 3:enable=2;break;case 4:enable=7;break;case 5:enable=3;break;case 6:enable=4;break;case 7:enable=5;break;}if(enable>=0)switches[(size_t)enable].button.setVisible(true);switches[8].button.setVisible(type==3&&!gm);}
 else for(int i:visibleSlotControls()){slotSliders[(size_t)i].setVisible(true);slotValues[(size_t)i].setVisible(true);slotSliders[(size_t)i].setColour(juce::Slider::rotarySliderFillColourId,colour);}
 slotEnabled.setVisible(gm&&!masterPage&&type!=0);loadIRButton.setVisible(!gm&&type==3);irName.setVisible(!gm&&type==3);loadGridIRButton.setVisible(gm&&type==3);gridIRName.setVisible(gm&&type==3);loadNeuralButton.setVisible(gm&&type==2);neuralName.setVisible(gm&&type==2);addButton.setVisible(false);addButton.setEnabled(browserType<11);removeButton.setEnabled(!masterPage);
 for(int i=0;i<11;++i){browserButtons[(size_t)i].selected=i+1==browserType;browserButtons[(size_t)i].repaint();}refreshDeviceList();
 refreshSlotControls();resized();repaint();
}
void VirtualCortexEditor::setEditorsVisible(bool visible){editorsVisible=visible;lastView=-1;updateView();}
void VirtualCortexEditor::resized(){
 designScale=juce::jmin(getWidth()/1184.f,getHeight()/864.f);designOffsetX=(getWidth()-1184.f*designScale)*.5f;designOffsetY=(getHeight()-864.f*designScale)*.5f;
 browserArea={53,78,240,752};gridArea={302,132,872,184};parameterArea={302,318,872,272};stompArea={474,598,700,232};
 previousPreset.setBounds(302,78,35,36);nextPreset.setBounds(339,78,35,36);presetBox.setBounds(376,78,340,36);savePresetButton.setBounds(718,78,35,36);loadPresetButton.setBounds(755,78,34,36);
 for(int i=0;i<8;++i)quickSelectors[(size_t)i].setBounds(798+i*38,78,35,36);headerStompButton.setBounds(1106,78,68,36);
 bankButtons[0].setBounds(224,37,25,27);bankButtons[1].setBounds(253,37,25,27);directoryButton.setBounds(638,30,142,44);ioButton.setBounds(1046,35,32,32);switches[6].button.setBounds(976,35,32,32);switches[9].button.setBounds(354,837,120,24);
 for(int i=0;i<11;++i)browserButtons[(size_t)i].setBounds(9,78+i*68,43,68);
 for(int i=0;i<8;++i)deviceEntries[(size_t)i].setBounds(62,119+i*38,224,36);
 addButton.setBounds(62,789,224,31);
 bankButtons[0].setVisible(false);bankButtons[1].setVisible(false);
 const int rowStep=editorsVisible?46:160,rowTop=editorsVisible?132:153;
 for(int r=0;r<4;++r){laneButtons[(size_t)r].setVisible(true);laneButtons[(size_t)r].setBounds(302,rowTop+r*rowStep+5,49,editorsVisible?39:84);for(int c=0;c<8;++c){auto& cell=gridCells[(size_t)r][(size_t)c];cell.setVisible(true);cell.setBounds(363+c*93,rowTop+r*rowStep,94,editorsVisible?44:92);}}
 editorsButton.setButtonText(editorsVisible?"HIDE EDITORS":"SHOW EDITORS");editorsButton.setBounds(205,837,140,24);
 const auto indices=gridMode()&&!masterPage?visibleSlotControls():visibleKnobs();
 for(int j=0;j<(int)indices.size();++j){const int i=indices[(size_t)j];const int x=302+(j%5)*174,y=354+(j/5)*118;
  const auto knob=juce::Rectangle<int>(x+92,y+43,76,76),value=juce::Rectangle<int>(x+6,y+83,92,28);
  if(gridMode()&&!masterPage){slotSliders[(size_t)i].setBounds(knob);slotValues[(size_t)i].setBounds(value);}else{knobs[(size_t)i].slider.setBounds(knob);knobs[(size_t)i].value.setBounds(value);}}
 for(auto& sw:switches)if(&sw!=&switches[6]&&&sw!=&switches[9])sw.button.setBounds(1137,318,36,35);slotEnabled.setBounds(1137,318,36,35);
 switches[8].button.setBounds(309,489,150,34);removeButton.setBounds(302,318,35,35);
 for(auto* b:{&loadIRButton,&loadGridIRButton,&loadNeuralButton})b->setBounds(838,492,245,32);
 for(auto* l:{&irName,&gridIRName,&neuralName})l->setBounds(838,530,245,25);
 presetMenuButton.setBounds(302,635,172,52);rowSelector.setBounds(302,688,172,52);stompModeButton.setBounds(302,741,172,52);
 for(int c=0;c<8;++c)stomps[(size_t)c].setBounds(474+(c%4)*175,598+(c/4)*116,175,116);
 statusLabel.setBounds(482,836,690,24);
 if(!editorsVisible){
  for(auto& k:knobs){k.slider.setVisible(false);k.value.setVisible(false);}for(auto& s:slotSliders)s.setVisible(false);for(auto& v:slotValues)v.setVisible(false);
  for(size_t i=0;i<switches.size();++i)if(i!=6&&i!=9)switches[i].button.setVisible(false);
  slotEnabled.setVisible(false);removeButton.setVisible(false);loadIRButton.setVisible(false);loadGridIRButton.setVisible(false);loadNeuralButton.setVisible(false);irName.setVisible(false);gridIRName.setVisible(false);neuralName.setVisible(false);
 }
 else removeButton.setVisible(true);
 for(auto& b:stomps)b.setVisible(editorsVisible);presetMenuButton.setVisible(editorsVisible);rowSelector.setVisible(editorsVisible);stompModeButton.setVisible(editorsVisible);
 const auto transform=juce::AffineTransform::scale(designScale).translated(designOffsetX,designOffsetY);
 for(int i=0;i<getNumChildComponents();++i){auto* child=getChildComponent(i);if(dynamic_cast<juce::ResizableCornerComponent*>(child)==nullptr)child->setTransform(transform);}
}
void VirtualCortexEditor::paint(juce::Graphics& g){
 g.fillAll(juce::Colours::black);g.addTransform(juce::AffineTransform::scale(designScale).translated(designOffsetX,designOffsetY));
 g.setColour(juce::Colour(0xff313031));g.fillRect(0,0,1184,26);drawCaption(g,"Virtual Cortex",{440,1,300,23},12.f,juce::Colour(0xffa5a5a5),juce::Justification::centred);
 g.setColour(juce::Colour(0xff5845ff));g.fillRoundedRectangle(10,2,65,18,9);g.setColour(juce::Colours::white);g.drawRoundedRectangle(35,5,16,11,1.5f,1.f);g.drawLine(39,18,47,18,1.f);g.fillEllipse(45,10,4,4);
 CortexBlockButton::drawIcon(g,{14,38,24,27},7,juce::Colours::white);drawCaption(g,"Virtual Cortex",{55,36,160,32},14.f,juce::Colour(0xff777777));
 drawCaption(g,"GRID",{438,30,66,45},14.f,juce::Colours::white,juce::Justification::centred);g.setColour(juce::Colour(0xff85f87d));g.fillRect(438,26,66,2);
 g.setColour(juce::Colour(0xff131313));g.fillRoundedRectangle(9,78,43,752,6);g.setColour(panel);g.fillRoundedRectangle(browserArea.toFloat(),6.f);
 drawCaption(g,CortexBlockButton::nameFor(browserType).toUpperCase(),{68,87,216,26},14.f,juce::Colours::white,juce::Justification::centredLeft,true);
 const int rowStep=editorsVisible?46:160,rowTop=editorsVisible?132:153;
 for(int row=0;row<4;++row){const int y=rowTop+row*rowStep;const bool active=processor.isGridLaneEnabled((size_t)row);g.setColour(juce::Colour(active?0xffa3a3a3:0xff383838));const float cy=(float)y+(editorsVisible?22.f:47.f);g.drawLine(352,cy,1124,cy,1.f);g.setColour(juce::Colour(0xff111212));g.fillRoundedRectangle(1124.f,(float)y+5,49.f,editorsVisible?39.f:84.f,16);drawCaption(g,"Mix",{1125,y+(editorsVisible?9:30),47,25},12.f,juce::Colours::white,juce::Justification::centred);}
 if(!editorsVisible)drawCaption(g,"PARALLEL ROWS  /  Same input to enabled rows / averaged main output",{363,768,760,35},14.f,muted);
 if(editorsVisible){
 g.setColour(juce::Colour(0xff111111));g.fillRoundedRectangle(parameterArea.toFloat(),5);g.setColour(panel);g.fillRect(302,354,872,236);
 for(int i=0;i<3;++i){g.setColour(muted);g.fillEllipse(318.f,325.f+i*7.f,2.f,2.f);g.fillEllipse(318.f,605.f+i*7.f,2.f,2.f);}
 drawCaption(g,CortexBlockButton::nameFor(currentType()),{347,319,135,33},18.f,muted);drawCaption(g,currentType()==2?"Original amplifier":currentType()==3?"Cabinet IR":currentType()==8?"Original peak compressor":currentType()==9?"Original soft clipping":currentType()==10?"Original hard clipping":"Original",{485,319,600,33},18.f,CortexBlockButton::colourFor(currentType()));
 const auto indices=gridMode()&&!masterPage?visibleSlotControls():visibleKnobs();for(int j=0;j<10;++j){int x=302+(j%5)*174,y=354+(j/5)*118;g.setColour(juce::Colour(0xff070707));g.drawLine((float)x+174,(float)y+8,(float)x+174,(float)y+118,1.f);g.drawLine((float)x,(float)y+118,(float)x+174,(float)y+118,1.f);if(j<(int)indices.size()){int i=indices[(size_t)j];drawCaption(g,gridMode()&&!masterPage?slotLabels[(size_t)i]:knobs[(size_t)i].label,{x+9,y+6,156,23},15.f);}}
 if(indices.empty())drawCaption(g,currentType()==0?"Select a slot, then choose a device from the directory.":currentType()==3?"Load a cabinet impulse response.":"Fixed settings in the current grid engine.",{320,381,730,44},14.f,muted);

 g.setColour(panel);g.fillRect(302,598,172,232);g.setColour(juce::Colour(0xff070707));g.drawRect(302,598,172,36,1);drawCaption(g,"Gig view",{343,599,125,33},20.f,juce::Colours::white,juce::Justification::centredLeft,true);drawCaption(g,"Parameter Editor",{310,796,156,32},14.f,muted);
 }
 drawCaption(g,gridMode()?"GRID":"SERIAL",{20,836,65,24},11.f,muted);drawCaption(g,processor.getSampleRate()>0?juce::String(processor.getSampleRate()/1000.,1)+" kHz":"AUDIO",{103,836,100,24},11.f,muted);drawCaption(g,"Virtual Cortex v6.5.0",{613,837,230,22},11.f,juce::Colour(0xff676767));
}
void VirtualCortexEditor::applyPreset(int index){auto set=[this](const char* id,float value){if(auto* p=processor.parameters.getParameter(id))p->setValueNotifyingHost(p->convertTo0to1(value));};if(index<2)return;set("gridMode",0.f);
 if(index==2){set("drive",.12f);set("tone",.7f);set("level",.65f);set("delayMix",.12f);set("reverbMix",.25f);set("ampBias",0.f);set("delayOn",1.f);set("reverbOn",1.f);}
 if(index==3){set("drive",.48f);set("tone",.56f);set("level",.48f);set("delayMix",0.f);set("reverbMix",.04f);set("ampBias",.08f);set("delayOn",0.f);set("reverbOn",0.f);}
 if(index==4){set("drive",.73f);set("tone",.62f);set("level",.38f);set("delayTime",390.f);set("delayFeedback",.32f);set("delayMix",.22f);set("reverbMix",.15f);set("delayOn",1.f);set("reverbOn",1.f);}lastView=-1;updateView();}
juce::AudioProcessorEditor* VirtualCortexProcessor::createEditor(){return new VirtualCortexEditor(*this);}

juce::StringArray VirtualCortexEditor::deviceNames() const{
 if(gridMode()&&(browserType==1||browserType==4))return {"Original "+CortexBlockButton::nameFor(browserType)};
 switch(browserType){
 case 1:return {"Gentle","Balanced","Tight","Hard gate"};
 case 2:return {"Original amplifier","Clean","Edge of breakup","Crunch","High gain","Load VCMLP1 model..."};
 case 3:return {"Built-in cabinet","Load cabinet IR..."};
 case 4:return {"Flat","Warm","Bright","Mid boost","Mid cut"};
 case 5:return {"Slapback","Short echo","Medium echo","Long echo","Ambient echo"};
 case 6:return {"Dry","Light","Medium","Deep","Full"};
 case 7:return {"Slow","Gentle","Classic","Fast","Maximum"};
 case 8:return {"Original peak compressor","Gentle compression","Firm compression","Parallel compression"};
 case 9:return {"Original soft overdrive","Low drive","Medium drive","High drive"};
 case 10:return {"Original hard distortion","Low distortion","Medium distortion","High distortion"};
 case 11:return {"Unity","-6 dB","-12 dB","Mute"};
 default:return {};
 }
}
void VirtualCortexEditor::refreshDeviceList(){const auto names=deviceNames();for(int i=0;i<8;++i){auto& b=deviceEntries[(size_t)i];b.setVisible(i<names.size());if(i<names.size()){b.setButtonText(names[i]);b.setToggleState(i==variant,juce::dontSendNotification);}}}
void VirtualCortexEditor::applyDeviceVariant(int index){
 const auto names=deviceNames();if(index<0||index>=names.size())return;variant=index;
 auto set=[this](const char* id,float v){if(auto* p=processor.parameters.getParameter(id))p->setValueNotifyingHost(p->convertTo0to1(v));};
 if(browserType==11){const float levels[]={1.f,.501187f,.251189f,0.f};set("outputTrim",levels[index]);statusLabel.setText("Output level updated",juce::dontSendNotification);refreshDeviceList();return;}
 if(browserType>=8&&browserType<=10&&!gridMode()){set("gridMode",1.f);lastView=-1;updateView();}
 if(gridMode())insertBlock(browserType);
 if(browserType==2&&index==5){if(!gridMode()){set("gridMode",1.f);insertBlock(2);}lastView=-1;updateView();loadNeuralButton.triggerClick();return;}
 if(browserType==3&&index==1){lastView=-1;updateView();if(gridMode())loadGridIRButton.triggerClick();else loadIRButton.triggerClick();return;}
 if(gridMode()){
  auto p=processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn);
  if(browserType==8){p.drive=index==2?.35f:.5f;p.feedback=index==2?.37f:.158f;p.rate=.05f;p.depth=.18f;p.time=0.f;p.mix=index==3?.5f:1.f;}
  if(browserType==9||browserType==10){const float drives[]={.4f,.15f,.4f,.75f};p.drive=drives[index];p.rate=.5f;p.depth=.35f;p.mix=1.f;}
  if(browserType==2){const float drives[]={.5f,.08f,.2f,.45f,.8f};p.drive=drives[index];}
  if(browserType==5){const float ms[]={90,180,320,480,450};p.time=(ms[index]-20.f)/480.f;p.feedback=.15f+.15f*index;p.mix=.15f+.08f*index;}
  if(browserType==6){const float mixes[]={0,.15f,.3f,.55f,1.f};p.mix=mixes[index];}
  if(browserType==7){const float rates[]={.05f,.2f,.4f,.75f,1.f};p.rate=rates[index];p.depth=.5f;}
  processor.setGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn,p);
 }else{
  if(browserType==1){const float values[]={-74,-62,-50,-36};set("gate",values[index]);}
  if(browserType==8){p.drive=index==2?.35f:.5f;p.feedback=index==2?.37f:.158f;p.rate=.05f;p.depth=.18f;p.time=0.f;p.mix=index==3?.5f:1.f;}
  if(browserType==9||browserType==10){const float drives[]={.4f,.15f,.4f,.75f};p.drive=drives[index];p.rate=.5f;p.depth=.35f;p.mix=1.f;}
  if(browserType==2){const float drives[]={.35f,.08f,.2f,.45f,.8f};set("drive",drives[index]);set("level",index>=3?.45f:.65f);}
  if(browserType==3){set("cabOn",1);set("irOn",0);set("cab",1);}
  if(browserType==4){const float values[5][3]={{0,0,0},{3,0,-2},{-1,0,4},{0,4,0},{0,-4,0}};set("eqLow",values[index][0]);set("eqMid",values[index][1]);set("eqHigh",values[index][2]);}
  if(browserType==5){const float ms[]={90,180,320,480,650};set("delayTime",ms[index]);set("delayFeedback",.15f+.12f*index);set("delayMix",.15f+.08f*index);set("delayOn",1);}
  if(browserType==6){const float values[]={0,.1f,.2f,.35f,.55f};set("reverbMix",values[index]);set("reverbOn",1);}
  if(browserType==7){const float rates[]={.5f,2,4,8,12};set("tremRate",rates[index]);set("tremDepth",.5f);set("tremOn",1);}
 }
 lastView=-1;refreshSlotControls();updateView();statusLabel.setText(names[index]+" selected",juce::dontSendNotification);
}
