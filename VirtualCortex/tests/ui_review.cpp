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
  auto capture=[&](const juce::String& name,int w,int h){
   editor->setSize(w,h);
   for(int i=0;i<editor->getNumChildComponents();++i)if(auto* slider=dynamic_cast<juce::Slider*>(editor->getChildComponent(i));slider&&slider->isVisible()&&slider->getSliderStyle()==juce::Slider::RotaryHorizontalVerticalDrag)require(slider->getBounds().getWidth()==slider->getBounds().getHeight(),"Rotary control distorted by widescreen layout");
   for(int i=0;i<editor->getNumChildComponents();++i){auto* child=editor->getChildComponent(i);if(child->isVisible())require(editor->getLocalBounds().contains(child->getBoundsInParent()),"A visible control is clipped by the window");}
   if(w*9==h*16){
    auto* route=editor->findChildWithID("row-route-3");require(route!=nullptr,"Row output missing");
    require(route->getBoundsInParent().getRight()>w-w/40,"Wide layout leaves a right margin");
    auto* category=editor->findChildWithID("device-category");require(category&&category->getBoundsInParent().getX()<w/10,"Wide layout leaves a left margin");
   }
   const auto image=editor->createComponentSnapshot(editor->getLocalBounds());
   require(image.isValid(),"Editor snapshot failed");
   auto out=dir.getChildFile(name).createOutputStream();require(out!=nullptr,"PNG output failed");
   juce::PNGImageFormat png;require(png.writeImageToStream(image,*out),"PNG encoder failed");
  };
  {
   auto* openCatalog=dynamic_cast<juce::Button*>(editor->findChildWithID("catalog-toggle"));
   auto* catalog=dynamic_cast<DeviceCatalogPanel*>(editor->findChildWithID("device-catalog-panel"));
   require(openCatalog&&catalog&&!catalog->isVisible(),"Catalog must start hidden");
   openCatalog->onClick();require(catalog->isVisible()&&catalog->visibleEntryCount()==724,"Catalog is incomplete");
   auto* search=dynamic_cast<juce::TextEditor*>(catalog->findChildWithID("catalog-search"));
   auto* availability=dynamic_cast<juce::ComboBox*>(catalog->findChildWithID("catalog-availability"));
   auto* section=dynamic_cast<juce::ComboBox*>(catalog->findChildWithID("catalog-section"));
   auto* action=dynamic_cast<juce::Button*>(catalog->findChildWithID("catalog-action"));
   require(search&&availability&&section&&action,"Catalog controls missing");
   auto callback=catalog->onChooseOriginal;int chosen=-1;catalog->onChooseOriginal=[&](int type){chosen=type;};
   auto query=[&](const char* value){search->setText(value,false);search->onTextChange();};
   query("bRiT 2203 87");require(catalog->visibleEntryCount()==1&&catalog->selectedPlayableType()==0&&!action->isEnabled(),"Reference entry presented as playable");
   action->onClick();require(chosen==-1,"Unavailable model invoked insertion");
   capture("catalog-reference-1184x864.png",1184,864);
   query("Marshall JCM800");require(catalog->visibleEntryCount()>0&&catalog->selectedPlayableType()==0,"Gear search requires trademark symbols");
   query("");availability->setSelectedId(2,juce::sendNotificationSync);require(catalog->visibleEntryCount()==35,"Original count incorrect");
   query("Bit-crusher");require(catalog->visibleEntryCount()==1&&action->isEnabled(),"Working original cannot be selected");
   action->onClick();require(chosen==15,"Original action routed to wrong processor");
   query("Four-head Echo");action->onClick();require(chosen==25,"Post-chain action routed to wrong processor");
   query("Grid Stereo Delay");action->onClick();require(chosen==30,"Grid delay catalog insertion failed");
   query("Grid Stereo Reverb");action->onClick();require(chosen==31,"Grid reverb catalog insertion failed");
   query("");availability->setSelectedId(4,juce::sendNotificationSync);require(catalog->visibleEntryCount()==57&&!action->isEnabled(),"Announced entries not isolated");
   availability->setSelectedId(3,juce::sendNotificationSync);require(catalog->visibleEntryCount()==632,"Released reference count incorrect");
   availability->setSelectedId(1,juce::sendNotificationSync);
   for(int i=0;i<section->getNumItems();++i)if(section->getItemText(i)=="Guitar amps")section->setSelectedId(section->getItemId(i),juce::sendNotificationSync);
   require(catalog->visibleEntryCount()==83&&!action->isEnabled(),"Guitar amp section incomplete");
   capture("catalog-guitar-amps-2560x1440.png",2560,1440);
   capture("catalog-guitar-amps-822x600.png",822,600);
   query("zz-no-matching-device-zz");require(catalog->visibleEntryCount()==0&&!action->isEnabled(),"Empty search retained a stale action");
   catalog->onChooseOriginal=callback;
   require(editor->keyPressed(juce::KeyPress(juce::KeyPress::escapeKey))&&!catalog->isVisible(),"Escape did not close catalog");
  }
  for(int r=0;r<4;++r){auto* lane=editor->findChildWithID("lane-"+juce::String(r));require(lane&&lane->isVisible(),"A parallel row is hidden");}
  auto* initialDrive=editor->findChildWithID("drive");require(initialDrive&&!initialDrive->isVisible(),"Editors must start hidden");
  require(!editor->findChildWithID("fullscreen-toggle")->isVisible(),"Fullscreen leaked into plugin editor");
  require(!editor->setFullScreenMode(true),"Plugin attempted to fullscreen host window");
  capture("four-row-grid-2560x1440.png",2560,1440);
  capture("four-row-grid-1707x960.png",1707,960); // 1440p at 150% Windows scaling
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
  capture("editors-2560x1440.png",2560,1440);
  capture("editors-1707x960.png",1707,960);
  {
   juce::AudioProcessor::setTypeOfNextNewPlugin(juce::AudioProcessor::wrapperType_Standalone);
   auto standaloneProcessor=std::make_unique<VirtualCortexProcessor>();
   juce::AudioProcessor::setTypeOfNextNewPlugin(juce::AudioProcessor::wrapperType_Undefined);
   auto standaloneEditor=std::make_unique<VirtualCortexEditor>(*standaloneProcessor);
   juce::ResizableWindow window("Fullscreen regression",juce::Colours::black,true);
   window.setUsingNativeTitleBar(true);
   window.setContentNonOwned(standaloneEditor.get(),false);
   window.setBounds(60,60,1000,750);window.setVisible(true);
   const auto bounds=window.getBounds();
   require(standaloneEditor->findChildWithID("fullscreen-toggle")->isVisible(),"Standalone fullscreen button missing");
   for(int cycle=0;cycle<3;++cycle){
    require(standaloneEditor->keyPressed(juce::KeyPress(juce::KeyPress::F11Key))&&window.isFullScreen(),"F11 fullscreen entry failed");
    require(standaloneEditor->keyPressed(juce::KeyPress(juce::KeyPress::escapeKey))&&!window.isFullScreen(),"Escape fullscreen exit failed");
    require(window.getBounds()==bounds,"Windowed bounds not restored");
   }
   auto* fullscreen=dynamic_cast<juce::Button*>(standaloneEditor->findChildWithID("fullscreen-toggle"));
   fullscreen->onClick();require(window.isFullScreen(),"Fullscreen button failed");
   require(standaloneEditor->keyPressed(juce::KeyPress(juce::KeyPress::F11Key))&&!window.isFullScreen(),"F11 fullscreen exit failed");
   require(!standaloneEditor->keyPressed(juce::KeyPress(juce::KeyPress::escapeKey)),"Windowed Escape consumed");
   window.clearContentComponent();
  }
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
  auto* category=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("device-category"));require(category&&category->getNumItems()==36,"Expanded category selector missing");
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
  editor->setEditorsVisible(true);
  auto* cloud=dynamic_cast<juce::Button*>(editor->findChildWithID("device-5"));require(cloud&&cloud->isVisible(),"Diffuse preset missing");cloud->onClick();
  auto* diffusion=dynamic_cast<juce::Slider*>(editor->findChildWithID("sdDiffusion"));require(diffusion&&diffusion->isVisible(),"Loop diffusion hidden");
  require(processor->parameters.getRawParameterValue("sdDiffusion")->load()==1.f,"Diffuse preset not applied");
  diffusion->setValue(.63,juce::sendNotificationSync);require(std::abs(processor->parameters.getRawParameterValue("sdDiffusion")->load()-.63f)<.001f,"Diffusion control not bound");
  capture("diffuse-delay-2560x1440.png",2560,1440);capture("diffuse-delay-822x600.png",822,600);
  auto* bloom=dynamic_cast<juce::Button*>(editor->findChildWithID("device-6"));require(bloom&&bloom->isVisible(),"Bloom preset missing");bloom->onClick();require(processor->parameters.getRawParameterValue("sdDucking")->load()>.6f,"Bloom ducking missing");
  stereoPreset->onClick();require(processor->parameters.getRawParameterValue("sdDiffusion")->load()==0.f,"Standard preset retained diffusion");
  category->setSelectedId(24,juce::sendNotificationSync);auto* reverbPreset=dynamic_cast<juce::Button*>(editor->findChildWithID("device-4"));require(reverbPreset&&reverbPreset->isVisible(),"Stereo reverb preset missing");reverbPreset->onClick();require(processor->parameters.getRawParameterValue("stereoReverbOn")->load()>.5f,"Stereo reverb did not enable");require(processor->parameters.getRawParameterValue("srDucking")->load()>.7f,"Ducked preset missing");require(processor->parameters.getRawParameterValue("srPreGlide")->load()<.5f,"New reverb preset not using clean pre-delay");editor->setEditorsVisible(true);
  auto* glide=dynamic_cast<juce::ToggleButton*>(editor->findChildWithID("reverb-pre-glide"));require(glide&&glide->isVisible(),"Reverb pre-delay mode switch missing");glide->setToggleState(true,juce::sendNotificationSync);require(processor->parameters.getRawParameterValue("srPreGlide")->load()>.5f,"Pre-delay mode not bound");glide->setToggleState(false,juce::sendNotificationSync);
  for(int page=0;page<3;++page){auto* button=dynamic_cast<juce::Button*>(editor->findChildWithID("stereo-page-reverb-"+juce::String(page)));require(button&&button->isVisible(),"Stereo reverb page missing");button->onClick();const auto name=juce::String("stereo-reverb-page-")+juce::String(page)+".png";capture(name.toRawUTF8(),1184,864);}
  auto* width=dynamic_cast<juce::Slider*>(editor->findChildWithID("srWidth"));require(width&&width->isVisible(),"Reverb width missing");width->setValue(.5,juce::sendNotificationSync);require(std::abs(processor->parameters.getRawParameterValue("srWidth")->load()-.5f)<.001,"Reverb control not bound");editor->setEditorsVisible(false);for(int i=0;i<3;++i)require(!editor->findChildWithID("stereo-page-reverb-"+juce::String(i))->isVisible(),"Reverb page over grid view");capture("stereo-reverb-grid-only.png",1184,864);

  category->setSelectedId(25,juce::sendNotificationSync);editor->setEditorsVisible(true);
  auto* echoPreset=dynamic_cast<juce::Button*>(editor->findChildWithID("device-0"));echoPreset->onClick();
  require(processor->parameters.getRawParameterValue("mhOn")->load()>.5f,"Echo preset did not enable");
  for(int page=0;page<4;++page){auto* b=dynamic_cast<juce::Button*>(editor->findChildWithID("echo-head-"+juce::String(page)));require(b&&b->isVisible(),"Echo page missing");b->onClick();
   auto* slider=dynamic_cast<juce::Slider*>(editor->findChildWithID("mh"+juce::String(page+1)+"Time"));require(slider&&slider->isVisible(),"Echo time hidden");slider->setValue(456,juce::sendNotificationSync);
   require(std::abs(processor->parameters.getRawParameterValue("mh"+juce::String(page+1)+"Time")->load()-456)<.01f,"Echo time not bound");}
  capture("four-head-echo-2560x1440.png",2560,1440);capture("four-head-echo-822x600.png",822,600);
  editor->setEditorsVisible(false);require(!editor->findChildWithID("echo-head-0")->isVisible(),"Echo controls remain over hidden editor");
  for(int type:{26,27}){category->setSelectedId(type,juce::sendNotificationSync);editor->setEditorsVisible(true);auto* preset=dynamic_cast<juce::Button*>(editor->findChildWithID("device-0"));require(preset&&preset->isVisible(),"Creative preset missing");preset->onClick();
   auto* knob=dynamic_cast<juce::Slider*>(editor->findChildWithID(type==26?"ieA":"llRate"));require(knob&&knob->isVisible(),"Creative control hidden");knob->setValue(type==26?12:1.5,juce::sendNotificationSync);
   require(std::abs(processor->parameters.getRawParameterValue(type==26?"ieA":"llRate")->load()-(type==26?12:1.5))<.001,"Creative binding failed");
   capture(type==26?"interval-echo-2560x1440.png":"layer-loom-2560x1440.png",2560,1440);capture(type==26?"interval-echo-822x600.png":"layer-loom-822x600.png",822,600);
  }
  for(int i=0;i<6;++i)require(editor->findChildWithID("loop-command-"+juce::String(i))->isVisible(),"Loop transport missing");
  editor->setEditorsVisible(false);require(!editor->findChildWithID("loop-command-0")->isVisible(),"Loop transport over hidden editor");
  for(int type:{28,29}){category->setSelectedId(type,juce::sendNotificationSync);editor->setEditorsVisible(true);auto* preset=dynamic_cast<juce::Button*>(editor->findChildWithID("device-0"));require(preset&&preset->isVisible(),"Tracking effect preset missing");preset->onClick();
   auto* page=dynamic_cast<juce::Button*>(editor->findChildWithID("tracking-page"));require(page&&page->isVisible(),"Tracking page missing");
   auto* mainKnob=dynamic_cast<juce::Slider*>(editor->findChildWithID(type==28?"syMix":"scBits"));if(!mainKnob->isVisible())page->onClick();require(mainKnob->isVisible(),"Tracking main control missing");mainKnob->setValue(type==28?.6:6,juce::sendNotificationSync);require(std::abs(processor->parameters.getRawParameterValue(type==28?"syMix":"scBits")->load()-(type==28?.6:6))<.001,"Tracking parameter binding failed");
   capture(type==28?"prism-synth-2560x1440.png":"step-crush-2560x1440.png",2560,1440);page->onClick();
   require(editor->findChildWithID(type==28?"syAttack":"scStep6")->isVisible(),"Tracking second page incomplete");capture(type==28?"prism-envelopes-2560x1440.png":"step-crush-sequence-2560x1440.png",2560,1440);
   capture(type==28?"prism-synth-822x600.png":"step-crush-822x600.png",822,600);editor->setEditorsVisible(false);require(!page->isVisible(),"Tracking pages over hidden editor");
  }
  for(int type:{26,29}){category->setSelectedId(type,juce::sendNotificationSync);editor->setEditorsVisible(true);const char* prefix=type==26?"ie":"sc";auto* toggle=dynamic_cast<juce::ToggleButton*>(editor->findChildWithID(type==26?"interval-sync":"crusher-sync"));require(toggle&&toggle->isVisible(),"Sequencer tempo toggle missing");toggle->setToggleState(true,juce::sendNotificationSync);
   auto* manual=editor->findChildWithID(type==26?"ieStep":"scStepMs");auto* beats=dynamic_cast<juce::Slider*>(editor->findChildWithID(juce::String(prefix)+"Beats"));if(!beats->isVisible()){auto* page=dynamic_cast<juce::Button*>(editor->findChildWithID("tracking-page"));page->onClick();}beats->setValue(.5,juce::sendNotificationSync);editor->setEditorsVisible(true);require(!manual->isEnabled(),"Manual time enabled during sync");require(std::abs(processor->parameters.getRawParameterValue(juce::String(prefix)+"Beats")->load()-.5f)<.001,"Beat control unbound");capture(type==26?"interval-tempo-2560x1440.png":"crusher-tempo-2560x1440.png",2560,1440);
  }
  category->setSelectedId(29,juce::sendNotificationSync);editor->setEditorsVisible(true);
  {auto* page=dynamic_cast<juce::Button*>(editor->findChildWithID("tracking-page"));auto* pitch=dynamic_cast<juce::ToggleButton*>(editor->findChildWithID("crusher-pitch"));if(!pitch->isVisible())page->onClick();require(pitch->isVisible(),"Pitch switch hidden");pitch->setToggleState(true,juce::sendNotificationSync);require(processor->parameters.getRawParameterValue("scPitch")->load()>.5f,"Pitch switch unbound");capture("crusher-pitch-2560x1440.png",2560,1440);capture("crusher-pitch-822x600.png",822,600);}
  {
   auto* a=dynamic_cast<juce::Button*>(editor->findChildWithID("quick-0"));
   auto* b=dynamic_cast<juce::Button*>(editor->findChildWithID("quick-1"));
   require(a&&b,"Scene controls missing");
   auto* amount=processor->parameters.getParameter("ieA");
   amount->setValueNotifyingHost(amount->convertTo0to1(7.f));a->onClick();
   amount->setValueNotifyingHost(amount->convertTo0to1(-5.f));b->onClick();
   a->onClick();require(processor->parameters.getRawParameterValue("ieA")->load()==7.f&&a->getToggleState(),"Scene A control failed");
   b->onClick();require(processor->parameters.getRawParameterValue("ieA")->load()==-5.f&&b->getToggleState()&&!a->getToggleState(),"Scene B control failed");
   capture("stored-scenes-2560x1440.png",2560,1440);
  }
  {
   auto* routing=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("grid-routing"));
   require(routing&&routing->isVisible(),"Routing selector missing");
   for(int id=1;id<=3;++id){routing->setSelectedId(id,juce::sendNotificationSync);require(processor->getRoutingMode()==id-1,"Routing selector unbound");}
   capture("dual-routing-2560x1440.png",2560,1440);
   capture("dual-routing-822x600.png",822,600);
   routing->setSelectedId(1,juce::sendNotificationSync);
  }
  {
   auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("device-category"));
   editor->selectSlot(0,7);menu->setSelectedId(30,juce::sendNotificationSync);
   auto* preset=dynamic_cast<juce::Button*>(editor->findChildWithID("device-0"));require(preset&&preset->isVisible(),"Grid stereo delay preset missing");preset->onClick();
   require(processor->getEditableGrid().get(0,7)->type==vc::BlockType::GridStereoDelay,"Grid delay not inserted");
   require(editor->findChildWithID("advanced-grid-editor")->findChildWithID("advanced-sdTimeR")->isVisible(),"Grid delay right time hidden");
   capture("grid-stereo-delay-2560x1440.png",2560,1440);
   editor->selectSlot(1,7);menu->setSelectedId(31,juce::sendNotificationSync);preset->onClick();
   require(processor->getEditableGrid().get(1,7)->type==vc::BlockType::GridStereoReverb,"Grid reverb not inserted");
   require(editor->findChildWithID("advanced-grid-editor")->findChildWithID("advanced-srDecay")->isVisible(),"Grid reverb decay hidden");
   capture("grid-stereo-reverb-2560x1440.png",2560,1440);
  }
  {
   auto* menu=dynamic_cast<juce::ComboBox*>(editor->findChildWithID("device-category"));
   auto* panel=editor->findChildWithID("advanced-grid-editor");
   for(int type=30;type<=36;++type){
    editor->selectSlot(2,7);menu->setSelectedId(type,juce::sendNotificationSync);
    dynamic_cast<juce::Button*>(editor->findChildWithID("device-0"))->onClick();
    require((int)processor->getEditableGrid().get(2,7)->type==type,"Advanced insertion failed");
    const auto& def=vc::advancedDefinitions[type-30];
    for(int i=0;i<def.count;++i){
     if(i&&i%8==0)dynamic_cast<juce::Button*>(panel->findChildWithID("advanced-next"))->onClick();
     auto* control=panel->findChildWithID("advanced-"+juce::String(def.controls[i].id));
     // Both slider and toggle carry the ID; select the visible component.
     for(auto* child:panel->getChildren())if(child->getComponentID()=="advanced-"+juce::String(def.controls[i].id)&&child->isVisible())control=child;
     require(control&&control->isVisible(),"Advanced full control missing");
     const float value=def.controls[i].toggle?1.f:def.controls[i].low+.37f*(def.controls[i].high-def.controls[i].low);
     if(auto* slider=dynamic_cast<juce::Slider*>(control))slider->setValue(value,juce::sendNotificationSync);
     if(auto* button=dynamic_cast<juce::ToggleButton*>(control)){button->setToggleState(true,juce::dontSendNotification);button->onClick();}
     require(std::abs(processor->getAdvancedValues(2,7)[i]-value)<.01f,"Advanced slot control unbound");
     if(i%8==0)capture("advanced-"+juce::String(type)+"-page-"+juce::String(i/8+1)+"-2560x1440.png",2560,1440);
    }
   }
  }
#if VC_HAS_NAM
  {
   const auto fixture=dir.getChildFile("Native NAM test.nam");fixture.replaceWithText(R"({"version":"0.5.4","architecture":"Linear","config":{"receptive_field":1,"bias":false},"weights":[0.5],"sample_rate":48000,"metadata":{}})");
   editor->selectSlot(0,0);editor->insertBlock(2);require(processor->loadGridNeuralModel(fixture,0,0),"UI NAM fixture failed");editor->selectSlot(0,0);editor->setEditorsVisible(true);
   require(editor->findChildWithID("slot-param-1")->isVisible(),"NAM output gain hidden");
   capture("live-nam-2560x1440.png",2560,1440);capture("live-nam-822x600.png",822,600);
   auto* clear=dynamic_cast<juce::Button*>(editor->findChildWithID("clear-amp-model"));require(clear&&clear->isVisible(),"NAM removal missing");clear->onClick();require(!processor->isGridNAM(0,0)&&!editor->findChildWithID("slot-param-1")->isVisible(),"NAM removal unbound");
  }
#endif
  {
   auto* open=dynamic_cast<juce::Button*>(editor->findChildWithID("row-route-0"));require(open!=nullptr,"Routing button missing");open->onClick();
   auto* panel=editor->findChildWithID("routing-editor");require(panel&&panel->isVisible(),"Routing panel hidden");
   require(!panel->findChildWithID("route-source-0-1")->isEnabled(),"Self feedback offered in UI");
   dynamic_cast<juce::Button*>(panel->findChildWithID("route-preset-1"))->onClick();
   require(processor->getRoutingMode()==3&&processor->getRowRouting().rows[3].sources==12,"Split preset not bound");
   require(dynamic_cast<juce::ComboBox*>(editor->findChildWithID("grid-routing"))->getSelectedId()==4,"Routing header stale after template selection");
   for(int r=0;r<4;++r)require(processor->isGridLaneEnabled(r),"Routing template left a row disabled");
   auto* lane=dynamic_cast<juce::ToggleButton*>(panel->findChildWithID("route-lane-3"));require(lane!=nullptr,"Routing lane switch missing");lane->setToggleState(false,juce::dontSendNotification);lane->onClick();require(!processor->isGridLaneEnabled(3),"Routing lane switch unbound");lane->setToggleState(true,juce::dontSendNotification);lane->onClick();
   dynamic_cast<juce::Slider*>(panel->findChildWithID("route-gain-1"))->setValue(-6,juce::sendNotificationSync);
   require(processor->getRowRouting().rows[1].gainDb==-6,"Route level unbound");
   auto* balanceControl=dynamic_cast<juce::Slider*>(panel->findChildWithID("route-balance-1"));require(balanceControl&&balanceControl->getTextFromValue(0)=="CENTER","Center balance formatting missing");
   balanceControl->setValue(-.5,juce::sendNotificationSync);require(processor->getRowRouting().rows[1].balance==-.5f&&balanceControl->getTextFromValue(-.5)=="L 50%","Balance control unbound");balanceControl->setValue(0,juce::sendNotificationSync);
   auto* entry=dynamic_cast<juce::ComboBox*>(panel->findChildWithID("route-entry-1"));auto* tap=dynamic_cast<juce::ComboBox*>(panel->findChildWithID("route-tap-1-0"));require(entry&&tap&&tap->isEnabled(),"Column routing controls missing");
   entry->setSelectedId(5,juce::sendNotificationSync);tap->setSelectedId(4,juce::sendNotificationSync);require(processor->getRowRouting().rows[1].entryColumn==4&&processor->getRowRouting().rows[1].sourceTaps[0]==3,"Column routing controls unbound");
   require(tap->getText()=="C>"&&tap->getTooltip().contains("Row 1 after C"),"Tap label or accessible description stale");
   tap->setSelectedId(1,juce::sendNotificationSync);require(tap->getText()=="<A"&&tap->getTooltip().contains("before A")&&processor->getRowRouting().rows[1].sourceTaps[0]==0,"Before-input tap label unbound");tap->setSelectedId(4,juce::sendNotificationSync);
   auto* skipped=dynamic_cast<CortexBlockButton*>(editor->findChildWithID("grid-1-0"));require(skipped&&skipped->routingSkipped&&!skipped->blockEnabled,"Skipped prefix still shown active");
   capture("custom-routing-2560x1440.png",2560,1440);capture("custom-routing-822x600.png",822,600);
   dynamic_cast<juce::Button*>(panel->findChildWithID("routing-close"))->onClick();require(!panel->isVisible(),"Routing close failed");
   editor->setEditorsVisible(true);capture("custom-grid-editors-2560x1440.png",2560,1440);capture("custom-grid-editors-822x600.png",822,600);
   editor->setEditorsVisible(false);capture("custom-grid-2560x1440.png",2560,1440);capture("custom-grid-1920x1080.png",1920,1080);capture("custom-grid-1600x900.png",1600,900);
   editor->selectSlot(3,0);capture("custom-routing-focus-row4-2560x1440.png",2560,1440);capture("custom-routing-focus-row4-822x600.png",822,600);
   editor->selectSlot(1,4);capture("custom-routing-focus-row2-2560x1440.png",2560,1440);editor->selectSlot(0,0);


   for(const auto size:{juce::Point<int>(2560,1440),juce::Point<int>(822,600)}){
    editor->setSize(size.x,size.y);editor->setVisible(true);
    const float scale=juce::jmin(size.x/1184.f,size.y/864.f),width=size.x/scale;
    const float offsetY=(size.y-864.f*scale)*.5f;
    // R1 after C to R2 entry E: horizontal gutter above the destination row.
    const float x=(302.f+((357.f+93.f*3.f-6.f+357.f+93.f*4.f)*.5f-302.f)*(width-312.f)/872.f)*scale;
    const juce::Point<float> point{x,311.f*scale+offsetY};
    auto* hitComponent=editor->getComponentAt(point.roundToInt());
    if(hitComponent!=editor.get())std::cerr<<"Wire hit at "<<size.x<<"x"<<size.y<<": "<<(hitComponent?hitComponent->getComponentID().toStdString():"null")<<" name="<<(hitComponent?hitComponent->getName().toStdString():"null")<<"\\n";
    require(hitComponent==editor.get(),"Wire click intercepted by child control");
    editor->selectRoutingWireAt(point);
    require(editor->selectedRoutingWire()==std::array<int,2>{0,1},"Native wire selection missed source rail");
    capture("selected-wire-"+juce::String(size.x)+"x"+juce::String(size.y)+".png",size.x,size.y);
    editor->selectRoutingWireAt({1.f,1.f});
    require(editor->selectedRoutingWire()==std::array<int,2>{-1,-1},"Background miss retained selection");

    const auto savedRoutes=processor->getRowRouting();
    for(int mutation=0;mutation<4;++mutation){
     require(processor->setRowRouting(savedRoutes),"Restore routing for focus test failed");
     processor->setRoutingMode(3);editor->setEditorsVisible(false);editor->selectRoutingWireAt(point);
     require(editor->selectedRoutingWire()==std::array<int,2>{0,1},"Wire focus setup failed");
     auto changed=savedRoutes;
     if(mutation==0)changed.rows[1].sources&=~2u;
     if(mutation==1)changed.rows[1].sourceTaps[0]=(changed.rows[1].sourceTaps[0]+1)%9;
     if(mutation==2)changed.rows[1].entryColumn=(changed.rows[1].entryColumn+1)%8;
     if(mutation==3)processor->setRoutingMode(0);
     require(processor->setRowRouting(changed),"Routing mutation rejected");
     editor->setEditorsVisible(false);
     require(editor->selectedRoutingWire()==std::array<int,2>{-1,-1},"Changed topology retained stale wire focus");
    }
    require(processor->setRowRouting(savedRoutes),"Final routing restore failed");
    processor->setRoutingMode(3);editor->setEditorsVisible(false);

    // The actual file-load path must report rejection without disturbing state/focus.
    {
     editor->selectRoutingWireAt(point);
     const auto folder=dir.getChildFile("preset-file-regression");require(folder.createDirectory().wasOk(),"Preset test directory failed");
     const auto file=folder.getChildFile("test.vcpreset");file.deleteFile();
     auto* status=dynamic_cast<juce::Label*>(editor->findChildWithID("preset-status"));require(status!=nullptr,"Preset status missing");
     juce::MemoryBlock saved;processor->getStateInformation(saved);
     const auto before=processor->stateRecallRevision();
     auto rejected=[&](const char* message){
      require(!editor->loadPresetFile(file),"Invalid preset file accepted");
      require(status->getText()==message,"Preset rejection status incorrect");
      require(processor->stateRecallRevision()==before,"Rejected file published recall");
      require(editor->selectedRoutingWire()==std::array<int,2>{0,1},"Rejected file cleared wire selection");
      juce::MemoryBlock after;processor->getStateInformation(after);require(after==saved,"Rejected file changed serialized state");
     };
     rejected("Preset not found.");
     require(file.replaceWithData("",0),"Empty preset fixture failed");rejected("Invalid preset; state unchanged.");
     require(file.replaceWithText("not a preset"),"Corrupt preset fixture failed");rejected("Invalid preset; state unchanged.");
     juce::MemoryBlock oversized(1024*1024+1,true);require(file.replaceWithData(oversized.getData(),oversized.getSize()),"Oversize fixture failed");rejected("Preset exceeds 1 MB limit.");
     capture("preset-rejected-"+juce::String(size.x)+"x"+juce::String(size.y)+".png",size.x,size.y);
     require(file.replaceWithData(saved.getData(),saved.getSize()),"Valid preset fixture failed");
     require(editor->loadPresetFile(file)&&status->getText()=="Preset loaded","Valid preset not reported");
     require(processor->stateRecallRevision()==before+1,"File restore did not publish exactly once");
     require(editor->selectedRoutingWire()==std::array<int,2>{-1,-1},"Valid file recall kept stale selection");
     capture("preset-loaded-"+juce::String(size.x)+"x"+juce::String(size.y)+".png",size.x,size.y);
     require(folder.deleteRecursively(),"Preset test cleanup failed");
     editor->selectRoutingWireAt(point);
    }
    // A new preset/state is a new editing context, even with identical routing.
    editor->selectRoutingWireAt(point);
    const auto revision=processor->stateRecallRevision();
    auto gainRoutes=processor->getRowRouting();gainRoutes.rows[1].gainDb=-3.f;
    require(processor->setRowRouting(gainRoutes),"Gain-only route edit failed");
    editor->setEditorsVisible(false);
    require(editor->selectedRoutingWire()==std::array<int,2>{0,1},"Gain edit unexpectedly cleared wire focus");
    require(processor->setRowRouting(savedRoutes),"Gain edit restore failed");
    const char invalidState[]="not a JUCE state";
    processor->setStateInformation(invalidState,sizeof(invalidState));
    editor->setEditorsVisible(false);
    require(processor->stateRecallRevision()==revision&&editor->selectedRoutingWire()==std::array<int,2>{0,1},"Rejected state changed recall identity or focus");
    juce::MemoryBlock recalledState;processor->getStateInformation(recalledState);
    for(int recall=0;recall<2;++recall){
     editor->selectRoutingWireAt(point);
     require(editor->selectedRoutingWire()==std::array<int,2>{0,1},"Recall focus setup failed");
     processor->setStateInformation(recalledState.getData(),(int)recalledState.getSize());
     require(processor->stateRecallRevision()==revision+recall+1,"Valid state did not publish recall identity");
     editor->setEditorsVisible(false);
     require(editor->selectedRoutingWire()==std::array<int,2>{-1,-1},"Identical-topology state recall retained wire focus");
     const auto restoredRoutes=processor->getRowRouting();
     require(processor->getRoutingMode()==3&&restoredRoutes.rows[1].sources==savedRoutes.rows[1].sources&&restoredRoutes.rows[1].sourceTaps==savedRoutes.rows[1].sourceTaps&&restoredRoutes.rows[1].entryColumn==savedRoutes.rows[1].entryColumn,"Focus clearing changed recalled routing");
    }
    capture("recalled-wire-cleared-"+juce::String(size.x)+"x"+juce::String(size.y)+".png",size.x,size.y);
    processor->setStateInformation(recalledState.getData(),(int)recalledState.getSize());
    editor->selectRoutingWireAt(point);editor->setEditorsVisible(false);
    require(editor->selectedRoutingWire()==std::array<int,2>{0,1},"New selection after recall was cleared by delayed refresh");

    // Shared popup action: reject stale edits, remove exactly one source bit,
    // preserve unrelated input/rows, and persist the edited route in a preset.
    auto multiSource=savedRoutes;multiSource.rows[1].sources|=1u;
    require(processor->setRowRouting(multiSource),"Disconnect test setup failed");
    editor->setEditorsVisible(false);editor->selectRoutingWireAt(point);
    const auto menuRevision=processor->routingRevision();const auto menuRecall=processor->stateRecallRevision();
    require(!processor->disconnectRowSource(-1,1,menuRevision,menuRecall),"Invalid source accepted");
    require(!processor->disconnectRowSource(1,1,menuRevision,menuRecall),"Self connection accepted");
    require(!processor->disconnectRowSource(0,4,menuRevision,menuRecall),"Invalid destination accepted");
    require(processor->routingRevision()==menuRevision,"Rejected action mutated routing");
    auto gainChange=multiSource;gainChange.rows[1].gainDb=-2.f;
    require(processor->setRowRouting(gainChange),"Stale-action setup failed");
    require(!editor->disconnectRoutingWire(0,1,menuRevision,menuRecall),"Stale routing menu removed a connection");
    require(processor->getRowRouting().rows[1].sources==multiSource.rows[1].sources,"Stale action changed sources");
    require(processor->setRowRouting(multiSource),"Restore multi-source routing failed");
    juce::MemoryBlock beforeDisconnect;processor->getStateInformation(beforeDisconnect);
    processor->setStateInformation(beforeDisconnect.getData(),(int)beforeDisconnect.getSize());
    require(!editor->disconnectRoutingWire(0,1,processor->routingRevision(),menuRecall),"Stale recall menu removed a connection");
    const auto currentRevision=processor->routingRevision();const auto currentRecall=processor->stateRecallRevision();
    processor->setRoutingMode(0);
    require(!editor->disconnectRoutingWire(0,1,currentRevision,currentRecall),"Disconnect accepted outside custom mode");
    processor->setRoutingMode(3);
    editor->selectRoutingWireAt(point);
    require(editor->disconnectRoutingWire(0,1,currentRevision,currentRecall),"Current connection removal failed");
    const auto disconnected=processor->getRowRouting();
    for(int row=0;row<4;++row){
     require(disconnected.rows[row].sources==(row==1?(multiSource.rows[row].sources&~2u):multiSource.rows[row].sources),"Disconnect changed unrelated sources");
     require(disconnected.rows[row].sourceTaps==multiSource.rows[row].sourceTaps&&disconnected.rows[row].entryColumn==multiSource.rows[row].entryColumn,"Disconnect moved a tap or entry");
    }
    require(editor->selectedRoutingWire()==std::array<int,2>{-1,-1},"Removed wire retained focus");
    require(!editor->disconnectRoutingWire(0,1,processor->routingRevision(),currentRecall),"Missing connection removed twice");
    capture("disconnected-wire-"+juce::String(size.x)+"x"+juce::String(size.y)+".png",size.x,size.y);
    juce::MemoryBlock afterDisconnect;processor->getStateInformation(afterDisconnect);
    require(processor->setRowRouting(savedRoutes),"Roundtrip setup failed");
    processor->setStateInformation(afterDisconnect.getData(),(int)afterDisconnect.getSize());
    require(processor->getRowRouting().rows[1].sources==disconnected.rows[1].sources,"Disconnected route did not survive state recall");
    require(processor->setRowRouting(savedRoutes),"Final disconnected test restore failed");
    editor->setEditorsVisible(false);
    editor->selectRoutingWireAt(point);editor->selectSlot(0,0);
    require(editor->selectedRoutingWire()==std::array<int,2>{-1,-1},"Slot selection retained wire focus");
   }

   processor->parameters.getParameter("gridMode")->setValueNotifyingHost(0.f);editor->selectSlot(0,0);
   auto* offBlock=dynamic_cast<CortexBlockButton*>(editor->findChildWithID("grid-0-0"));require(offBlock&&offBlock->inactiveCaption()=="GRID OFF"&&!offBlock->blockEnabled&&offBlock->getTooltip().contains("grid engine off"),"Inactive grid block appears active");
   auto* outButton=dynamic_cast<juce::Button*>(editor->findChildWithID("row-route-3"));require(outButton&&!outButton->getToggleState()&&outButton->isEnabled(),"Inactive output state wrong");
   require(processor->getEditableGrid().get(0,0)->enabled&&processor->getRowRouting().rows[3].output,"Visual off state changed saved configuration");
   require(skipped->inactiveCaption()=="GRID OFF","Grid off must override skipped caption");
   capture("custom-grid-disabled-2560x1440.png",2560,1440);capture("custom-grid-disabled-822x600.png",822,600);
   processor->parameters.getParameter("gridMode")->setValueNotifyingHost(1.f);editor->selectSlot(0,0);
   require(skipped->inactiveCaption()=="SKIPPED","Skipped caption did not restore");
   require(offBlock->blockEnabled&&outButton->getToggleState(),"Grid active visuals did not restore");

  }
  for(const auto size:{juce::Point<int>(2560,1440),juce::Point<int>(1280,720)}){
   editor->setSize(size.x,size.y);editor->setEditorsVisible(false);
   editor->selectSlot(0,0);require(editor->insertBlock(2),"Drag source insertion failed");
   editor->selectSlot(3,7);require(editor->insertBlock(0),"Drag target clearing failed");
   auto* target=editor->findChildWithID("grid-3-7");require(target!=nullptr,"Drag target missing");
   require(editor->moveGridSlotToPosition(0,0,target->getBoundsInParent().getCentre().toFloat()),"Scaled drop missed target");
   auto grid=processor->getEditableGrid();require((int)grid.get(3,7)->type==2&&(int)grid.get(0,0)->type==0,"Scaled drop moved to wrong slot");
   auto* source=editor->findChildWithID("grid-0-0");require(editor->moveGridSlotToPosition(3,7,source->getBoundsInParent().getCentre().toFloat()),"Scaled return drop failed");
   require(!editor->moveGridSlotToPosition(0,0,{-10.f,-10.f}),"Outside drop accepted");
  }
  editor.reset();processor->releaseResources();
  std::cout<<"PASS: native editor snapshots at three sizes, control binding, insertion and bypass\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
