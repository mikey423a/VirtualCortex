#include "PluginEditor.h"
#include <iostream>
#include <stdexcept>

static void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int main(int argc,char** argv){
 try {
  juce::ScopedJuceInitialiser_GUI gui;
  auto processor=std::make_unique<VirtualCortexProcessor>();
  processor->prepareToPlay(48000.,128);
  auto editor=std::make_unique<VirtualCortexEditor>(*processor);
  juce::File dir(argc>1?juce::String(argv[1]):juce::File::getCurrentWorkingDirectory().getFullPathName());
  require(dir.createDirectory().wasOk(),"Cannot create screenshot directory");
  auto capture=[&](const char* name,int w,int h){
   editor->setSize(w,h);
   for(int i=0;i<editor->getNumChildComponents();++i){auto* child=editor->getChildComponent(i);if(child->isVisible())require(editor->getLocalBounds().contains(child->getBounds()),"A visible control is clipped by the window");}
   const auto image=editor->createComponentSnapshot(editor->getLocalBounds());
   require(image.isValid(),"Editor snapshot failed");
   auto out=dir.getChildFile(name).createOutputStream();require(out!=nullptr,"PNG output failed");
   juce::PNGImageFormat png;require(png.writeImageToStream(image,*out),"PNG encoder failed");
  };
  auto* tone=dynamic_cast<juce::Slider*>(editor->findChildWithID("tone"));
  require(tone!=nullptr,"Tone control missing");
  bool formatted=false;
  for(int i=0;i<tone->getNumChildComponents();++i)if(auto* label=dynamic_cast<juce::Label*>(tone->getChildComponent(i)))formatted|=label->getText()=="60.0 %";
  require(formatted,"Initial tone value was not formatted before interaction");
  auto* drive=dynamic_cast<juce::Slider*>(editor->findChildWithID("drive"));require(drive!=nullptr,"Drive control missing");
  drive->setValue(.73,juce::sendNotificationSync);
  require(std::abs(processor->parameters.getRawParameterValue("drive")->load()-.73f)<.001f,"Drive control did not update processor");
  drive->setValue(.35,juce::sendNotificationSync);
  capture("serial-1120x660.png",1120,660);
  capture("compact-900x560.png",900,560);
  capture("large-1600x900.png",1600,900);
  editor->selectSlot(0,4);require(editor->insertBlock(6),"Block insertion failed");
  {const auto grid=processor->getEditableGrid();require(grid.get(0,4)->type==vc::BlockType::Reverb,"Inserted block has wrong type");}
  require(editor->toggleSelectedSlot(),"Slot bypass failed");
  {const auto grid=processor->getEditableGrid();require(!grid.get(0,4)->enabled,"Slot bypass was not applied");}
  require(editor->toggleSelectedSlot(),"Slot unbypass failed");
  processor->parameters.getParameter("gridMode")->setValueNotifyingHost(1.f);
  editor->selectSlot(0,4);
  capture("grid-reverb-1120x660.png",1120,660);
  editor->selectSlot(0,1);
  capture("grid-amp-1120x660.png",1120,660);
  editor.reset();processor->releaseResources();
  std::cout<<"PASS: native editor snapshots at three sizes, control binding, insertion and bypass\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
