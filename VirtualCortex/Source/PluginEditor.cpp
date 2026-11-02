#include "PluginEditor.h"
#include <cmath>

namespace {
const juce::Colour background(0xff090a0b), panel(0xff1c1d1f), muted(0xff8a8e93), mint(0xff53e3c2);
void drawCaption(juce::Graphics& g,const juce::String& s,juce::Rectangle<int> r,float size,juce::Colour c=juce::Colours::white,int align=juce::Justification::centredLeft){
 g.setColour(c);g.setFont(juce::Font(juce::FontOptions(size)));g.drawFittedText(s,r,align,1);
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
void CortexLookAndFeel::drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour&,bool over,bool down){
 auto r=b.getLocalBounds().toFloat().reduced(1.f);
 g.setColour(b.getToggleState()?juce::Colour(0xff25473e):juce::Colour(over?0xff303337:0xff222426));
 g.fillRoundedRectangle(r,5.f);
 g.setColour(b.getToggleState()?mint:juce::Colour(down?0xff7c858c:0xff34373b));
 g.drawRoundedRectangle(r,5.f,1.f);
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
 const juce::uint32 values[]={0xff41454a,0xffd8a944,0xfff25562,0xff57d3bf,0xff4097ef,0xff43d8c9,0xff65f2cb,0xff9265ff,0xffa0a7ad};
 return juce::Colour(values[juce::jlimit(0,8,type)]);
}
juce::String CortexBlockButton::nameFor(int type){
 const char* names[]={"Empty","Gate","Amp","Cabinet","EQ","Delay","Reverb","Tremolo","Output"};
 return names[juce::jlimit(0,8,type)];
}
void CortexBlockButton::drawIcon(juce::Graphics& g,juce::Rectangle<float> r,int type,juce::Colour colour){
 g.setColour(colour);const auto cx=r.getCentreX(),cy=r.getCentreY();const auto u=r.getWidth()/24.f;
 juce::Path p;
 if(type==0){g.drawLine(cx-5*u,cy,cx+5*u,cy,1.2f);g.drawLine(cx,cy-5*u,cx,cy+5*u,1.2f);return;}
 if(type==2){g.drawRoundedRectangle(r.reduced(2*u,5*u),1.f,1.4f);g.drawLine(r.getX()+5*u,cy,r.getRight()-5*u,cy,1.2f);return;}
 if(type==3){g.drawEllipse(cx-8*u,cy-8*u,16*u,16*u,1.4f);g.drawEllipse(cx-3*u,cy-3*u,6*u,6*u,1.2f);return;}
 if(type==4){for(int i=0;i<3;++i){auto xx=cx+(i-1)*7*u;auto yy=cy+(i==1?3.f:-3.f)*u;g.drawLine(xx,cy-9*u,xx,cy+9*u,1.2f);g.fillRoundedRectangle(xx-2*u,yy-2*u,4*u,4*u,1.f);}return;}
 if(type==5){for(int i=0;i<4;++i){const auto a=(8-2*i)*u;g.drawLine(cx+(i*5-8)*u,cy-a,cx+(i*5-8)*u,cy+a,1.5f);}return;}
 if(type==6){p.startNewSubPath(cx-8*u,cy-4*u);p.lineTo(cx+2*u,cy-4*u);p.lineTo(cx+2*u,cy+7*u);p.lineTo(cx-8*u,cy+7*u);p.closeSubPath();p.startNewSubPath(cx-8*u,cy-4*u);p.lineTo(cx-2*u,cy-10*u);p.lineTo(cx+8*u,cy-10*u);p.lineTo(cx+8*u,cy+1*u);p.lineTo(cx+2*u,cy+7*u);p.startNewSubPath(cx+2*u,cy-4*u);p.lineTo(cx+8*u,cy-10*u);g.strokePath(p,juce::PathStrokeType(1.2f));return;}
 p.startNewSubPath(r.getX(),cy);
 for(int i=1;i<=40;++i){float t=i/40.f;float yy=type==1?(t<.45f?-.55f:.55f):std::sin(t*6.2831853f*(type==7?1.f:2.f))*.7f;p.lineTo(r.getX()+t*r.getWidth(),cy+yy*r.getHeight()*.45f);}
 g.strokePath(p,juce::PathStrokeType(1.5f));
}
void CortexBlockButton::paintButton(juce::Graphics& g,bool over,bool){
 const auto bounds=getLocalBounds().toFloat();const auto colour=colourFor(type);
 if(stomp){
  g.setColour(blockEnabled&&type?colour.withAlpha(.13f):juce::Colour(0xff191b1d));g.fillRoundedRectangle(bounds.reduced(1),5.f);
  g.setColour(selected?colour:juce::Colour(0xff303338));g.drawRoundedRectangle(bounds.reduced(1),5.f,selected?2.f:1.f);
  drawCaption(g,caption,getLocalBounds().withTrimmedLeft(9).removeFromTop(22),12.f,colour);
  drawIcon(g,{bounds.getCentreX()-10,25.f,20.f,20.f},type,blockEnabled?juce::Colours::white:muted);
  drawCaption(g,nameFor(type),getLocalBounds().withTrimmedTop(47),11.f,blockEnabled?juce::Colours::white:muted,juce::Justification::centred);return;
 }
 const float side=juce::jmin(bounds.getWidth()-4.f,bounds.getHeight()-15.f);
 auto box=juce::Rectangle<float>(side,side).withCentre({bounds.getCentreX(),bounds.getCentreY()-5.f});
 g.setColour(selected?colour.withAlpha(.16f):juce::Colour(over?0xff242628:0xff101213));g.fillRoundedRectangle(box,10.f);
 g.setColour(blockEnabled?colour:colour.withAlpha(.3f));g.drawRoundedRectangle(box,10.f,selected?3.f:1.4f);
 drawIcon(g,box.reduced(side*.24f),type,blockEnabled?juce::Colours::white:muted.withAlpha(.5f));
 drawCaption(g,nameFor(type),{0,(int)box.getBottom()+2,getWidth(),10},10.f,selected?colour:muted,juce::Justification::centred);
}
VirtualCortexEditor::VirtualCortexEditor(VirtualCortexProcessor& p):AudioProcessorEditor(&p),processor(p){
 setLookAndFeel(&look);
 const std::array<const char*,18> ids={"gate","drive","tone","level","cab","delayTime","delayFeedback","delayMix","reverbMix","ampBias","inputHP","cabCut","tremRate","tremDepth","outputTrim","eqLow","eqMid","eqHigh"};
 const std::array<const char*,18> labels={"THRESHOLD","DRIVE","TONE","LEVEL","CAB MIX","TIME","FEEDBACK","MIX","MIX","BIAS","INPUT HIGH PASS","LOW PASS","RATE","DEPTH","OUTPUT TRIM","BASS","MID","TREBLE"};
 for(size_t i=0;i<knobs.size();++i){auto& k=knobs[i];k.label=labels[i];k.slider.setComponentID(ids[i]);k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,96,20);addChildComponent(k.slider);k.attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,ids[i],k.slider);
  k.slider.valueFromTextFunction=[i](const juce::String& text){double v=text.getDoubleValue();return (i==0||i>=15||i==5||i==10||i==11||i==12||i==9)?v:v/100.;};
  k.slider.textFromValueFunction=[i](double v){if(i==0||i>=15)return juce::String(v,1)+" dB";if(i==5)return juce::String(v,0)+" ms";if(i==10||i==11)return juce::String(v,0)+" Hz";if(i==12)return juce::String(v,2)+" Hz";if(i==9)return juce::String(v,2);return juce::String(v*100,1)+" %";};
 }
 const std::array<const char*,10> switchesIds={"gateOn","ampOn","cabOn","delayOn","reverbOn","tremOn","bypass","eqOn","irOn","gridMode"};
 const std::array<const char*,10> switchesLabels={"ENABLED","ENABLED","ENABLED","ENABLED","ENABLED","ENABLED","BYPASS","ENABLED","USE IR","GRID ENGINE"};
 for(size_t i=0;i<switches.size();++i){auto& s=switches[i];s.button.setButtonText(switchesLabels[i]);addChildComponent(s.button);s.attachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.parameters,switchesIds[i],s.button);}
 presetBox.addItem("Clean ambience",2);presetBox.addItem("Crunch rhythm",3);presetBox.addItem("Lead delay",4);presetBox.setTextWhenNothingSelected("01  Virtual Cortex / User preset");presetBox.onChange=[this]{applyPreset(presetBox.getSelectedId());};addAndMakeVisible(presetBox);
 for(auto* b:{&savePresetButton,&loadPresetButton,&addButton,&removeButton,&loadIRButton,&loadGridIRButton,&loadNeuralButton})addAndMakeVisible(*b);
 for(auto* l:{&irName,&gridIRName,&neuralName,&statusLabel}){l->setColour(juce::Label::textColourId,muted);l->setFont(juce::Font(juce::FontOptions(11.f)));addAndMakeVisible(*l);}
 for(int i=0;i<8;++i){auto& b=browserButtons[(size_t)i];b.setButtonText(CortexBlockButton::nameFor(i+1));b.onClick=[this,i]{browserType=i+1;masterPage=(i==7);lastView=-1;updateView();};addAndMakeVisible(b);}
 addButton.onClick=[this]{insertBlock(browserType);};removeButton.onClick=[this]{insertBlock(0);};
 slotEnabled.onClick=[this]{toggleSelectedSlot();};addAndMakeVisible(slotEnabled);
 for(int r=0;r<4;++r){auto& lane=laneButtons[(size_t)r];lane.setButtonText(juce::String(r+1));lane.onClick=[this,r]{processor.setGridLaneEnabled((size_t)r,laneButtons[(size_t)r].getToggleState());refreshGridControls();};addAndMakeVisible(lane);
  for(int c=0;c<8;++c){auto& b=gridCells[(size_t)r][(size_t)c];b.onClick=[this,r,c]{selectSlot(r,c);};addAndMakeVisible(b);}
 }
 for(int c=0;c<8;++c){auto& b=stomps[(size_t)c];b.stomp=true;b.caption=juce::String::charToString((juce::juce_wchar)('A'+c));b.onClick=[this,c]{selectSlot(selectedRow,c);toggleSelectedSlot();};addAndMakeVisible(b);}
 for(int i=0;i<6;++i){auto& s=slotSliders[(size_t)i];s.setRange(0.,1.,.001);s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,96,20);s.valueFromTextFunction=[i](const juce::String& text){double v=text.getDoubleValue();if(i==2)return (v-20.)/480.;if(i==4)return (v-.1)/9.9;return v/(i==3?90.:100.);};s.textFromValueFunction=[i](double v){if(i==2)return juce::String(20.+480.*v,0)+" ms";if(i==4)return juce::String(.1+9.9*v,2)+" Hz";return juce::String(v*(i==3?90.:100.),1)+" %";};s.onValueChange=[this]{commitSlotControls();};addChildComponent(s);}
 loadIRButton.onClick=[this]{irChooser=std::make_unique<juce::FileChooser>("Load cabinet IR",juce::File{},"*.wav;*.aiff;*.aif");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);irChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe](const juce::FileChooser& c){if(safe&&c.getResult().existsAsFile()){safe->statusLabel.setText(safe->processor.loadCabinetIR(c.getResult())?"Cabinet IR loaded":"Could not load IR",juce::dontSendNotification);safe->refreshSlotControls();}});};
 loadGridIRButton.onClick=[this]{const auto r=selectedRow,c=selectedColumn;irChooser=std::make_unique<juce::FileChooser>("Load selected cabinet WAV IR",juce::File{},"*.wav");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);irChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe,r,c](const juce::FileChooser& chooser){if(safe&&chooser.getResult().existsAsFile()){bool ok=safe->processor.loadGridCabinetIR(chooser.getResult(),(size_t)r,(size_t)c);safe->statusLabel.setText(ok?"Slot IR loaded":"IR rejected: WAV, max 4096 taps",juce::dontSendNotification);safe->refreshSlotControls();}});};
 loadNeuralButton.onClick=[this]{const auto r=selectedRow,c=selectedColumn;neuralChooser=std::make_unique<juce::FileChooser>("Load VCMLP1 amplifier model",juce::File{},"*.vcmlp;*.txt");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);neuralChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe,r,c](const juce::FileChooser& chooser){if(safe&&chooser.getResult().existsAsFile()){bool ok=safe->processor.loadGridNeuralModel(chooser.getResult(),(size_t)r,(size_t)c);safe->statusLabel.setText(ok?"Model loaded":"Choose an enabled Amp slot and VCMLP1 model",juce::dontSendNotification);safe->refreshSlotControls();}});};
 savePresetButton.onClick=[this]{presetChooser=std::make_unique<juce::FileChooser>("Save preset",juce::File{},"*.vcpreset");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);presetChooser->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles|juce::FileBrowserComponent::warnAboutOverwriting,[safe](const juce::FileChooser& c){if(!safe||c.getResult()==juce::File{})return;juce::MemoryBlock data;safe->processor.getStateInformation(data);bool ok=c.getResult().withFileExtension(".vcpreset").replaceWithData(data.getData(),data.getSize());safe->statusLabel.setText(ok?"Preset saved":"Preset save failed",juce::dontSendNotification);});};
 loadPresetButton.onClick=[this]{presetChooser=std::make_unique<juce::FileChooser>("Open preset",juce::File{},"*.vcpreset");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);presetChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe](const juce::FileChooser& c){if(!safe||!c.getResult().existsAsFile()||c.getResult().getSize()>1024*1024)return;juce::MemoryBlock data;if(c.getResult().loadFileAsData(data)){safe->processor.setStateInformation(data.getData(),(int)data.getSize());safe->refreshSlotControls();safe->lastView=-1;safe->updateView();safe->statusLabel.setText("Preset loaded",juce::dontSendNotification);}});};
 setResizable(true,true);setResizeLimits(900,560,1800,1100);
 int width=1120,height=660;if(auto* display=juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()){width=juce::jlimit(900,1120,display->userArea.getWidth()-80);height=juce::jlimit(560,660,display->userArea.getHeight()-130);}
 setSize(width,height);refreshSlotControls();updateView();startTimerHz(12);
}
VirtualCortexEditor::~VirtualCortexEditor(){stopTimer();setLookAndFeel(nullptr);}
bool VirtualCortexEditor::gridMode() const{return processor.parameters.getRawParameterValue("gridMode")->load()>.5f;}
int VirtualCortexEditor::currentType() const{if(masterPage)return 8;if(!gridMode())return browserType;const auto grid=processor.getEditableGrid();return (int)grid.get((size_t)selectedRow,(size_t)selectedColumn)->type;}
std::vector<int> VirtualCortexEditor::visibleKnobs() const{
 switch(currentType()){case 1:return {0,10};case 2:return {1,2,9,3};case 3:return {4,11};case 4:return {15,16,17};case 5:return {5,6,7};case 6:return {8};case 7:return {12,13};case 8:return {14};default:return {};}
}
std::vector<int> VirtualCortexEditor::visibleSlotControls() const{
 switch(currentType()){case 2:return {0};case 5:return {1,2,3};case 6:return {1};case 7:return {4,5};default:return {};}
}
void VirtualCortexEditor::selectSlot(int row,int col){if(!vc::SignalGrid::valid((size_t)row,(size_t)col))return;selectedRow=row;selectedColumn=col;masterPage=false;const auto grid=processor.getEditableGrid();const int type=(int)grid.get((size_t)row,(size_t)col)->type;if(type)browserType=type;lastView=-1;refreshSlotControls();updateView();}
bool VirtualCortexEditor::insertBlock(int type){if(type<0||type>7)return false;bool ok=processor.setGridBlock((size_t)selectedRow,(size_t)selectedColumn,{(vc::BlockType)type,true});masterPage=false;lastView=-1;updateView();statusLabel.setText(gridMode()?"Block updated":"Block updated. Enable GRID ENGINE to hear the grid.",juce::dontSendNotification);return ok;}
bool VirtualCortexEditor::toggleSelectedSlot(){const auto grid=processor.getEditableGrid();const auto slot=*grid.get((size_t)selectedRow,(size_t)selectedColumn);if(slot.type==vc::BlockType::Empty)return false;bool ok=processor.setGridBlock((size_t)selectedRow,(size_t)selectedColumn,{slot.type,!slot.enabled});refreshGridControls();return ok;}
void VirtualCortexEditor::refreshGridControls(){const auto grid=processor.getEditableGrid();for(int r=0;r<4;++r){laneButtons[(size_t)r].setToggleState(processor.isGridLaneEnabled((size_t)r),juce::dontSendNotification);for(int c=0;c<8;++c){const auto slot=*grid.get((size_t)r,(size_t)c);auto& b=gridCells[(size_t)r][(size_t)c];b.type=(int)slot.type;b.blockEnabled=slot.enabled&&processor.isGridLaneEnabled((size_t)r);b.selected=r==selectedRow&&c==selectedColumn;b.repaint();}}
 for(int c=0;c<8;++c){const auto slot=*grid.get((size_t)selectedRow,(size_t)c);auto& b=stomps[(size_t)c];b.type=(int)slot.type;b.blockEnabled=slot.enabled;b.selected=c==selectedColumn;b.repaint();}
 slotEnabled.setToggleState(grid.get((size_t)selectedRow,(size_t)selectedColumn)->enabled,juce::dontSendNotification);
}
void VirtualCortexEditor::refreshSlotControls(){const auto p=processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn);const float values[]={p.drive,p.mix,p.time,p.feedback,p.rate,p.depth};for(int i=0;i<6;++i)if(!slotSliders[(size_t)i].isMouseButtonDown())slotSliders[(size_t)i].setValue(values[i],juce::dontSendNotification);irName.setText(processor.getCabinetIRName(),juce::dontSendNotification);gridIRName.setText(processor.getGridCabinetIRName((size_t)selectedRow,(size_t)selectedColumn),juce::dontSendNotification);neuralName.setText(processor.getGridNeuralModelName((size_t)selectedRow,(size_t)selectedColumn),juce::dontSendNotification);}
void VirtualCortexEditor::commitSlotControls(){vc::SlotParameters p;p.drive=(float)slotSliders[0].getValue();p.mix=(float)slotSliders[1].getValue();p.time=(float)slotSliders[2].getValue();p.feedback=(float)slotSliders[3].getValue();p.rate=(float)slotSliders[4].getValue();p.depth=(float)slotSliders[5].getValue();processor.setGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn,p);}
void VirtualCortexEditor::timerCallback(){updateView();refreshSlotControls();}
void VirtualCortexEditor::updateView(){refreshGridControls();const int type=currentType();const bool gm=gridMode();if(type==lastView&&gm==lastGridMode)return;lastView=type;lastGridMode=gm;for(auto& k:knobs)k.slider.setVisible(false);for(auto& s:slotSliders)s.setVisible(false);for(auto& s:switches)s.button.setVisible(false);
 switches[6].button.setVisible(true);switches[9].button.setVisible(true);
 const auto colour=CortexBlockButton::colourFor(type);
 if(!gm||masterPage){for(int i:visibleKnobs()){knobs[(size_t)i].slider.setVisible(true);knobs[(size_t)i].slider.setColour(juce::Slider::rotarySliderFillColourId,colour);knobs[(size_t)i].slider.setEnabled(!(gm&&masterPage));}int enable=-1;switch(type){case 1:enable=0;break;case 2:enable=1;break;case 3:enable=2;break;case 4:enable=7;break;case 5:enable=3;break;case 6:enable=4;break;case 7:enable=5;break;}if(enable>=0)switches[(size_t)enable].button.setVisible(true);switches[8].button.setVisible(type==3&&!gm);}
 else for(int i:visibleSlotControls()){slotSliders[(size_t)i].setVisible(true);slotSliders[(size_t)i].setColour(juce::Slider::rotarySliderFillColourId,colour);}
 slotEnabled.setVisible(gm&&!masterPage&&type!=0);loadIRButton.setVisible(!gm&&type==3);irName.setVisible(!gm&&type==3);loadGridIRButton.setVisible(gm&&type==3);gridIRName.setVisible(gm&&type==3);loadNeuralButton.setVisible(gm&&type==2);neuralName.setVisible(gm&&type==2);addButton.setEnabled(browserType<8);removeButton.setEnabled(!masterPage);
 for(int i=0;i<8;++i)browserButtons[(size_t)i].setToggleState(i+1==browserType,juce::dontSendNotification);
 resized();repaint();
}
void VirtualCortexEditor::resized(){const int w=getWidth(),h=getHeight();const int left=210,mainWidth=w-left-16;
 browserArea={12,96,186,h-124};gridArea={left,98,mainWidth,juce::jlimit(180,240,(int)(h*.31f))};stompArea={left,h-108,mainWidth,78};parameterArea={left,gridArea.getBottom()+12,mainWidth,stompArea.getY()-gridArea.getBottom()-22};
 presetBox.setBounds(left,52,mainWidth-296,32);savePresetButton.setBounds(w-296,52,66,32);loadPresetButton.setBounds(w-222,52,66,32);switches[6].button.setBounds(w-145,52,120,32);switches[9].button.setBounds(w-168,9,150,32);
 for(int i=0;i<8;++i)browserButtons[(size_t)i].setBounds(22,142+i*37,166,31);addButton.setBounds(22,h-74,166,30);
 const float rowHeight=gridArea.getHeight()/4.f,cellWidth=(mainWidth-100)/8.f;
 for(int r=0;r<4;++r){const int yy=gridArea.getY()+(int)(r*rowHeight);laneButtons[(size_t)r].setBounds(left+2,yy+8,43,28);for(int c=0;c<8;++c)gridCells[(size_t)r][(size_t)c].setBounds(left+48+(int)(c*cellWidth),yy, (int)cellWidth,(int)rowHeight-2);}
 const auto p=parameterArea;const int controlsW=p.getWidth()-174,availableH=juce::jmax(66,p.getHeight()-56);
 const auto indices=gridMode()&&!masterPage?visibleSlotControls():visibleKnobs();const int count=juce::jmax(1,(int)indices.size());const int cw=controlsW/count;
 for(int j=0;j<(int)indices.size();++j){const int i=indices[(size_t)j];auto bounds=juce::Rectangle<int>(p.getX()+j*cw+8,p.getY()+49,cw-16,availableH-8);if(gridMode()&&!masterPage)slotSliders[(size_t)i].setBounds(bounds);else knobs[(size_t)i].slider.setBounds(bounds);}
 for(auto& s:switches)if(&s!=&switches[6]&&&s!=&switches[9])s.button.setBounds(p.getRight()-130,p.getY()+4,120,28);
 switches[8].button.setBounds(p.getRight()-150,p.getY()+35,140,25);slotEnabled.setBounds(p.getRight()-130,p.getY()+4,120,28);removeButton.setBounds(p.getRight()-158,p.getBottom()-35,145,26);
 for(auto* b:{&loadIRButton,&loadGridIRButton,&loadNeuralButton})b->setBounds(p.getRight()-158,p.getY()+65,145,29);
 for(auto* l:{&irName,&gridIRName,&neuralName})l->setBounds(p.getRight()-160,p.getY()+96,150,24);
 const int sw=(mainWidth-100)/8;for(int c=0;c<8;++c)stomps[(size_t)c].setBounds(left+100+c*sw,stompArea.getY(),sw-4,stompArea.getHeight());statusLabel.setBounds(left,h-26,mainWidth-8,22);
}
void VirtualCortexEditor::paint(juce::Graphics& g){g.fillAll(background);const int w=getWidth(),h=getHeight();
 drawCaption(g,"VIRTUAL CORTEX",{18,10,260,28},19.f);drawCaption(g,"6.1  /  GRID WORKSPACE",{282,15,290,20},11.f,muted);g.setColour(juce::Colour(0xff292b2e));g.drawHorizontalLine(43,12.f,(float)w-12.f);
 drawCaption(g,"DEVICE BROWSER",{22,102,166,22},12.f);drawCaption(g,"Select a type, then add to slot",{22,123,166,16},10.f,muted);
 g.setColour(panel);g.fillRoundedRectangle(browserArea.toFloat(),6.f);drawCaption(g,"DEVICE BROWSER",{22,102,166,22},12.f);drawCaption(g,"Original effects",{22,124,166,15},10.f,muted);
 drawCaption(g,"SERIAL PRESET",{18,51,180,18},10.f,muted);drawCaption(g,"Amp / effects / cabinet IR",{18,70,180,18},11.f);
 const auto grid=processor.getEditableGrid();const float rowHeight=gridArea.getHeight()/4.f;
 for(int r=0;r<4;++r){const float yy=gridArea.getY()+r*rowHeight+rowHeight*.5f-5.f;g.setColour(juce::Colour(processor.isGridLaneEnabled((size_t)r)?0xff656a70:0xff282c30));g.drawLine((float)gridArea.getX()+43,yy,(float)gridArea.getRight()-40,yy,1.f);drawCaption(g,"OUT",{gridArea.getRight()-36,(int)yy-12,34,18},9.f,muted,juce::Justification::centred);drawCaption(g,"1/2",{gridArea.getRight()-36,(int)yy+2,34,16},9.f,muted,juce::Justification::centred);}
 const auto p=parameterArea;g.setColour(panel);g.fillRoundedRectangle(p.toFloat(),6.f);g.setColour(juce::Colour(0xff303338));g.drawHorizontalLine(p.getY()+33,(float)p.getX(),(float)p.getRight());const auto colour=CortexBlockButton::colourFor(currentType());
 juce::String title=masterPage?"Output":CortexBlockButton::nameFor(currentType());const juce::String context=gridMode()?"  /  ROW "+juce::String(selectedRow+1)+"  SLOT "+juce::String(selectedColumn+1):"  /  SERIAL ENGINE";drawCaption(g,title+context,{p.getX()+14,p.getY()+4,p.getWidth()-160,26},14.f,colour);
 const auto indices=gridMode()&&!masterPage?visibleSlotControls():visibleKnobs();for(int i:indices){const auto b=gridMode()&&!masterPage?slotSliders[(size_t)i].getBounds():knobs[(size_t)i].slider.getBounds();drawCaption(g,gridMode()&&!masterPage?slotLabels[(size_t)i]:knobs[(size_t)i].label,{b.getX(),p.getY()+36,b.getWidth(),16},10.f,juce::Colours::white,juce::Justification::centred);}
 if(indices.empty()){juce::String help=currentType()==0?"Choose a device in the browser and add it to this slot.":currentType()==3?"Load a WAV impulse response for this cabinet slot.":"This grid effect uses fixed settings in the current DSP engine.";drawCaption(g,help,{p.getX()+20,p.getY()+51,p.getWidth()-200,p.getHeight()-64},12.f,muted);}
 if(masterPage&&gridMode())drawCaption(g,"Trim applies to Serial engine",{p.getX()+150,p.getY()+65,260,28},11.f,muted);
 drawCaption(g,"STOMP",{stompArea.getX()+7,stompArea.getY()+10,90,20},12.f);drawCaption(g,"ROW "+juce::String(selectedRow+1),{stompArea.getX()+7,stompArea.getY()+32,90,18},11.f,muted);
 drawCaption(g,gridMode()?"GRID ACTIVE":"SERIAL ACTIVE",{18,h-25,182,20},10.f,gridMode()?mint:muted);if(statusLabel.getText().isEmpty())drawCaption(g,"Click a block to edit  /  A-H toggle slots in the selected row",{gridArea.getX(),h-25,gridArea.getWidth(),20},10.f,muted);
}
void VirtualCortexEditor::applyPreset(int index){auto set=[this](const char* id,float value){if(auto* p=processor.parameters.getParameter(id))p->setValueNotifyingHost(p->convertTo0to1(value));};if(index<2)return;set("gridMode",0.f);
 if(index==2){set("drive",.12f);set("tone",.7f);set("level",.65f);set("delayMix",.12f);set("reverbMix",.25f);set("ampBias",0.f);set("delayOn",1.f);set("reverbOn",1.f);}
 if(index==3){set("drive",.48f);set("tone",.56f);set("level",.48f);set("delayMix",0.f);set("reverbMix",.04f);set("ampBias",.08f);set("delayOn",0.f);set("reverbOn",0.f);}
 if(index==4){set("drive",.73f);set("tone",.62f);set("level",.38f);set("delayTime",390.f);set("delayFeedback",.32f);set("delayMix",.22f);set("reverbMix",.15f);set("delayOn",1.f);set("reverbOn",1.f);}lastView=-1;updateView();}
juce::AudioProcessorEditor* VirtualCortexProcessor::createEditor(){return new VirtualCortexEditor(*this);}
