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
 if(id.startsWith("stereo-page-")){g.setColour(panel);g.fillRoundedRectangle(r,4.f);if(b.getToggleState()){g.setColour(mint);g.fillRect(r.withHeight(2.f));}return;}
 if(id.startsWith("device-")){if(b.getToggleState()||over){g.setColour(juce::Colour(0xff111212));g.fillRoundedRectangle(r,6.f);}return;}
 g.setColour(juce::Colour(b.getToggleState()&&id.startsWith("quick-")?0xffffd545:(over?0xff292929:0xff1d1d1d)));g.fillRoundedRectangle(r.reduced(.5f),id.startsWith("lane-")?16.f:5.f);
}
void CortexLookAndFeel::drawButtonText(juce::Graphics& g,juce::TextButton& b,bool,bool){
 const auto id=b.getComponentID();auto r=b.getLocalBounds();
 if(id=="pedal"){g.setColour(juce::Colours::white);juce::Path foot;foot.startNewSubPath(20,23);foot.lineTo(25,15);foot.lineTo(35,12);foot.lineTo(38,7);foot.lineTo(43,10);foot.lineTo(45,18);foot.lineTo(36,22);foot.lineTo(33,26);foot.lineTo(20,26);foot.closeSubPath();g.strokePath(foot,juce::PathStrokeType(1.f));g.drawLine(17,29,46,29,1.f);return;}
 if(id=="save"){g.setColour(muted);auto box=r.toFloat().reduced(9.f);g.drawRect(box,1.5f);g.drawRect(box.reduced(4.f,0).withHeight(7.f),1.f);g.drawRect(box.reduced(4.f,0).withY(box.getBottom()-8).withHeight(8.f),1.f);return;}
 if(id=="open"){drawCaption(g,"...",r,21.f,muted,juce::Justification::centred);return;}
 const auto c=id.startsWith("stereo-page-")&&b.getToggleState()?mint:b.getToggleState()&&id.startsWith("quick-")?juce::Colours::black:(id.startsWith("device-")&&b.getToggleState()?juce::Colour(0xff8bf879):(id=="directory"?muted:juce::Colours::white));
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
 const juce::uint32 values[]={0xff41454a,0xffd8a944,0xfff25562,0xff57d3bf,0xff4097ef,0xff43d8c9,0xff61fbd8,0xff9265ff,0xffe6ad55,0xffee8641,0xffe6546d,0xffb68af5,0xff829cff,0xffda88de,0xffd8bf55,0xff64bed6,0xffae81ed,0xffe28aaf,0xffa0b99c,0xff6fb8df,0xff75a5ec,0xff6bdbcb,0xffa0a7ad,0xff61fbd8,0xff61fbd8,0xffd8a944,0xffd8a944,0xff749bff,0xffeac967,0xff63dab5};
 return juce::Colour(values[juce::jlimit(0,29,type)]);
}
juce::String CortexBlockButton::nameFor(int type){
 const char* names[]={"Empty","Gate","Amp","Cabinet","EQ","Delay","Reverb","Tremolo","Compressor","Overdrive","Distortion","Chorus","Flanger","Phaser","Wah","Bit-crusher","Vibrato","Ring modulator","Gain","Low / High Cut","Tone EQ","Room Reverb","Output","Stereo Delay","Stereo Reverb","Four-head Echo","Interval Echo","Layer Loom","Prism Synth","Step Crush"};
 return names[juce::jlimit(0,29,type)];
}
void CortexBlockButton::drawIcon(juce::Graphics& g,juce::Rectangle<float> r,int type,juce::Colour colour){
 g.setColour(colour);const auto cx=r.getCentreX(),cy=r.getCentreY();const auto u=r.getWidth()/24.f;
 juce::Path p;
 if(type==0){g.drawLine(cx-5*u,cy,cx+5*u,cy,1.2f);g.drawLine(cx,cy-5*u,cx,cy+5*u,1.2f);return;}
 if(type>=16&&type<=21){const char* symbols[]={"VI","RM","GN","CUT","EQ","RV"};drawCaption(g,symbols[type-16],r.toNearestInt(),r.getWidth()*.38f,colour,juce::Justification::centred,true);return;}
 if(type>=11&&type<=15){const char* symbols[]={"CH","FL","PH","WA","BC"};drawCaption(g,symbols[type-11],r.toNearestInt(),r.getWidth()*.45f,colour,juce::Justification::centred,true);return;}
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
 // Preserve the reference rounded-square silhouette in compact and expanded rows.
 const float corner=side*.2f,outline=side*(selected?.08f:.027f);
 g.setColour(selected?colour.withAlpha(.13f):juce::Colour(0xff101111));g.fillRoundedRectangle(box,corner);
 g.setColour(blockEnabled?colour:colour.withAlpha(.3f));g.drawRoundedRectangle(box,corner,outline);
 drawIcon(g,box.reduced(side*.24f),type,blockEnabled?juce::Colours::white:muted.withAlpha(.55f));
 if(type){const auto badge=juce::Rectangle<float>(side*.38f,side*.38f).withCentre({box.getX()-1,box.getY()-1});g.setColour(juce::Colours::white);g.fillEllipse(badge);drawCaption(g,caption,badge.toNearestInt(),side*.24f,juce::Colours::black,juce::Justification::centred);}
}
VirtualCortexEditor::VirtualCortexEditor(VirtualCortexProcessor& p):AudioProcessorEditor(&p),processor(p){
 setLookAndFeel(&look);
 const std::array<const char*,107> ids={"gate","drive","tone","level","cab","delayTime","delayFeedback","delayMix","reverbMix","ampBias","inputHP","cabCut","tremRate","tremDepth","outputTrim","eqLow","eqMid","eqHigh","sdTimeL","sdTimeR","sdFeedback","sdCross","sdLowCut","sdHighCut","sdMix","sdModRate","sdModDepth","sdDucking","sdDuckRelease","sdWidth","sdWetLevel","sdGlide","sdBpm","sdBeatsL","sdBeatsR","sdFilterMix","srDecay","srSize","srPreDelay","srDiffusion","srEarly","srMix","srLowDecay","srHighDecay","srLowFreq","srHighFreq","srModRate","srModDepth","srWidth","srDucking","srRelease","srWetLevel","srInputHP","srInputLP","sdDiffusion","mh1Time","mh1Level","mh1Pan","mh1Feedback","mh2Time","mh2Level","mh2Pan","mh2Feedback","mh3Time","mh3Level","mh3Pan","mh3Feedback","mh4Time","mh4Level","mh4Pan","mh4Feedback","mhMix","ieTime","ieFeedback","ieA","ieB","ieStep","ieCutoff","ieMix","llLevel","llRate","llCutoff","syMix","syTranspose","syDetune","syCutoff","syResonance","syAttack","syRelease","syGlide","syWave","syGate","scBits","scRate","scCutoff","scStepMs","scDepth","scMix","scMode","scStutterMs","scStutter","scStep1","scStep2","scStep3","scStep4","scStep5","scStep6"};
 const std::array<const char*,107> labels={"THRESHOLD","DRIVE","TONE","LEVEL","CAB MIX","TIME","FEEDBACK","MIX","MIX","BIAS","INPUT HIGH PASS","LOW PASS","RATE","DEPTH","OUTPUT TRIM","BASS","MID","TREBLE","LEFT TIME","RIGHT TIME","FEEDBACK","CROSS FEEDBACK","LOOP HIGH PASS","LOOP LOW PASS","MIX","MOD RATE","MOD DEPTH","DUCKING","DUCK RELEASE","WIDTH","WET LEVEL","GLIDE TIME","MANUAL TEMPO","LEFT BEATS","RIGHT BEATS","LOOP FILTER MIX","DECAY","SIZE","PRE DELAY","DIFFUSION","EARLY BALANCE","MIX","LOW DECAY RATIO","HIGH DECAY RATIO","LOW CROSSOVER","HIGH CROSSOVER","MOD RATE","MOD DEPTH","WIDTH","DUCKING","DUCK RELEASE","WET LEVEL","INPUT HIGH PASS","INPUT LOW PASS","LOOP DIFFUSION","TIME","LEVEL","PAN","FEEDBACK","TIME","LEVEL","PAN","FEEDBACK","TIME","LEVEL","PAN","FEEDBACK","TIME","LEVEL","PAN","FEEDBACK","MIX","TIME","FEEDBACK","INTERVAL A","INTERVAL B","STEP TIME","LOW PASS","MIX","LOOP LEVEL","SPEED","LOW PASS","MIX","TRANSPOSE","DETUNE","FILTER","RESONANCE","ATTACK","RELEASE","GLIDE","WAVE","TRACKING GATE","BITS","SAMPLE RATE","LOW PASS","STEP TIME","SEQ DEPTH","MIX","SEQ TARGET","STUTTER TIME","STUTTER MIX","STEP 1","STEP 2","STEP 3","STEP 4","STEP 5","STEP 6"};
 for(size_t i=0;i<knobs.size();++i){auto& k=knobs[i];k.label=labels[i];k.slider.setComponentID(ids[i]);if(i==18||i==19)k.slider.setTooltip("Manual milliseconds; Tempo Sync uses beats on Dynamics / Tempo.");if(i==21)k.slider.setTooltip("Manual cross-feedback; Ping Pong forces full cross-feedback.");k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);k.slider.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);addChildComponent(k.slider);k.attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,ids[i],k.slider);
  k.slider.valueFromTextFunction=[i](const juce::String& text){double v=text.getDoubleValue();if(i>=82)return (i==82||i==96||i==97||i==100)?v/100.:v;if(i>=72)return (i==73||i==78||i==79)?v/100.:v;if(i>=55)return ((i-55)%4==0&&i<71)?v:v/100.;if(i==54)return v/100.;if(i>=36)return (i==37||i==39||i==40||i==41||i==48||i==49)?v/100.:v;if(i>=18)return (i==20||i==21||i==24||i==27||i==29||i==35)?v/100.:v;return (i==0||i>=15||i==5||i==10||i==11||i==12||i==9)?v:v/100.;};
  k.slider.textFromValueFunction=[i](double v){if(i>=82){if(i==82||i==96||i==97||i==100)return juce::String(v*100,1)+" %";if(i==83)return juce::String(v,0)+" st";if(i==84)return juce::String(v,1)+" ct";if(i==85||i==93||i==94)return juce::String(v,0)+" Hz";if(i==87||i==88||i==89||i==95||i==99)return juce::String(v,0)+" ms";if(i==91)return juce::String(v,0)+" dB";if(i==98)return juce::String(v,0)+(v<.5?" Rate":v<1.5?" Filter":" Bits");if(i==90)return juce::String(v,1)+" wave";return juce::String(v,2);}if(i>=72){if(i==73||i==78||i==79)return juce::String(v*100,1)+" %";if(i==74||i==75)return juce::String(v,1)+" st";if(i==80)return juce::String(v,2)+" x";return juce::String(v,1)+(i==72||i==76?" ms":" Hz");}if(i>=55)return ((i-55)%4==0&&i<71)?juce::String(v,1)+" ms":juce::String(v*100,1)+" %";if(i==54)return juce::String(v*100,1)+" %";if(i>=36){if(i==37||i==39||i==40||i==41||i==48||i==49)return juce::String(v*100,1)+" %";if(i==38||i==47||i==50)return juce::String(v,1)+" ms";if(i==44||i==45||i==46||i==52||i==53)return juce::String(v,1)+" Hz";if(i==51)return juce::String(v,1)+" dB";if(i==36)return juce::String(v,2)+" s";return juce::String(v,2)+" x";}if(i>=18){if(i==20||i==21||i==24||i==27||i==29||i==35)return juce::String(v*100,1)+" %";if(i==18||i==19||i==26||i==28||i==31)return juce::String(v,1)+" ms";if(i==22||i==23||i==25)return juce::String(v,1)+" Hz";if(i==30)return juce::String(v,1)+" dB";if(i==32)return juce::String(v,1)+" BPM";return juce::String(v,3)+" beats";}if(i==0||i>=15)return juce::String(v,1)+" dB";if(i==5)return juce::String(v,0)+" ms";if(i==10||i==11)return juce::String(v,0)+" Hz";if(i==12)return juce::String(v,2)+" Hz";if(i==9)return juce::String(v,2);return juce::String(v*100,1)+" %";};
  k.value.setComponentID(juce::String(ids[i])+"Value");k.value.setFont(juce::Font(juce::FontOptions(15.f)));k.value.setBorderSize(juce::BorderSize<int>(0));k.value.setColour(juce::Label::textColourId,juce::Colours::white);k.value.setEditable(false,true,false);addChildComponent(k.value);
  k.slider.onValueChange=[this,i]{auto& item=knobs[i];if(!item.value.isBeingEdited())item.value.setText(item.slider.getTextFromValue(item.slider.getValue()),juce::dontSendNotification);};
  k.value.onTextChange=[this,i]{auto& item=knobs[i];item.slider.setValue(item.slider.getValueFromText(item.value.getText()),juce::sendNotificationSync);};
  k.value.setText(k.slider.getTextFromValue(k.slider.getValue()),juce::dontSendNotification);
 }
 const std::array<const char*,23> switchesIds={"gateOn","ampOn","cabOn","delayOn","reverbOn","tremOn","bypass","eqOn","irOn","gridMode","stereoDelayOn","sdPingPong","sdTape","sdSync","stereoReverbOn","srPreGlide","mhOn","ieOn","llOn","llReverse","syOn","syMidi","scOn"};
 const std::array<const char*,23> switchesLabels={"ENABLED","ENABLED","ENABLED","ENABLED","ENABLED","ENABLED","BYPASS","ENABLED","USE IR","GRID ENGINE","ENABLED","PING PONG","TAPE GLIDE","TEMPO SYNC","ENABLED","PRE GLIDE","ENABLED","ENABLED","ENABLED","REVERSE","ENABLED","MIDI NOTES","ENABLED"};
 for(size_t i=0;i<switches.size();++i){auto& s=switches[i];s.button.setButtonText(switchesLabels[i]);addChildComponent(s.button);s.attachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.parameters,switchesIds[i],s.button);}
 presetBox.addItem("Clean ambience",2);presetBox.addItem("Crunch rhythm",3);presetBox.addItem("Lead delay",4);presetBox.setTextWhenNothingSelected("1A Virtual Cortex MAIN");presetBox.onChange=[this]{applyPreset(presetBox.getSelectedId());};addAndMakeVisible(presetBox);
 for(auto* b:{&savePresetButton,&loadPresetButton,&addButton,&removeButton,&loadIRButton,&loadGridIRButton,&loadNeuralButton})addAndMakeVisible(*b);
 for(auto* l:{&irName,&gridIRName,&neuralName,&statusLabel}){l->setColour(juce::Label::textColourId,muted);l->setFont(juce::Font(juce::FontOptions(11.f)));addAndMakeVisible(*l);}
 for(int i=0;i<16;++i){auto& b=browserButtons[(size_t)i];b.rail=true;b.type=i==15?22:i+1;b.setTooltip(CortexBlockButton::nameFor(b.type));b.onClick=[this,i]{browserType=browserButtons[(size_t)i].type;variant=0;masterPage=(i==15);lastView=-1;updateView();};addAndMakeVisible(b);}
 addButton.onClick=[this]{insertBlock(browserType);};removeButton.setButtonText("");removeButton.setComponentID("edit-menu");removeButton.setTooltip("Selected block options");
 removeButton.onClick=[this]{juce::PopupMenu menu;menu.addItem(1,"Remove selected block");menu.addItem(2,"Toggle selected block bypass");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&removeButton),[safe](int item){if(!safe)return;if(item==1)safe->insertBlock(0);if(item==2)safe->toggleSelectedSlot();});};
 slotEnabled.onClick=[this]{toggleSelectedSlot();};addAndMakeVisible(slotEnabled);
 repeatFxButton.setComponentID("repeat-fx");repeatFxButton.setTooltip("Filter the repeat return path; dry input and first repeat stay unfiltered. Grid Delay only.");repeatFxButton.onClick=[this]{auto p=processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn);p.repeatFx=repeatFxButton.getToggleState();processor.setGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn,p);lastView=-1;updateView();};addChildComponent(repeatFxButton);
 for(int r=0;r<4;++r){auto& lane=laneButtons[(size_t)r];lane.setButtonText("ROW "+juce::String(r+1));lane.setComponentID("lane-"+juce::String(r));lane.setTooltip("Enable parallel row: same input, averaged with other enabled rows in Grid mode");lane.setClickingTogglesState(true);lane.onClick=[this,r]{processor.setGridLaneEnabled((size_t)r,laneButtons[(size_t)r].getToggleState());refreshGridControls();};addAndMakeVisible(lane);
  for(int c=0;c<8;++c){auto& b=gridCells[(size_t)r][(size_t)c];b.setComponentID("grid-"+juce::String(r)+"-"+juce::String(c));b.onClick=[this,r,c]{selectSlot(r,c);};addAndMakeVisible(b);}
 }
 for(int c=0;c<8;++c){auto& b=stomps[(size_t)c];b.stomp=true;b.caption=juce::String::charToString((juce::juce_wchar)('A'+c));b.onClick=[this,c]{selectSlot(selectedRow,c);toggleSelectedSlot();};addAndMakeVisible(b);}
 for(int i=0;i<6;++i){auto& s=slotSliders[(size_t)i];s.setRange(0.,1.,.001);s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);s.setComponentID("slot-param-"+juce::String(i));s.valueFromTextFunction=[this,i](const juce::String& text){double v=text.getDoubleValue();if(i>=82)return (i==82||i==96||i==97||i==100)?v/100.:v;if(i>=72)return (i==73||i==78||i==79)?v/100.:v;if(i>=55)return ((i-55)%4==0&&i<71)?v:v/100.;
   const int type=currentType();
   if(type==5&&processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn).repeatFx){if(i==0)return (v-20.)/980.;if(i==5)return (v-1000.)/19000.;if(i==4)return v/100.;}
   if(type==17){if(i==4)return std::log(juce::jlimit(10.,2000.,v)/10.)/std::log(200.);return v/100.;}
   if(type==18){if(i==0)return (v+60.)/72.;return text.containsIgnoreCase("inv")?1.:0.;}
   if(type==19){if(i==0)return (v-20.)/980.;if(i==2)return (v-1000.)/19000.;return v/100.;}
   if(type==20){if(i==0||i==2||i==3)return (v+12.)/24.;if(i==4)return (v-100.)/3900.;if(i==5)return (v-.3)/3.7;return v/100.;}
   if(type==21){if(i==2)return (v-.2)/7.8;if(i==4)return v/100.;return v/100.;}
   if((type>=11&&type<=13)||type==16){if(i==4)return (v-((type==11||type==16)?.1:.05))/((type==11||type==16)?9.9:4.95);if(i==2)return (type==11||type==16)?(v-5.)/20.:type==12?(v-.2)/5.:(v-150.)/1350.;return v/100.;}
   if(type==14){if(i==4)return (v-2.)/98.;if(i==5)return (v-50.)/950.;if(i==3)return (v-.5)/4.5;return v/100.;}
   if(type==15){if(i==0)return (v-4.)/12.;if(i==2)return (v-1.)/63.;return v/(i==5?200.:100.);}
   if(currentType()==8){if(i==0)return (v+48.)/48.;if(i==3)return (v-1.)/19.;if(i==4)return (v-.1)/99.9;if(i==5)return (v-10.)/990.;if(i==2)return v/24.;return v/100.;}
   if(currentType()==9||currentType()==10){if(i==0)return v/36.;if(i==4)return (v-500.)/9500.;if(i==5)return v/200.;return v/100.;}if(i==2)return (v-20.)/480.;if(i==4)return (v-.1)/9.9;return v/(i==3?90.:100.);};s.textFromValueFunction=[this,i](double v){
   const int type=currentType();
   if(type==5&&processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn).repeatFx){if(i==0)return juce::String(20.+980.*v,0)+" Hz";if(i==5)return juce::String(1000.+19000.*v,0)+" Hz";if(i==4)return juce::String(v*100.,1)+" %";}
   if(type==17){if(i==4)return juce::String(10.*std::pow(200.,v),1)+" Hz";return juce::String(v*100.,1)+" %";}
   if(type==18){if(i==0)return juce::String(-60.+72.*v,1)+" dB";return juce::String(v>=.5?"Inverted":"Normal");}
   if(type==19){if(i==0)return juce::String(20.+980.*v,0)+" Hz";if(i==2)return juce::String(1000.+19000.*v,0)+" Hz";return juce::String(v*100.,1)+" %";}
   if(type==20){if(i==0||i==2||i==3)return juce::String(-12.+24.*v,1)+" dB";if(i==4)return juce::String(100.+3900.*v,0)+" Hz";if(i==5)return juce::String(.3+3.7*v,2)+" Q";return juce::String(v*100.,1)+" %";}
   if(type==21){if(i==2)return juce::String(.2+7.8*v,2)+" s";if(i==4)return juce::String(v*100.,1)+" ms";return juce::String(v*100.,1)+" %";}
   if((type>=11&&type<=13)||type==16){if(i==4)return juce::String(((type==11||type==16)?.1:.05)+((type==11||type==16)?9.9:4.95)*v,2)+" Hz";if(i==2)return type==13?juce::String(150.+1350.*v,0)+" Hz":juce::String(((type==11||type==16)?5.:.2)+((type==11||type==16)?20.:5.)*v,1)+" ms";return juce::String(v*100.,1)+" %";}
   if(type==14){if(i==4)return juce::String(2.+98.*v,1)+" ms";if(i==5)return juce::String(50.+950.*v,0)+" ms";if(i==3)return juce::String(.5+4.5*v,2)+" Q";return juce::String(v*100.,1)+" %";}
   if(type==15){if(i==0)return juce::String(4+(int)std::round(12.*v))+" bits";if(i==2)return juce::String(1+(int)std::round(63.*v))+" samples";return juce::String(v*(i==5?200.:100.),1)+" %";}
   if(currentType()==8){if(i==0)return juce::String(-48.+48.*v,1)+" dB";if(i==3)return juce::String(1.+19.*v,1)+":1";if(i==4)return juce::String(.1+99.9*v,1)+" ms";if(i==5)return juce::String(10.+990.*v,0)+" ms";if(i==2)return juce::String(24.*v,1)+" dB";return juce::String(v*100.,1)+" %";}
   if(currentType()==9||currentType()==10){if(i==0)return juce::String(v*36.,1)+" dB";if(i==4)return juce::String(500.+9500.*v,0)+" Hz";if(i==5)return juce::String(v*200.,1)+" %";return juce::String(v*100.,1)+" %";}if(i==2)return juce::String(20.+480.*v,0)+" ms";if(i==4)return juce::String(.1+9.9*v,2)+" Hz";return juce::String(v*(i==3?90.:100.),1)+" %";};s.onValueChange=[this]{commitSlotControls();};addChildComponent(s);
  auto& value=slotValues[(size_t)i];value.setFont(juce::Font(juce::FontOptions(15.f)));value.setBorderSize(juce::BorderSize<int>(0));value.setColour(juce::Label::textColourId,juce::Colours::white);value.setEditable(false,true,false);value.onTextChange=[this,i]{slotSliders[(size_t)i].setValue(slotSliders[(size_t)i].getValueFromText(slotValues[(size_t)i].getText()),juce::sendNotificationSync);};addChildComponent(value);}
 loadIRButton.onClick=[this]{irChooser=std::make_unique<juce::FileChooser>("Load cabinet IR",juce::File{},"*.wav;*.aiff;*.aif");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);irChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe](const juce::FileChooser& c){if(safe&&c.getResult().existsAsFile()){safe->statusLabel.setText(safe->processor.loadCabinetIR(c.getResult())?"Cabinet IR loaded":"Could not load IR",juce::dontSendNotification);safe->refreshSlotControls();}});};
 loadGridIRButton.onClick=[this]{const auto r=selectedRow,c=selectedColumn;irChooser=std::make_unique<juce::FileChooser>("Load selected cabinet WAV IR",juce::File{},"*.wav");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);irChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe,r,c](const juce::FileChooser& chooser){if(safe&&chooser.getResult().existsAsFile()){bool ok=safe->processor.loadGridCabinetIR(chooser.getResult(),(size_t)r,(size_t)c);safe->statusLabel.setText(ok?"Slot IR loaded":"IR rejected: WAV, max 4096 taps",juce::dontSendNotification);safe->refreshSlotControls();}});};
 loadNeuralButton.onClick=[this]{const auto r=selectedRow,c=selectedColumn;neuralChooser=std::make_unique<juce::FileChooser>("Load VCMLP1 amplifier model",juce::File{},"*.vcmlp;*.txt");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);neuralChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe,r,c](const juce::FileChooser& chooser){if(safe&&chooser.getResult().existsAsFile()){bool ok=safe->processor.loadGridNeuralModel(chooser.getResult(),(size_t)r,(size_t)c);safe->statusLabel.setText(ok?"Model loaded":"Choose an Amp slot and VCMLP1 model",juce::dontSendNotification);safe->refreshSlotControls();}});};
 savePresetButton.onClick=[this]{presetChooser=std::make_unique<juce::FileChooser>("Save preset",juce::File{},"*.vcpreset");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);presetChooser->launchAsync(juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles|juce::FileBrowserComponent::warnAboutOverwriting,[safe](const juce::FileChooser& c){if(!safe||c.getResult()==juce::File{})return;juce::MemoryBlock data;safe->processor.getStateInformation(data);bool ok=c.getResult().withFileExtension(".vcpreset").replaceWithData(data.getData(),data.getSize());safe->statusLabel.setText(ok?"Preset saved":"Preset save failed",juce::dontSendNotification);});};
 loadPresetButton.onClick=[this]{presetChooser=std::make_unique<juce::FileChooser>("Open preset",juce::File{},"*.vcpreset");auto safe=juce::Component::SafePointer<VirtualCortexEditor>(this);presetChooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[safe](const juce::FileChooser& c){if(!safe||!c.getResult().existsAsFile()||c.getResult().getSize()>1024*1024)return;juce::MemoryBlock data;if(c.getResult().loadFileAsData(data)){safe->processor.setStateInformation(data.getData(),(int)data.getSize());safe->refreshSlotControls();safe->lastView=-1;safe->updateView();safe->statusLabel.setText("Preset loaded",juce::dontSendNotification);}});};
 savePresetButton.setComponentID("save");loadPresetButton.setComponentID("open");directoryButton.setComponentID("directory");
 for(size_t i=0;i<switches.size();++i)if(i!=8&&i!=9&&i<11){switches[i].button.setComponentID(i==6?"bypass":"power");switches[i].button.setTooltip(i==6?"Global bypass":"Enable selected effect");}
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
 trackingPageButton.setComponentID("tracking-page");trackingPageButton.onClick=[this]{trackingPage=1-trackingPage;lastView=-1;updateView();};addAndMakeVisible(trackingPageButton);
 categoryBox.setComponentID("device-category");for(int type=1;type<=29;++type)categoryBox.addItem(CortexBlockButton::nameFor(type),type);categoryBox.setSelectedId(browserType,juce::dontSendNotification);categoryBox.onChange=[this]{browserType=categoryBox.getSelectedId();variant=0;masterPage=browserType>=22;lastView=-1;updateView();};addAndMakeVisible(categoryBox);
 for(int i=0;i<4;++i){auto& b=echoPages[i];b.setButtonText("HEAD "+juce::String(i+1));b.setComponentID("echo-head-"+juce::String(i));b.onClick=[this,i]{echoPage=i;lastView=-1;updateView();};addChildComponent(b);}
 const char* loopNames[]={"REC / DUB","STOP","PLAY","UNDO","REDO","CLEAR"};const unsigned commands[]={1,2,4,8,16,32};for(int i=0;i<6;++i){auto& b=loopButtons[i];b.setButtonText(loopNames[i]);b.setComponentID("loop-command-"+juce::String(i));const auto command=commands[i];b.onClick=[this,command]{if(command==1||command==4)processor.parameters.getParameter("llOn")->setValueNotifyingHost(1.f);processor.loopCommand(command);};addChildComponent(b);}
 const char* reverbNames[]={"SPACE","TONE / MOD","OUTPUT / DYNAMICS"};for(int i=0;i<3;++i){auto& button=reverbPages[(std::size_t)i];button.setButtonText(reverbNames[i]);button.setComponentID("stereo-page-reverb-"+juce::String(i));button.onClick=[this,i]{reverbPage=i;lastView=-1;updateView();};addChildComponent(button);}
 const char* pageNames[]={"MAIN","TONE / MOD","DYNAMICS / TEMPO"};for(int i=0;i<3;++i){auto& button=stereoPages[(std::size_t)i];button.setButtonText(pageNames[i]);button.setComponentID("stereo-page-"+juce::String(i));button.onClick=[this,i]{stereoPage=i;lastView=-1;updateView();};addChildComponent(button);}
 fullScreenButton.setComponentID("fullscreen-toggle");
 fullScreenButton.setTooltip("Full screen: F11. Exit: Esc or F11.");
 fullScreenButton.onClick=[this]{setFullScreenMode(!isFullScreenMode());};
 addChildComponent(fullScreenButton);
 fullScreenButton.setVisible(processor.wrapperType==juce::AudioProcessor::wrapperType_Standalone);
 catalogButton.setComponentID("catalog-toggle");catalogButton.setTooltip("Browse playable originals and the complete reference device inventory");
 catalogButton.onClick=[this]{catalogPanel.setVisible(!catalogPanel.isVisible());if(catalogPanel.isVisible())catalogPanel.toFront(false);};
 addAndMakeVisible(catalogButton);addChildComponent(catalogPanel);
 catalogPanel.onChooseOriginal=[this](int type){
  if(type<=21){if(!insertBlock(type))return;}
  else {browserType=type;masterPage=true;lastView=-1;updateView();}
  setEditorsVisible(true);catalogPanel.setVisible(false);
 };
 setWantsKeyboardFocus(true);
 setResizable(true,true);setResizeLimits(800,584,7680,4320);
 int width=1184,height=864;if(auto* display=juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()){height=juce::jlimit(584,864,display->userArea.getHeight()-100);width=(int)std::round(height*1184./864.);if(width>display->userArea.getWidth()-60){width=juce::jmax(800,display->userArea.getWidth()-60);height=(int)std::round(width*864./1184.);}}
 setSize(width,height);refreshSlotControls();updateView();startTimerHz(12);
}
VirtualCortexEditor::~VirtualCortexEditor(){stopTimer();setLookAndFeel(nullptr);}
bool VirtualCortexEditor::isFullScreenMode() const {
 if(processor.wrapperType!=juce::AudioProcessor::wrapperType_Standalone)return false;
 if(auto* window=dynamic_cast<juce::ResizableWindow*>(getTopLevelComponent()))return window->isFullScreen();
 return false;
}
bool VirtualCortexEditor::setFullScreenMode(bool enabled){
 if(processor.wrapperType!=juce::AudioProcessor::wrapperType_Standalone)return false;
 auto* window=dynamic_cast<juce::ResizableWindow*>(getTopLevelComponent());
 if(window==nullptr)return false;
 if(enabled==window->isFullScreen())return true;
 if(enabled)windowedBounds=window->getBounds();
 window->setFullScreen(enabled);
 if(!enabled&&!windowedBounds.isEmpty())window->setBounds(windowedBounds);
 fullScreenButton.setButtonText(enabled?"EXIT FULL SCREEN":"FULL SCREEN");
 return true;
}
bool VirtualCortexEditor::keyPressed(const juce::KeyPress& key){
 if(key.getKeyCode()==juce::KeyPress::escapeKey&&catalogPanel.isVisible()){catalogPanel.setVisible(false);return true;}
 if(key.getKeyCode()==juce::KeyPress::F11Key)return setFullScreenMode(!isFullScreenMode());
 if(key.getKeyCode()==juce::KeyPress::escapeKey&&isFullScreenMode())return setFullScreenMode(false);
 return false;
}
bool VirtualCortexEditor::gridMode() const{return processor.parameters.getRawParameterValue("gridMode")->load()>.5f;}
int VirtualCortexEditor::currentType() const{if(masterPage)return browserType;if(!gridMode())return browserType;const auto grid=processor.getEditableGrid();return (int)grid.get((size_t)selectedRow,(size_t)selectedColumn)->type;}
std::vector<int> VirtualCortexEditor::visibleKnobs() const{
 switch(currentType()){case 28:return trackingPage?std::vector<int>{87,88,89}:std::vector<int>{82,83,84,85,86,90,91};case 29:return trackingPage?std::vector<int>{99,100,101,102,103,104,105,106}:std::vector<int>{92,93,94,95,96,97,98};case 26:return {72,73,74,75,76,77,78};case 27:return {79,80,81};case 25:return {55+echoPage*4,56+echoPage*4,57+echoPage*4,58+echoPage*4,71};case 1:return {0,10};case 2:return {1,2,9,3};case 3:return {4,11};case 4:return {15,16,17};case 5:return {5,6,7};case 6:return {8};case 7:return {12,13};case 22:return {14};case 23:if(stereoPage==0)return {18,19,20,21,24,29};if(stereoPage==1)return {22,23,35,25,26,31,54};return {27,28,30,32,33,34};case 24:if(reverbPage==0)return {36,37,38,39,40,41};if(reverbPage==1)return {42,43,44,45,46,47};return {48,49,50,51,52,53};default:return {};}
}
std::vector<int> VirtualCortexEditor::visibleSlotControls() const{
 switch(currentType()){case 2:return {0};case 5:if(processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn).repeatFx)return {2,3,1,0,5,4};return {1,2,3};case 6:return {1};case 7:return {4,5};case 8:return {0,3,4,5,2,1};case 9:case 10:return {0,4,5,1};case 11:case 12:case 13:return {4,5,2,3,1};case 14:return {0,2,3,4,5,1};case 15:return {0,2,5,1};case 16:return {4,5,2,1};case 17:return {4,5,1};case 18:return {0,3};case 19:return {0,2,1};case 20:return {0,2,3,4,5,1};case 21:return {2,3,4,5,1};default:return {};}
}
void VirtualCortexEditor::selectSlot(int row,int col){if(!vc::SignalGrid::valid((size_t)row,(size_t)col))return;selectedRow=row;selectedColumn=col;rowBank=row/2;rowSelector.setSelectedId(row+1,juce::dontSendNotification);masterPage=false;const auto grid=processor.getEditableGrid();const int type=(int)grid.get((size_t)row,(size_t)col)->type;if(type)browserType=type;lastView=-1;refreshSlotControls();updateView();}
bool VirtualCortexEditor::insertBlock(int type){if(type<0||type>21)return false;bool ok=processor.setGridBlock((size_t)selectedRow,(size_t)selectedColumn,{(vc::BlockType)type,true});masterPage=false;if(type)browserType=type;lastView=-1;updateView();statusLabel.setText(gridMode()?"Block updated":"Block updated. Enable GRID ENGINE to hear the grid.",juce::dontSendNotification);return ok;}
bool VirtualCortexEditor::toggleSelectedSlot(){const auto grid=processor.getEditableGrid();const auto slot=*grid.get((size_t)selectedRow,(size_t)selectedColumn);if(slot.type==vc::BlockType::Empty)return false;bool ok=processor.setGridBlock((size_t)selectedRow,(size_t)selectedColumn,{slot.type,!slot.enabled});refreshGridControls();return ok;}
void VirtualCortexEditor::refreshGridControls(){const auto grid=processor.getEditableGrid();for(int r=0;r<4;++r){laneButtons[(size_t)r].setToggleState(processor.isGridLaneEnabled((size_t)r),juce::dontSendNotification);for(int c=0;c<8;++c){const auto slot=*grid.get((size_t)r,(size_t)c);auto& b=gridCells[(size_t)r][(size_t)c];b.type=(int)slot.type;b.blockEnabled=slot.enabled&&processor.isGridLaneEnabled((size_t)r);b.selected=r==selectedRow&&c==selectedColumn;b.caption=juce::String::charToString((juce::juce_wchar)('A'+c));b.repaint();}}
 for(int c=0;c<8;++c){const auto slot=*grid.get((size_t)selectedRow,(size_t)c);auto& b=stomps[(size_t)c];b.type=(int)slot.type;b.blockEnabled=slot.enabled;b.selected=c==selectedColumn;b.repaint();}
 for(int c=0;c<8;++c)quickSelectors[(size_t)c].setToggleState(c==selectedColumn,juce::dontSendNotification);
 slotEnabled.setToggleState(grid.get((size_t)selectedRow,(size_t)selectedColumn)->enabled,juce::dontSendNotification);
}
void VirtualCortexEditor::refreshSlotControls(){const auto p=processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn);repeatFxButton.setToggleState(p.repeatFx,juce::dontSendNotification);const float values[]={p.drive,p.mix,p.time,p.feedback,p.rate,p.depth};for(int i=0;i<6;++i)if(!slotSliders[(size_t)i].isMouseButtonDown())slotSliders[(size_t)i].setValue(values[i],juce::dontSendNotification);irName.setText(processor.getCabinetIRName(),juce::dontSendNotification);gridIRName.setText(processor.getGridCabinetIRName((size_t)selectedRow,(size_t)selectedColumn),juce::dontSendNotification);neuralName.setText(processor.getGridNeuralModelName((size_t)selectedRow,(size_t)selectedColumn),juce::dontSendNotification);
 for(auto& k:knobs)if(!k.value.isBeingEdited())k.value.setText(k.slider.getTextFromValue(k.slider.getValue()),juce::dontSendNotification);
 for(size_t i=0;i<6;++i)if(!slotValues[i].isBeingEdited())slotValues[i].setText(slotSliders[i].getTextFromValue(slotSliders[i].getValue()),juce::dontSendNotification);
}
void VirtualCortexEditor::commitSlotControls(){auto p=processor.getGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn);p.drive=(float)slotSliders[0].getValue();p.mix=(float)slotSliders[1].getValue();p.time=(float)slotSliders[2].getValue();p.feedback=(float)slotSliders[3].getValue();p.rate=(float)slotSliders[4].getValue();p.depth=(float)slotSliders[5].getValue();processor.setGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn,p);}
void VirtualCortexEditor::timerCallback(){updateView();refreshSlotControls();if(currentType()==27){const auto state=processor.loopStatus();loopButtons[0].setButtonText((state&256)?"END RECORD":"REC / DUB");statusLabel.setText(juce::String((state&256)?"Recording":(state&512)?"Playing":"Stopped")+" | Layers "+juce::String((int)(state&255))+" / 8 | 16-second maximum | Session-only audio",juce::dontSendNotification);}}
void VirtualCortexEditor::updateView(){refreshGridControls();const int type=currentType();const bool gm=gridMode(),sync=processor.parameters.getRawParameterValue("sdSync")->load()>.5f,ping=processor.parameters.getRawParameterValue("sdPingPong")->load()>.5f;if(type==lastView&&gm==lastGridMode&&sync==lastStereoSync&&ping==lastStereoPing)return;lastStereoSync=sync;lastStereoPing=ping;if(gm!=lastGridMode)statusLabel.setText({},juce::dontSendNotification);lastView=type;lastGridMode=gm;for(auto& k:knobs){k.slider.setVisible(false);k.value.setVisible(false);}for(auto& s:slotSliders)s.setVisible(false);for(auto& value:slotValues)value.setVisible(false);for(auto& s:switches)s.button.setVisible(false);
 switches[6].button.setVisible(true);switches[9].button.setVisible(true);
 slotLabels={"DRIVE","MIX","TIME","FEEDBACK","RATE","DEPTH"};
 if(type==8)slotLabels={"THRESHOLD","MIX","MAKEUP","RATIO","ATTACK","RELEASE"};
 if(type==9||type==10)slotLabels={"DRIVE","MIX","TIME","FEEDBACK","TONE","LEVEL"};
 if(type>=11&&type<=13)slotLabels={"DRIVE","MIX",type==13?"CENTER":"BASE DELAY","FEEDBACK","RATE","DEPTH"};
 if(type==14)slotLabels={"SENSITIVITY","MIX","POSITION","RESONANCE","ATTACK","RELEASE"};
 if(type==15)slotLabels={"RESOLUTION","MIX","HOLD","FEEDBACK","RATE","LEVEL"};
 if(type==16)slotLabels={"DRIVE","MIX","BASE DELAY","FEEDBACK","RATE","DEPTH"};
 if(type==17)slotLabels={"DRIVE","MIX","TIME","FEEDBACK","FREQUENCY","DEPTH"};
 if(type==18)slotLabels={"GAIN","MIX","TIME","POLARITY","RATE","DEPTH"};
 if(type==19)slotLabels={"HIGH PASS","MIX","LOW PASS","FEEDBACK","RATE","DEPTH"};
 if(type==20)slotLabels={"LOW GAIN","MIX","MID GAIN","HIGH GAIN","MID FREQ","MID Q"};
 if(type==21)slotLabels={"DRIVE","MIX","DECAY","DAMPING","PRE DELAY","DIFFUSION"};
 if(type==5&&gm)slotLabels={"LOOP HIGH PASS","MIX","TIME","FEEDBACK","LOOP FILTER MIX","LOOP LOW PASS"};
 repeatFxButton.setVisible(gm&&type==5);
 for(int i=0;i<3;++i){stereoPages[(std::size_t)i].setVisible(type==23);stereoPages[(std::size_t)i].setToggleState(i==stereoPage,juce::dontSendNotification);}
 for(int i=0;i<3;++i){reverbPages[(std::size_t)i].setVisible(type==24);reverbPages[(std::size_t)i].setToggleState(i==reverbPage,juce::dontSendNotification);}
 for(int i=0;i<4;++i){echoPages[i].setVisible(type==25);echoPages[i].setToggleState(i==echoPage,juce::dontSendNotification);}switches[16].button.setVisible(type==25);
 trackingPageButton.setVisible(type==28||type==29);trackingPageButton.setButtonText(trackingPage?"MAIN":type==28?"ENVELOPES":"STUTTER / STEPS");switches[20].button.setVisible(type==28);switches[21].button.setVisible(type==28);switches[22].button.setVisible(type==29);
 for(auto& b:loopButtons)b.setVisible(type==27);switches[17].button.setVisible(type==26);switches[18].button.setVisible(type==27);switches[19].button.setVisible(type==27);
 switches[14].button.setVisible(type==24);switches[15].button.setVisible(type==24);
 if(type==23)for(int i=10;i<14;++i)switches[(std::size_t)i].button.setVisible(true);
 const auto colour=CortexBlockButton::colourFor(type);
 if(!gm||masterPage){for(int i:visibleKnobs()){knobs[(size_t)i].slider.setVisible(true);knobs[(size_t)i].value.setVisible(true);knobs[(size_t)i].slider.setColour(juce::Slider::rotarySliderFillColourId,colour);knobs[(size_t)i].slider.setEnabled(true);knobs[(size_t)i].value.setEnabled(true);}int enable=-1;switch(type){case 1:enable=0;break;case 2:enable=1;break;case 3:enable=2;break;case 4:enable=7;break;case 5:enable=3;break;case 6:enable=4;break;case 7:enable=5;break;}if(enable>=0)switches[(size_t)enable].button.setVisible(true);switches[8].button.setVisible(type==3&&!gm);}
 else for(int i:visibleSlotControls()){slotSliders[(size_t)i].setVisible(true);slotValues[(size_t)i].setVisible(true);slotSliders[(size_t)i].setColour(juce::Slider::rotarySliderFillColourId,colour);}
 if(type==23){for(int i:{18,19}){knobs[(std::size_t)i].slider.setEnabled(!sync);knobs[(std::size_t)i].value.setEnabled(!sync);}for(int i:{33,34}){knobs[(std::size_t)i].slider.setEnabled(sync);knobs[(std::size_t)i].value.setEnabled(sync);}knobs[21].slider.setEnabled(!ping);knobs[21].value.setEnabled(!ping);}
 slotEnabled.setVisible(gm&&!masterPage&&type!=0);loadIRButton.setVisible(!gm&&type==3);irName.setVisible(!gm&&type==3);loadGridIRButton.setVisible(gm&&type==3);gridIRName.setVisible(gm&&type==3);loadNeuralButton.setVisible(gm&&type==2);neuralName.setVisible(gm&&type==2);addButton.setVisible(false);addButton.setEnabled(browserType<22);removeButton.setEnabled(!masterPage);
 for(int i=0;i<16;++i){browserButtons[(size_t)i].selected=browserButtons[(size_t)i].type==browserType;browserButtons[(size_t)i].repaint();}refreshDeviceList();
 refreshSlotControls();resized();repaint();
}
void VirtualCortexEditor::setEditorsVisible(bool visible){editorsVisible=visible;lastView=-1;updateView();}
void VirtualCortexEditor::resized(){
 designScale=juce::jmin(getWidth()/1184.f,getHeight()/864.f);designOffsetX=(getWidth()-1184.f*designScale)*.5f;designOffsetY=(getHeight()-864.f*designScale)*.5f;
 browserArea={53,78,240,752};gridArea={302,132,872,184};parameterArea={302,318,872,272};stompArea={474,598,700,232};
 previousPreset.setBounds(302,78,35,36);nextPreset.setBounds(339,78,35,36);presetBox.setBounds(376,78,340,36);savePresetButton.setBounds(718,78,35,36);loadPresetButton.setBounds(755,78,34,36);
 for(int i=0;i<8;++i)quickSelectors[(size_t)i].setBounds(798+i*38,78,35,36);headerStompButton.setBounds(1106,78,68,36);
 bankButtons[0].setBounds(224,37,25,27);bankButtons[1].setBounds(253,37,25,27);directoryButton.setBounds(638,30,142,44);ioButton.setBounds(1046,35,32,32);switches[6].button.setBounds(976,35,32,32);switches[9].button.setBounds(354,837,120,24);
 for(int i=0;i<16;++i)browserButtons[(size_t)i].setBounds(9,78+i*47,43,47);
 categoryBox.setBounds(62,84,224,29);
 for(int i=0;i<8;++i)deviceEntries[(size_t)i].setBounds(62,119+i*38,224,36);
 addButton.setBounds(62,789,224,31);
 bankButtons[0].setVisible(false);bankButtons[1].setVisible(false);
 const int rowStep=editorsVisible?46:160,rowTop=editorsVisible?132:153;
 for(int r=0;r<4;++r){laneButtons[(size_t)r].setVisible(true);laneButtons[(size_t)r].setBounds(302,rowTop+r*rowStep+5,49,editorsVisible?39:84);for(int c=0;c<8;++c){auto& cell=gridCells[(size_t)r][(size_t)c];cell.setVisible(true);cell.setBounds(363+c*93,rowTop+r*rowStep,94,editorsVisible?44:92);}}
 catalogButton.setBounds(511,35,115,32);catalogPanel.setBounds(53,78,1121,752);
 fullScreenButton.setBounds(805,35,162,32);
 editorsButton.setButtonText(editorsVisible?"HIDE EDITORS":"SHOW EDITORS");editorsButton.setBounds(205,837,140,24);
 const auto indices=gridMode()&&!masterPage?visibleSlotControls():visibleKnobs();
 for(int j=0;j<(int)indices.size();++j){const int i=indices[(size_t)j];const int x=302+(j%5)*174,y=354+(j/5)*118;
  const auto knob=juce::Rectangle<int>(x+92,y+43,76,76),value=juce::Rectangle<int>(x+6,y+83,92,28);
  if(gridMode()&&!masterPage){slotSliders[(size_t)i].setBounds(knob);slotValues[(size_t)i].setBounds(value);}else{knobs[(size_t)i].slider.setBounds(knob);knobs[(size_t)i].value.setBounds(value);}}
 for(auto& sw:switches)if(&sw!=&switches[6]&&&sw!=&switches[9])sw.button.setBounds(1137,318,36,35);slotEnabled.setBounds(1137,318,36,35);repeatFxButton.setBounds(838,492,245,32);
 switches[10].button.setBounds(1137,318,36,35);for(int i=11;i<14;++i)switches[(std::size_t)i].button.setBounds(650+(i-11)*170,489,165,34);
 for(int i=0;i<3;++i){stereoPages[(std::size_t)i].setBounds(650+i*170,537,165,32);reverbPages[(std::size_t)i].setBounds(650+i*170,537,165,32);}
 for(int i=0;i<4;++i)echoPages[i].setBounds(480+i*170,537,165,32);
 for(int i=0;i<6;++i)loopButtons[i].setBounds(310+i*142,537,136,32);switches[19].button.setBounds(1000,489,165,34);switches[17].button.setComponentID("power");switches[18].button.setComponentID("power");
 trackingPageButton.setBounds(1000,537,165,32);switches[21].button.setBounds(1000,489,165,34);switches[20].button.setComponentID("power");switches[22].button.setComponentID("power");
 switches[16].button.setComponentID("power");
 switches[14].button.setComponentID("power");switches[14].button.setBounds(1138,318,34,34);
 switches[15].button.setBounds(650,489,165,34);switches[15].button.setComponentID("reverb-pre-glide");switches[15].button.setTooltip("Off: clean 20 ms pre-delay crossfade. On: pitch-changing glide. Size and modulation retain their moving reads.");
 switches[8].button.setBounds(309,489,150,34);removeButton.setBounds(302,318,35,35);
 for(auto* b:{&loadIRButton,&loadGridIRButton,&loadNeuralButton})b->setBounds(838,492,245,32);
 for(auto* l:{&irName,&gridIRName,&neuralName})l->setBounds(838,530,245,25);
 presetMenuButton.setBounds(302,635,172,52);rowSelector.setBounds(302,688,172,52);stompModeButton.setBounds(302,741,172,52);
 for(int c=0;c<8;++c)stomps[(size_t)c].setBounds(474+(c%4)*175,598+(c/4)*116,175,116);
 statusLabel.setBounds(482,836,500,24);
 if(!editorsVisible){
  for(auto& k:knobs){k.slider.setVisible(false);k.value.setVisible(false);}for(auto& s:slotSliders)s.setVisible(false);for(auto& v:slotValues)v.setVisible(false);
  for(size_t i=0;i<switches.size();++i)if(i!=6&&i!=9)switches[i].button.setVisible(false);
  trackingPageButton.setVisible(false);
 for(auto& b:loopButtons)b.setVisible(false);
  for(auto& b:echoPages)b.setVisible(false);
  for(auto& button:stereoPages)button.setVisible(false);for(auto& button:reverbPages)button.setVisible(false);
  repeatFxButton.setVisible(false);slotEnabled.setVisible(false);removeButton.setVisible(false);loadIRButton.setVisible(false);loadGridIRButton.setVisible(false);loadNeuralButton.setVisible(false);irName.setVisible(false);gridIRName.setVisible(false);neuralName.setVisible(false);
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

 const int rowStep=editorsVisible?46:160,rowTop=editorsVisible?132:153;
 for(int row=0;row<4;++row){const int y=rowTop+row*rowStep;const bool active=processor.isGridLaneEnabled((size_t)row);g.setColour(juce::Colour(active?0xffa3a3a3:0xff383838));const float cy=(float)y+(editorsVisible?22.f:47.f);g.drawLine(352,cy,1124,cy,1.f);g.setColour(juce::Colour(0xff111212));g.fillRoundedRectangle(1124.f,(float)y+5,49.f,editorsVisible?39.f:84.f,16);drawCaption(g,"Mix",{1125,y+(editorsVisible?9:30),47,25},12.f,juce::Colours::white,juce::Justification::centred);}
 if(!editorsVisible&&(switches[10].button.getToggleState()||switches[14].button.getToggleState()))drawCaption(g,juce::String("POST CHAIN: ")+(switches[10].button.getToggleState()?"STEREO DELAY":"")+(switches[10].button.getToggleState()&&switches[14].button.getToggleState()?" > ":"")+(switches[14].button.getToggleState()?"STEREO REVERB":""),{363,801,760,25},13.f,mint);
 if(!editorsVisible)drawCaption(g,"PARALLEL ROWS  /  Same input to enabled rows / averaged main output",{363,768,760,35},14.f,muted);
 if(editorsVisible){
 g.setColour(juce::Colour(0xff111111));g.fillRoundedRectangle(parameterArea.toFloat(),5);g.setColour(panel);g.fillRect(302,354,872,236);
 for(int i=0;i<3;++i){g.setColour(muted);g.fillEllipse(318.f,325.f+i*7.f,2.f,2.f);g.fillEllipse(318.f,605.f+i*7.f,2.f,2.f);}
 drawCaption(g,CortexBlockButton::nameFor(currentType()),{347,319,135,33},18.f,muted);drawCaption(g,currentType()==2?"Original amplifier":currentType()==3?"Cabinet IR":currentType()>=23?"POST CHAIN":currentType()==8?"Original peak compressor":currentType()==9?"Original soft clipping":currentType()==10?"Original hard clipping":"Original",{485,319,600,33},18.f,CortexBlockButton::colourFor(currentType()));
 const auto indices=gridMode()&&!masterPage?visibleSlotControls():visibleKnobs();for(int j=0;j<10;++j){int x=302+(j%5)*174,y=354+(j/5)*118;g.setColour(juce::Colour(0xff070707));g.drawLine((float)x+174,(float)y+8,(float)x+174,(float)y+118,1.f);g.drawLine((float)x,(float)y+118,(float)x+174,(float)y+118,1.f);if(j<(int)indices.size()){int i=indices[(size_t)j];drawCaption(g,gridMode()&&!masterPage?slotLabels[(size_t)i]:knobs[(size_t)i].label,{x+9,y+6,156,23},15.f);}}
 if(indices.empty())drawCaption(g,currentType()==0?"Select a slot, then choose a device from the directory.":currentType()==3?"Load a cabinet impulse response.":"Fixed settings in the current grid engine.",{320,381,730,44},14.f,muted);

 g.setColour(panel);g.fillRect(302,598,172,232);g.setColour(juce::Colour(0xff070707));g.drawRect(302,598,172,36,1);drawCaption(g,"Gig view",{343,599,125,33},20.f,juce::Colours::white,juce::Justification::centredLeft,true);drawCaption(g,"Parameter Editor",{310,796,156,32},14.f,muted);
 }
 drawCaption(g,gridMode()?"GRID":"SERIAL",{20,836,65,24},11.f,muted);drawCaption(g,processor.getSampleRate()>0?juce::String(processor.getSampleRate()/1000.,1)+" kHz":"AUDIO",{103,836,100,24},11.f,muted);drawCaption(g,"Virtual Cortex v6.17.0",{1006,837,170,22},11.f,juce::Colour(0xff676767));
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
 case 11:return {"Original chorus","Gentle chorus","Wide chorus","Fast chorus"};
 case 12:return {"Original flanger","Slow flange","Deep flange","Fast flange"};
 case 13:return {"Original six-stage phaser","Slow phase","Deep phase","Fast phase"};
 case 14:return {"Original envelope wah","Gentle envelope","Sharp envelope","Manual sweep"};
 case 15:return {"Original bit-crusher","Low resolution","Sample hold","Subtle crunch"};
 case 16:return {"Original vibrato","Gentle vibrato","Deep vibrato","Fast vibrato"};
 case 17:return {"Original ring modulation","Slow carrier","Metallic carrier","Blended modulation"};
 case 18:return {"Unity","-6 dB","-12 dB","Invert polarity"};
 case 19:return {"Full range","Guitar cuts","Dark cuts","Band limited"};
 case 20:return {"Original flat tone EQ","Warm tone","Mid presence","Mid scoop"};
 case 21:return {"Original room reverb","Small room","Long room","Dark room"};
 case 28:return {"Prism lead","Sub bass"};
 case 29:return {"Digital staircase","Filter stutter"};
 case 26:return {"Interval staircase","Octave wander"};
 case 27:return {"Session looper"};
 case 25:return {"Four-head spread","Rhythmic cascade"};
 case 24:return {"Original stereo FDN","Short space","Long space","Dark space","Ducked space"};
 case 23:return {"Original stereo delay","Ping-pong echoes","Stereo slapback","Wide echoes","Tempo dotted","Diffuse cloud","Slow bloom"};
 case 22:return {"Unity","-6 dB","-12 dB","Mute"};
 default:return {};
 }
}
void VirtualCortexEditor::refreshDeviceList(){categoryBox.setSelectedId(browserType,juce::dontSendNotification);const auto names=deviceNames();for(int i=0;i<8;++i){auto& b=deviceEntries[(size_t)i];b.setVisible(i<names.size());if(i<names.size()){b.setButtonText(names[i]);b.setToggleState(i==variant,juce::dontSendNotification);}}}
void VirtualCortexEditor::applyDeviceVariant(int index){
 const auto names=deviceNames();if(index<0||index>=names.size())return;variant=index;
 auto set=[this](const char* id,float v){if(auto* p=processor.parameters.getParameter(id))p->setValueNotifyingHost(p->convertTo0to1(v));};
 if(browserType==28){set("syOn",1);set("syTranspose",index?-12:0);set("syCutoff",index?600:1800);set("syMix",1);set("syWave",1);masterPage=true;lastView=-1;updateView();statusLabel.setText("Mono tracking | Left input | MIDI channel 1 (host routing)",juce::dontSendNotification);return;}
 if(browserType==29){set("scOn",1);set("scMode",index?1:0);set("scStutter",index?.7f:0);set("scMix",.7f);masterPage=true;lastView=-1;updateView();statusLabel.setText("Six free-running steps | Rate / Filter / Bits",juce::dontSendNotification);return;}
 if(browserType==26){set("ieOn",1);set("ieA",index?12:7);set("ieB",index?-12:-5);set("ieTime",350);set("ieFeedback",.4f);set("ieMix",.3f);set("ieStep",500);set("ieCutoff",5000);masterPage=true;lastView=-1;updateView();statusLabel.setText("Interval Echo: original sequenced digital pitch delay",juce::dontSendNotification);return;}
 if(browserType==27){set("llOn",1);masterPage=true;lastView=-1;updateView();statusLabel.setText("Layer Loom: 16 seconds, 8 layers. Session audio is not saved in presets.",juce::dontSendNotification);return;}
 if(browserType==25){set("mhOn",1);set("mhMix",.3f);for(int i=1;i<=4;++i){const auto prefix="mh"+juce::String(i);set((prefix+"Time").toRawUTF8(),(index?125.f:100.f)*i);set((prefix+"Level").toRawUTF8(),.5f);set((prefix+"Pan").toRawUTF8(),i%2?-.7f:.7f);set((prefix+"Feedback").toRawUTF8(),i==4?.55f:0.f);}masterPage=true;lastView=-1;updateView();return;}
 if(browserType==24){
  const float decays[]={2.5f,.7f,6.f,3.5f,3.f};set("stereoReverbOn",1);set("srPreGlide",0);set("srDecay",decays[index]);set("srSize",index==1?.65f:(index==2?1.5f:1.f));set("srHighDecay",index==3?.25f:.5f);set("srLowDecay",1);set("srPreDelay",index==1?5.f:20.f);set("srDucking",index==4?.75f:0.f);set("srMix",.2f);set("srDiffusion",.7f);set("srEarly",.2f);set("srWidth",1);set("srModDepth",.3f);set("srModRate",.35f);reverbPage=0;masterPage=true;lastView=-1;updateView();statusLabel.setText("Stereo Reverb enabled after Stereo Delay and the main chain",juce::dontSendNotification);return;
 }
 if(browserType==23){
  set("stereoDelayOn",1);set("sdDiffusion",index>=5?1.f:0.f);set("sdPingPong",index==1?1.f:0.f);set("sdTape",0);set("sdSync",index==4?1.f:0.f);set("sdTimeL",index==2?90.f:320.f);set("sdTimeR",index==2?120.f:480.f);set("sdFeedback",index>=5?.75f:(index==2?.15f:.35f));set("sdCross",index==3?.4f:0.f);set("sdMix",.2f);set("sdWidth",index==3?1.4f:1.f);set("sdModDepth",index>=5?.6f:(index==3?1.f:0.f));set("sdModRate",.5f);set("sdBpm",120);set("sdBeatsL",1);set("sdBeatsR",1.5f);set("sdFilterMix",1);set("sdLowCut",80);set("sdHighCut",index>=5?6000.f:16000.f);if(index==6){set("sdTimeL",700);set("sdTimeR",930);set("sdDucking",.65f);}else set("sdDucking",0);stereoPage=index>=5?1:0;masterPage=true;lastView=-1;updateView();statusLabel.setText("Stereo Delay enabled after the main chain",juce::dontSendNotification);return;
 }
 if(browserType==22){const float levels[]={1.f,.501187f,.251189f,0.f};set("outputTrim",levels[index]);statusLabel.setText("Output level updated",juce::dontSendNotification);refreshDeviceList();return;}
 if(browserType>=8&&browserType<=21&&!gridMode()){set("gridMode",1.f);lastView=-1;updateView();}
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
  if(browserType>=11&&browserType<=13){p.rate=index==3?.6f:(index==1?.04f:.08f);p.depth=index==2?.9f:(index==1?.2f:.4f);p.time=.4f;p.feedback=index==2?.7f:.2f;p.mix=index==1?.35f:.5f;}
  if(browserType==14){p.drive=index==3?0.f:(index==2?.85f:.4f);p.time=index==3?.5f:.05f;p.feedback=index==2?.75f:.35f;p.rate=.05f;p.depth=.2f;p.mix=1.f;}
  if(browserType==15){p.drive=index==1?.16f:.66f;p.time=index==2?.24f:0.f;p.depth=.5f;p.mix=index==3?.25f:1.f;}
  if(browserType==16){p.rate=index==3?.6f:.18f;p.depth=index==2?.9f:(index==1?.15f:.4f);p.time=.25f;p.feedback=0;p.mix=1;}
  if(browserType==17){p.rate=index==1?.13082f:(index==2?.69624f:.43459f);p.depth=index==3?.5f:1.f;p.mix=index==3?.5f:1.f;}
  if(browserType==18){const float levels[]={0,-6,-12,0};p.drive=(60.f+levels[index])/72.f;p.feedback=index==3?1.f:0.f;}
  if(browserType==19){const float hp[]={20,80,100,300},lp[]={20000,8000,4000,3000};p.drive=(hp[index]-20.f)/980.f;p.time=(lp[index]-1000.f)/19000.f;p.mix=1;}
  if(browserType==20){p.drive=index==1?.625f:.5f;p.time=index==2?.666667f:(index==3?.333333f:.5f);p.feedback=index==1?.416667f:.5f;p.rate=.230769f;p.depth=.189189f;p.mix=1;}
  if(browserType==21){const float seconds[]={1.8f,.5f,4.f,3.f};p.time=(seconds[index]-.2f)/7.8f;p.feedback=index==3?.8f:.45f;p.rate=index==1?0.f:.2f;p.depth=.6f;p.mix=index==2?.4f:.3f;}
  processor.setGridSlotParameters((size_t)selectedRow,(size_t)selectedColumn,p);
 }else{
  if(browserType==1){const float values[]={-74,-62,-50,-36};set("gate",values[index]);}
  if(browserType==2){const float drives[]={.35f,.08f,.2f,.45f,.8f};set("drive",drives[index]);set("level",index>=3?.45f:.65f);}
  if(browserType==3){set("cabOn",1);set("irOn",0);set("cab",1);}
  if(browserType==4){const float values[5][3]={{0,0,0},{3,0,-2},{-1,0,4},{0,4,0},{0,-4,0}};set("eqLow",values[index][0]);set("eqMid",values[index][1]);set("eqHigh",values[index][2]);}
  if(browserType==5){const float ms[]={90,180,320,480,650};set("delayTime",ms[index]);set("delayFeedback",.15f+.12f*index);set("delayMix",.15f+.08f*index);set("delayOn",1);}
  if(browserType==6){const float values[]={0,.1f,.2f,.35f,.55f};set("reverbMix",values[index]);set("reverbOn",1);}
  if(browserType==7){const float rates[]={.5f,2,4,8,12};set("tremRate",rates[index]);set("tremDepth",.5f);set("tremOn",1);}
 }
 lastView=-1;refreshSlotControls();updateView();statusLabel.setText(names[index]+" selected",juce::dontSendNotification);
}
