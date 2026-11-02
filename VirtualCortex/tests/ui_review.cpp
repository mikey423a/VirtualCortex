#include "PluginEditor.h"
#include <iostream>
#include <stdexcept>

int runProcessorAudioTests();
static void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int main(int argc,char** argv){
 try {
  juce::ScopedJuceInitialiser_GUI gui;
  if(argc>1&&juce::String(argv[1])=="--audio-tests")return runProcessorAudioTests();
  auto processor=std::make_unique<VirtualCortexProcessor>();
  processor->prepareToPlay(48000.,128);
  auto editor=std::make_unique<VirtualCortexEditor>(*processor);
  juce::File dir(argc>1?juce::String(argv[1]):juce::File::getCurrentWorkingDirectory().getFullPathName());
  require(dir.createDirectory().wasOk(),"Cannot create screenshot directory");
  auto capture=[&](const char* name,int w,int h){
   editor->setSize(w,h);
   for(int i=0;i<editor->getNumChildComponents();++i){auto* child=editor->getChildComponent(i);if(child->isVisible())require(editor->getLocalBounds().contains(child->getBoundsInParent()),"A visible control is clipped by the window");}
   const auto image=editor->createComponentSnapshot(editor->getLocalBounds());
   require(image.isValid(),"Editor snapshot failed");
   auto out=dir.getChildFile(name).createOutputStream();require(out!=nullptr,"PNG output failed");
   juce::PNGImageFormat png;require(png.writeImageToStream(image,*out),"PNG encoder failed");
  };
  for(int r=0;r<4;++r){auto* lane=editor->findChildWithID("lane-"+juce::String(r));require(lane&&lane->isVisible(),"A parallel row is hidden");}
  auto* initialDrive=editor->findChildWithID("drive");require(initialDrive&&!initialDrive->isVisible(),"Editors must start hidden");
  capture("four-row-grid-1184x864.png",1184,864);
  capture("four-row-compact-822x600.png",822,600);
  editor->setEditorsVisible(true);
  auto* tone=dynamic_cast<juce::Slider*>(editor->findChildWithID("tone"));
  require(tone!=nullptr,"Tone control missing");
  auto* toneValue=dynamic_cast<juce::Label*>(editor->findChildWithID("toneValue"));
  require(toneValue!=nullptr&&toneValue->getText()=="60.0 %","Initial tone value was not formatted before interaction");
  auto* drive=dynamic_cast<juce::Slider*>(editor->findChildWithID("drive"));require(drive!=nullptr,"Drive control missing");
  drive->setValue(.73,juce::sendNotificationSync);
  require(std::abs(processor->parameters.getRawParameterValue("drive")->load()-.73f)<.001f,"Drive control did not update processor");
  auto* driveValue=dynamic_cast<juce::Label*>(editor->findChildWithID("driveValue"));require(driveValue!=nullptr,"Editable drive readout missing");
  driveValue->setText("61.0 %",juce::sendNotificationSync);
  require(std::abs(processor->parameters.getRawParameterValue("drive")->load()-.61f)<.001f,"Editable value did not update processor");
  drive->setValue(.35,juce::sendNotificationSync);
  capture("reference-1184x864.png",1184,864);
  capture("serial-1120x660.png",1120,660);
  capture("compact-822x600.png",822,600);
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
  editor->selectSlot(3,2);
  capture("four-row-editors-1184x864.png",1184,864);
  for(int r=0;r<4;++r){auto* cell=editor->findChildWithID("grid-"+juce::String(r)+"-2");require(cell&&cell->isVisible(),"A row disappeared while editing");}
  editor->setEditorsVisible(false);
  editor->selectSlot(3,2);require(editor->insertBlock(5),"Fourth row block insertion failed");
  {const auto grid=processor->getEditableGrid();require(grid.get(3,2)->type==vc::BlockType::Delay,"Fourth row edit updated wrong lane");}
  capture("four-row-delay-1184x864.png",1184,864);
  editor->setEditorsVisible(true);
  bool openedOutput=false;
  for(int i=0;i<editor->getNumChildComponents();++i)if(auto* button=dynamic_cast<CortexBlockButton*>(editor->getChildComponent(i)))if(button->rail&&button->type==8){button->onClick();openedOutput=true;break;}
  auto* trim=dynamic_cast<juce::Slider*>(editor->findChildWithID("outputTrim"));
  require(openedOutput&&trim&&trim->isVisible()&&trim->isEnabled(),"Grid output trim UI is disabled");
  capture("grid-output-1184x864.png",1184,864);
  editor.reset();processor->releaseResources();
  std::cout<<"PASS: native editor snapshots at three sizes, control binding, insertion and bypass\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
