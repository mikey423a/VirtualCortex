#pragma once
#include "PluginProcessor.h"
class RoutingEditor final:public juce::Component {
public:
 std::function<void()> onChange;
 explicit RoutingEditor(VirtualCortexProcessor& p):processor(p){
  setComponentID("routing-editor");setOpaque(true);
  for(int r=0;r<4;++r){lanes[r].setButtonText("ROW "+juce::String(r+1));lanes[r].setComponentID("route-lane-"+juce::String(r));lanes[r].onClick=[this,r]{processor.setGridLaneEnabled(r,lanes[r].getToggleState());if(onChange)onChange();};addAndMakeVisible(lanes[r]);for(int s=0;s<5;++s){auto& b=sources[r][s];b.setButtonText(s?juce::String(s):"IN");b.setComponentID("route-source-"+juce::String(r)+"-"+juce::String(s));b.setEnabled(s<=r);b.onClick=[this]{commit();};addAndMakeVisible(b);}
   auto& entry=entries[r];for(int column=0;column<8;++column)entry.addItem("Start "+juce::String::charToString('A'+column),column+1);entry.setComponentID("route-entry-"+juce::String(r));entry.setTooltip("First slot processed in this row; earlier slots are skipped in Custom mode.");entry.onChange=[this]{commit();};addAndMakeVisible(entry);
   for(int from=0;from<4;++from){auto& tap=taps[r][from];tap.addItem("<A",1);for(int column=0;column<8;++column)tap.addItem(juce::String::charToString('A'+column)+">",column+2);tap.setComponentID("route-tap-"+juce::String(r)+"-"+juce::String(from));tap.setTooltip("Choose where to take this source row's signal, including its level, balance and polarity.");tap.onChange=[this]{commit();};addAndMakeVisible(tap);}
   auto& c=channels[r];c.addItemList({"Stereo","Left only","Right only","Mono sum"},1);c.setComponentID("route-channel-"+juce::String(r));c.onChange=[this]{commit();};addAndMakeVisible(c);
   for(auto* slider:{&gain[r],&balance[r]}){slider->setSliderStyle(juce::Slider::LinearHorizontal);slider->setTextBoxStyle(juce::Slider::TextBoxBelow,false,100,24);slider->onValueChange=[this]{commit();};addAndMakeVisible(slider);}
   gain[r].setRange(-60,12,.1);gain[r].setTextValueSuffix(" dB");gain[r].setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);balance[r].setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);gain[r].setDoubleClickReturnValue(true,0);gain[r].setComponentID("route-gain-"+juce::String(r));
   balance[r].setRange(-1,1,.01);balance[r].textFromValueFunction=[](double v){return std::abs(v)<.005?juce::String("CENTER"):juce::String(v<0?"L ":"R ")+juce::String(std::abs(v)*100,0)+"%";};balance[r].valueFromTextFunction=[](const juce::String& text){if(text.containsIgnoreCase("CENTER"))return 0.;const auto v=text.retainCharacters("0123456789.-").getDoubleValue();return text.containsIgnoreCase("L")?-std::abs(v)/100.:text.containsIgnoreCase("R")?std::abs(v)/100.:text.containsChar('%')?v/100.:v;};balance[r].updateText();balance[r].setDoubleClickReturnValue(true,0);balance[r].setComponentID("route-balance-"+juce::String(r));
   invert[r].setButtonText("INV");output[r].setButtonText("OUT");invert[r].onClick=output[r].onClick=[this]{commit();};invert[r].setComponentID("route-invert-"+juce::String(r));output[r].setComponentID("route-output-"+juce::String(r));addAndMakeVisible(invert[r]);addAndMakeVisible(output[r]);
  }
  average.setButtonText("Average selected sources / outputs");average.setComponentID("route-average");average.onClick=[this]{commit();};addAndMakeVisible(average);
  smoothTaps.setButtonText("Smooth tap changes (5 ms)");
  smoothTaps.setComponentID("route-smooth-taps");
  smoothTaps.onClick=[this]{commit();};
  addAndMakeVisible(smoothTaps);
  smoothEntries.setButtonText("Smooth row entry (5 ms)");
  smoothEntries.setComponentID("route-smooth-entries");
  smoothEntries.setTooltip("Fade affected effects when changing the first slot in a row.");
  smoothEntries.onClick=[this]{commit();};
  addAndMakeVisible(smoothEntries);
  smoothModes.setButtonText("Smooth routing modes (5 ms)");
  smoothModes.setComponentID("route-smooth-modes");
  smoothModes.setTooltip("Soften switches between Parallel, Series and Custom. Dual Mono switches immediately.");
  smoothModes.onClick=[this]{commit();};
  addAndMakeVisible(smoothModes);
  close.onClick=[this]{setVisible(false);};close.setComponentID("routing-close");addAndMakeVisible(close);
  use.onClick=[this]{activate();};use.setComponentID("routing-use");addAndMakeVisible(use);
  const char* names[]={"PARALLEL","SPLIT / REJOIN","TWO CHAINS"};
  for(int i=0;i<3;++i){presets[i].setButtonText(names[i]);presets[i].setComponentID("route-preset-"+juce::String(i));presets[i].onClick=[this,i]{vc::RowRouting c;
    c.smoothSourceTaps=smoothTaps.getToggleState();
    c.smoothRowEntries=smoothEntries.getToggleState();
    c.smoothRoutingModes=smoothModes.getToggleState();
    if(i==1){c.rows[1].sources=2;c.rows[2].sources=2;c.rows[3].sources=12;for(int r=0;r<3;++r)c.rows[r].output=false;}
    if(i==2){c.average=false;c.rows[0].channel=1;c.rows[0].output=false;c.rows[1].sources=2;c.rows[1].balance=-1;c.rows[2].channel=2;c.rows[2].output=false;c.rows[3].sources=8;c.rows[3].balance=1;}
    processor.setRowRouting(c);for(int r=0;r<4;++r)processor.setGridLaneEnabled(r,true);refresh();activate();};addAndMakeVisible(presets[i]);}
  refresh();
 }
 void refresh(){if(refreshing)return;refreshing=true;const auto c=processor.getRowRouting();
  for(int r=0;r<4;++r){entries[r].setSelectedId(c.rows[r].entryColumn+1,juce::dontSendNotification);for(int from=0;from<4;++from){taps[r][from].setSelectedId(c.rows[r].sourceTaps[from]+1,juce::dontSendNotification);const int boundary=c.rows[r].sourceTaps[from];taps[r][from].setTooltip("Row "+juce::String(from+1)+(boundary==0?juce::String(" before A"):juce::String(" after ")+juce::String::charToString('A'+boundary-1))+"; includes source row level, balance and polarity.");taps[r][from].setEnabled(from<r&&(c.rows[r].sources&(2u<<from)));}lanes[r].setToggleState(processor.isGridLaneEnabled(r),juce::dontSendNotification);for(int s=0;s<5;++s)sources[r][s].setToggleState((c.rows[r].sources&(1u<<s))!=0,juce::dontSendNotification);channels[r].setSelectedId(c.rows[r].channel+1,juce::dontSendNotification);
   if(!gain[r].isMouseButtonDown()&&!gain[r].hasKeyboardFocus(true))gain[r].setValue(c.rows[r].gainDb,juce::dontSendNotification);
   if(!balance[r].isMouseButtonDown()&&!balance[r].hasKeyboardFocus(true))balance[r].setValue(c.rows[r].balance,juce::dontSendNotification);
   invert[r].setToggleState(c.rows[r].invert,juce::dontSendNotification);output[r].setToggleState(c.rows[r].output,juce::dontSendNotification);
  }smoothModes.setToggleState(c.smoothRoutingModes,juce::dontSendNotification);smoothEntries.setToggleState(c.smoothRowEntries,juce::dontSendNotification);smoothTaps.setToggleState(c.smoothSourceTaps,juce::dontSendNotification);average.setToggleState(c.average,juce::dontSendNotification);refreshing=false;repaint();
 }
 void resized()override {
  close.setBounds(getWidth()-110,14,96,32);use.setBounds(getWidth()-300,14,180,32);
  for(int r=0;r<4;++r){int y=128+r*108;entries[r].setBounds(12,y+49,90,28);for(int from=0;from<4;++from)taps[r][from].setBounds(169+from*64,y+49,62,28);lanes[r].setBounds(12,y+8,90,36);for(int s=0;s<5;++s)sources[r][s].setBounds(105+s*64,y+8,62,36);channels[r].setBounds(440,y+10,135,32);gain[r].setBounds(590,y,150,65);balance[r].setBounds(760,y,150,65);invert[r].setBounds(930,y+10,70,32);output[r].setBounds(1010,y+10,90,32);}
  smoothTaps.setBounds(24,544,360,32);
  smoothEntries.setBounds(404,544,320,32);
  smoothModes.setBounds(744,544,353,32);
  average.setBounds(24,580,400,36);for(int i=0;i<3;++i)presets[i].setBounds(445+i*215,580,205,36);
  for(int i=0;i<getNumChildComponents();++i){auto* child=getChildComponent(i);if(child==&close||child==&use)continue;const auto r=child->getBounds();child->setBounds(px(r.getX()),r.getY(),px(r.getRight())-px(r.getX()),r.getHeight());}
 }
 void paint(juce::Graphics& g)override {
  g.fillAll(juce::Colour(0xff171919));g.setColour(juce::Colour(0xff57f5d5));g.setFont(22);g.drawText("CUSTOM ROUTING",24,14,500,32,juce::Justification::centredLeft);
  g.setColour(juce::Colour(0xffaebbb5));g.setFont(12);g.drawText("POLARITY",px(930),84,px(1005)-px(930),28,juce::Justification::centred);g.drawText("OUTPUT",px(1010),84,px(1100)-px(1010),28,juce::Justification::centred);g.drawText("SOURCES",px(105),84,px(425)-px(105),28,juce::Justification::centredLeft);g.drawText("CHANNEL",px(440),84,px(575)-px(440),28,juce::Justification::centredLeft);g.drawText("ROW LEVEL",px(590),84,px(740)-px(590),28,juce::Justification::centredLeft);g.drawText("BALANCE L / R",px(760),84,px(920)-px(760),28,juce::Justification::centredLeft);
  for(int r=0;r<4;++r){const int y=128+r*108;g.setColour(juce::Colour(0xff303535));g.drawHorizontalLine(y+85,24.f,(float)getWidth()-24);g.setColour(juce::Colours::white);}
  g.setColour(juce::Colour(0xffaaaaaa));g.setFont(15);
  g.drawFittedText("IN = app input. Numbered sources = earlier rows; select a tap below each source. Select several to merge; reuse a source to split.\nA disabled row is silent. OUT includes a row in the final mix. No selected OUT rows means silence.\nAverage prevents identical parallel signals from adding level. Off sums at unity; leave headroom.\nRouting belongs to the preset and stays attached to rows when blocks move. Start A-H chooses the first processed slot. Tap <A = before A; A> through H> = after that slot. Row level applies to all taps. Global post effects follow the mix.",24,634,getWidth()-48,100,juce::Justification::topLeft,5);
  g.setColour(processor.getRoutingMode()==3?juce::Colour(0xff57f5d5):juce::Colours::orange);g.drawText(processor.getRoutingMode()==3?"CUSTOM ROUTING SELECTED":"Preview settings — press USE CUSTOM to hear them",24,52,900,24,juce::Justification::centredLeft);
 }
private:
 int px(int x)const{return juce::roundToInt(x*getWidth()/1121.f);}
 VirtualCortexProcessor& processor;bool refreshing=false;
 std::array<std::array<juce::ToggleButton,5>,4> sources;std::array<juce::ComboBox,4>channels,entries;std::array<std::array<juce::ComboBox,4>,4>taps;
 std::array<juce::Slider,4>gain,balance;std::array<juce::ToggleButton,4>invert,output,lanes;juce::ToggleButton average,smoothTaps,smoothEntries,smoothModes;
 juce::TextButton close{"CLOSE"},use{"USE CUSTOM"};std::array<juce::TextButton,3>presets;
 void activate(){processor.setRoutingMode(3);if(auto* p=processor.parameters.getParameter("gridMode"))p->setValueNotifyingHost(1.f);if(onChange)onChange();repaint();}
 void commit(){if(refreshing)return;vc::RowRouting c;c.smoothRoutingModes=smoothModes.getToggleState();c.smoothRowEntries=smoothEntries.getToggleState();c.smoothSourceTaps=smoothTaps.getToggleState();c.average=average.getToggleState();for(int r=0;r<4;++r){auto& p=c.rows[r];p.entryColumn=entries[r].getSelectedId()-1;for(int from=0;from<4;++from)p.sourceTaps[from]=taps[r][from].getSelectedId()-1;p.sources=0;for(int s=0;s<=r;++s)if(sources[r][s].getToggleState())p.sources|=1u<<s;p.channel=channels[r].getSelectedId()-1;p.gainDb=(float)gain[r].getValue();p.balance=(float)balance[r].getValue();p.invert=invert[r].getToggleState();p.output=output[r].getToggleState();}processor.setRowRouting(c);refresh();if(onChange)onChange();}
};
