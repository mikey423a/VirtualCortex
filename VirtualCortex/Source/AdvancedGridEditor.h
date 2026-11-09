#pragma once
#include "PluginProcessor.h"
// Uses the same rotary look-and-feel as the main editor, with eight controls per
// page so every parameter remains readable at the original design resolution.
class AdvancedGridEditor final:public juce::Component {
public:
 explicit AdvancedGridEditor(VirtualCortexProcessor& p):processor(p){
  setComponentID("advanced-grid-editor");setOpaque(true);
  for(int i=0;i<8;++i){auto& slider=sliders[i];slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);slider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,180,22);slider.setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(0xff57f5d5));
   slider.onValueChange=[this,i]{if(!refreshing)processor.setAdvancedValue(row,col,page*8+i,(float)sliders[i].getValue());};addChildComponent(slider);
   toggles[i].onClick=[this,i]{processor.setAdvancedValue(row,col,page*8+i,toggles[i].getToggleState()?1.f:0.f);};addChildComponent(toggles[i]);
  }
  previous.setComponentID("advanced-previous");next.setComponentID("advanced-next");
  previous.onClick=[this]{if(page>0){--page;layoutPage();}};next.onClick=[this]{if(page+1<pageCount()){++page;layoutPage();}};
  addAndMakeVisible(previous);addAndMakeVisible(next);
  const char* commands[]={"REC / OVERDUB","STOP","PLAY","UNDO","REDO","CLEAR"};
  for(int i=0;i<6;++i){loopButtons[i].setButtonText(commands[i]);loopButtons[i].setComponentID("advanced-loop-"+juce::String(i));loopButtons[i].onClick=[this,i]{processor.gridLoopCommand(row,col,1u<<i);};addChildComponent(loopButtons[i]);}
 }
 void select(int r,int c,int t){if(row==r&&col==c&&type==t){refresh();return;}row=r;col=c;type=t;page=0;layoutPage();}
 void refresh(){if(!vc::isAdvanced(type))return;refreshing=true;const auto values=processor.getAdvancedValues(row,col);const auto& def=vc::advancedDefinitions[type-30];
  for(int i=0;i<8;++i){int index=page*8+i;if(index>=def.count)continue;if(!sliders[i].isMouseButtonDown()&&!sliders[i].hasKeyboardFocus(true))sliders[i].setValue(values[index],juce::dontSendNotification);toggles[i].setToggleState(values[index]>.5f,juce::dontSendNotification);}
  refreshing=false;repaint();
 }
 void resized()override {const int w=getWidth()/4;for(int i=0;i<8;++i){int x=(i%4)*w,y=(i/4)*98;sliders[i].setBounds(x+12,y+24,w-24,72);toggles[i].setBounds(x+20,y+38,w-40,40);}previous.setBounds(getWidth()-280,201,120,28);next.setBounds(getWidth()-150,201,140,28);for(int i=0;i<6;++i)loopButtons[i].setBounds(8+i*(getWidth()-16)/6,128,(getWidth()-16)/6-5,34);}
 void paint(juce::Graphics& g)override {
  g.fillAll(juce::Colour(0xff1e1e1e));if(!vc::isAdvanced(type))return;const auto& d=vc::advancedDefinitions[type-30];const int w=getWidth()/4;
  g.setFont(13.f);for(int i=0;i<8;++i){const int index=page*8+i;if(index>=d.count)break;int x=i%4*w,y=i/4*98;g.setColour(juce::Colour(0xff080808));g.drawRect(x,y,w,98);g.setColour(juce::Colours::white);g.drawFittedText(d.controls[index].label,x+10,y+4,w-20,22,juce::Justification::centredLeft,1);}
  g.setColour(juce::Colour(0xffaaaaaa));g.setFont(13.f);
  juce::String status="PAGE "+juce::String(page+1)+" / "+juce::String(pageCount())+"   |   ROW "+juce::String(row+1)+" / SLOT "+juce::String(col+1);
  if(type==34){const unsigned s=processor.gridLoopStatus(row,col);status=juce::String(s&255u)+" layers  |  "+((s&256)?"RECORDING":(s&512)?"PLAYING":"STOPPED")+"  |  Loop audio is session-only";}
  if(type==35)status+="   |   MIDI CH "+juce::String((row*8+col)%15+2);
  g.drawFittedText(status,12,201,getWidth()-305,28,juce::Justification::centredLeft,1);
 }
private:
 VirtualCortexProcessor& processor;int row=-1,col=-1,type=0,page=0;bool refreshing=false;
 std::array<juce::Slider,8> sliders;std::array<juce::ToggleButton,8>toggles;std::array<juce::TextButton,6>loopButtons;
 juce::TextButton previous{"PREVIOUS"},next{"NEXT"};
 int pageCount()const{return vc::isAdvanced(type)?(vc::advancedDefinitions[type-30].count+7)/8:1;}
 void layoutPage(){if(!vc::isAdvanced(type))return;refreshing=true;const auto& d=vc::advancedDefinitions[type-30];
  for(int i=0;i<8;++i){const int index=page*8+i;const bool exists=index<d.count;sliders[i].setVisible(exists&&!d.controls[index].toggle);toggles[i].setVisible(exists&&d.controls[index].toggle);if(!exists)continue;
   const auto& c=d.controls[index];sliders[i].setComponentID("advanced-"+juce::String(c.id));toggles[i].setComponentID("advanced-"+juce::String(c.id));sliders[i].setRange(c.low,c.high,0);sliders[i].setNumDecimalPlacesToDisplay((c.high-c.low)>100?1:3);sliders[i].setDoubleClickReturnValue(true,c.initial);toggles[i].setButtonText("ON");
  }
  for(auto& b:loopButtons)b.setVisible(type==34);previous.setEnabled(page>0);next.setEnabled(page+1<pageCount());previous.setVisible(type!=34);next.setVisible(type!=34);refreshing=false;refresh();resized();
 }
};
