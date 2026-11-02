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
  for(int i=0;i<editor->getNumChildComponents();++i)if(auto* button=dynamic_cast<CortexBlockButton*>(editor->getChildComponent(i)))if(button->rail&&button->type==22){button->onClick();openedOutput=true;break;}
  auto* trim=dynamic_cast<juce::Slider*>(editor->findChildWithID("outputTrim"));
  require(openedOutput&&trim&&trim->isVisible()&&trim->isEnabled(),"Grid output trim UI is disabled");
  capture("grid-output-1184x864.png",1184,864);
  for(int type:{8,9,10,11,12,13,14,15}){
   editor->selectSlot(2,(type-8)%8);require(editor->insertBlock(type),"New effect insertion failed");
   auto* control=editor->findChildWithID(type>=11&&type<=13?"slot-param-4":"slot-param-0");require(control&&control->isVisible(),"New effect control missing");
   const auto filename=juce::String("original-effect-")+juce::String(type)+".png";capture(filename.toRawUTF8(),1184,864);
  }
  editor->setEditorsVisible(false);capture("four-row-new-effects.png",1184,864);
  processor->parameters.getParameter("gridMode")->setValueNotifyingHost(0.f);editor->selectSlot(3,6);
  bool chorusCategory=false;
  for(int i=0;i<editor->getNumChildComponents();++i)if(auto* button=dynamic_cast<CortexBlockButton*>(editor->getChildComponent(i)))if(button->rail&&button->type==11){button->onClick();chorusCategory=true;break;}
  auto* chorusPreset=dynamic_cast<juce::Button*>(editor->findChildWithID("device-1"));require(chorusCategory&&chorusPreset&&chorusPreset->isVisible(),"Chorus browser category missing");chorusPreset->onClick();
  require(processor->parameters.getRawParameterValue("gridMode")->load()>.5f,"New device selection did not enable Grid mode");
  require(processor->getEditableGrid().get(3,6)->type==vc::BlockType::Chorus,"Chorus browser selection inserted wrong block");
  require(std::abs(processor->getGridSlotParameters(3,6).rate-.04f)<1.e-5f,"Chorus preset parameters not applied");
  editor->setEditorsVisible(true);capture("chorus-browser-preset.png",1184,864);
  auto* category=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("device-category"));require(category&&category->getNumItems()==23,"Expanded category selector missing");
  for(int type:{16,17,18,19,20,21}){
   editor->selectSlot(1,type-16);category->setSelectedId(type,juce::sendNotificationSync);
   auto* entry=dynamic_cast<juce::Button*>(editor->findChildWithID("device-0"));require(entry&&entry->isVisible(),"New category has no selectable device");entry->onClick();
   require((int)processor->getEditableGrid().get(1,type-16)->type==type,"New category inserted wrong block");
   const auto name=juce::String("expanded-effect-")+juce::String(type)+".png";capture(name.toRawUTF8(),1184,864);
  }
  editor->setEditorsVisible(false);capture("four-row-expanded-catalog.png",1184,864);
  editor->selectSlot(0,5);require(editor->insertBlock(5),"Delay insertion failed");editor->setEditorsVisible(true);
  auto* repeat=dynamic_cast<juce::ToggleButton*>(editor->findChildWithID("repeat-fx"));require(repeat&&repeat->isVisible(),"Repeat FX control missing");repeat->setToggleState(true,juce::dontSendNotification);repeat->onClick();
  require(processor->getGridSlotParameters(0,5).repeatFx,"Repeat FX enable was not applied");
  auto* cutoff=dynamic_cast<juce::Slider*>(editor->findChildWithID("slot-param-5"));require(cutoff&&cutoff->isVisible(),"Loop cutoff control missing");cutoff->setValue(.4,juce::sendNotificationSync);require(processor->getGridSlotParameters(0,5).repeatFx,"Editing a knob cleared Repeat FX");
  capture("delay-repeat-fx.png",1184,864);editor->setEditorsVisible(false);require(!repeat->isVisible(),"Repeat FX control remains over hidden editor");capture("delay-grid-only.png",1184,864);
  category->setSelectedId(23,juce::sendNotificationSync);auto* stereoPreset=dynamic_cast<juce::Button*>(editor->findChildWithID("device-1"));require(stereoPreset&&stereoPreset->isVisible(),"Stereo delay preset missing");stereoPreset->onClick();
  require(processor->parameters.getRawParameterValue("stereoDelayOn")->load()>.5f,"Stereo delay did not enable");require(processor->parameters.getRawParameterValue("sdPingPong")->load()>.5f,"Ping-pong preset not applied");editor->setEditorsVisible(true);
  for(int page=0;page<3;++page){auto* button=dynamic_cast<juce::Button*>(editor->findChildWithID("stereo-page-"+juce::String(page)));require(button&&button->isVisible(),"Stereo delay page missing");button->onClick();const auto name=juce::String("stereo-delay-page-")+juce::String(page)+".png";capture(name.toRawUTF8(),1184,864);}
  auto* wet=dynamic_cast<juce::Slider*>(editor->findChildWithID("sdWetLevel"));require(wet&&wet->isVisible(),"Deep stereo delay control hidden");wet->setValue(-6,juce::sendNotificationSync);require(std::abs(processor->parameters.getRawParameterValue("sdWetLevel")->load()+6)<.001f,"Stereo delay control did not bind to parameter");
  processor->parameters.getParameter("sdSync")->setValueNotifyingHost(1.f);auto* mainPage=dynamic_cast<juce::Button*>(editor->findChildWithID("stereo-page-0"));mainPage->onClick();require(!editor->findChildWithID("sdTimeL")->isEnabled(),"Manual time not disabled in tempo sync");require(!editor->findChildWithID("sdCross")->isEnabled(),"Manual cross feedback not disabled in ping pong");
  processor->parameters.getParameter("sdSync")->setValueNotifyingHost(0.f);processor->parameters.getParameter("sdPingPong")->setValueNotifyingHost(0.f);mainPage->onClick();require(editor->findChildWithID("sdTimeL")->isEnabled()&&editor->findChildWithID("sdCross")->isEnabled(),"Manual stereo controls did not re-enable");
  editor->setEditorsVisible(false);for(int i=0;i<3;++i)require(!editor->findChildWithID("stereo-page-"+juce::String(i))->isVisible(),"Stereo page remains over grid view");capture("stereo-delay-grid-only.png",1184,864);
  editor.reset();processor->releaseResources();
  std::cout<<"PASS: native editor snapshots at three sizes, control binding, insertion and bypass\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
