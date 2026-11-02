#pragma once
#include <JuceHeader.h>
#include "DeviceCatalogData.h"
#include <functional>
#include <vector>

// Reference inventory and working originals. Only records with a real local
// implementation can invoke the action; this panel never substitutes an alias.
class DeviceCatalogPanel final : public juce::Component, private juce::ListBoxModel {
public:
 DeviceCatalogPanel(){
  setComponentID("device-catalog-panel");setOpaque(true);
  title.setText("DEVICE CATALOG",juce::dontSendNotification);title.setFont(juce::Font(juce::FontOptions(22.f)));
  counts.setFont(juce::Font(juce::FontOptions(14.f)));
  close.setButtonText("CLOSE");close.setComponentID("catalog-close");close.onClick=[this]{setVisible(false);};
  search.setComponentID("catalog-search");search.setTextToShowWhenEmpty("Search device, original gear, plugin or previous name",juce::Colour(0xff8a8e93));
  search.onTextChange=[this]{rebuild();};
  sections.setComponentID("catalog-section");sections.addItem("All sections",1);
  juce::StringArray names;
  for(const auto& e:vc::deviceCatalog)names.addIfNotAlreadyThere(text(e.section));
  int id=2;for(const auto& name:names)sections.addItem(name,id++);
  sections.setSelectedId(1,juce::dontSendNotification);sections.onChange=[this]{rebuild();};
  availability.setComponentID("catalog-availability");availability.addItem("All entries",1);availability.addItem("Playable originals",2);availability.addItem("Reference only",3);availability.addItem("Announced only",4);
  availability.setSelectedId(1,juce::dontSendNotification);availability.onChange=[this]{rebuild();};
  list.setModel(this);list.setRowHeight(54);list.setColour(juce::ListBox::backgroundColourId,juce::Colour(0xff141414));list.setComponentID("catalog-list");
  details.setComponentID("catalog-details");details.setMultiLine(true);details.setReadOnly(true);details.setScrollbarsShown(true);details.setFont(juce::Font(juce::FontOptions(15.f)));details.setColour(juce::TextEditor::backgroundColourId,juce::Colour(0xff141414));
  action.setComponentID("catalog-action");action.onClick=[this]{const auto* e=selected();if(e&&e->implementationType>0&&onChooseOriginal)onChooseOriginal(e->implementationType);};
  footer.setFont(juce::Font(juce::FontOptions(13.f)));footer.setText("Reference snapshot: "+text(vc::catalogSnapshotDate)+" | neuraldsp.com/device-list | Reference entries contain no playable model or factory capture.",juce::dontSendNotification);
  std::array<juce::Component*,10> controls{&title,&counts,&close,&search,&sections,&availability,&list,&details,&action,&footer};
  for(auto* c:controls)addAndMakeVisible(c);
  rebuild();
 }
 ~DeviceCatalogPanel() override {list.setModel(nullptr);}
 std::function<void(int)> onChooseOriginal;
 int visibleEntryCount() const {return static_cast<int>(visible.size());}
 int selectedPlayableType() const {auto* e=selected();return e?e->implementationType:0;}
 void paint(juce::Graphics& g) override {g.fillAll(juce::Colour(0xff1c1c1c));g.setColour(juce::Colour(0xff61fbd8));g.fillRect(0,0,getWidth(),2);}
 void resized() override {
  auto bounds=getLocalBounds().reduced(16);auto top=bounds.removeFromTop(32);close.setBounds(top.removeFromRight(72));title.setBounds(top);
  counts.setBounds(bounds.removeFromTop(42));auto filters=bounds.removeFromTop(34);
  search.setBounds(filters.removeFromLeft(getWidth()*45/100));filters.removeFromLeft(10);
  availability.setBounds(filters.removeFromRight(getWidth()*20/100));filters.removeFromRight(10);sections.setBounds(filters);
  bounds.removeFromTop(14);footer.setBounds(bounds.removeFromBottom(34));bounds.removeFromBottom(8);
  auto side=bounds.removeFromRight(getWidth()*37/100);bounds.removeFromRight(14);list.setBounds(bounds);
  action.setBounds(side.removeFromBottom(42));side.removeFromBottom(14);details.setBounds(side);
 }
private:
 juce::Label title,counts,footer;
 juce::TextButton close,action;
 juce::TextEditor search,details;
 juce::ComboBox sections,availability;
 juce::ListBox list;
 std::vector<const vc::CatalogRecord*> visible;
 static juce::String text(const char* value){return juce::String::fromUTF8(value);}
 const vc::CatalogRecord* selected() const {const int i=list.getSelectedRow();return i>=0&&i<(int)visible.size()?visible[(size_t)i]:nullptr;}
 int getNumRows() override {return visibleEntryCount();}
 void paintListBoxItem(int row,juce::Graphics& g,int width,int height,bool selectedRow) override {
  if(row<0||row>=(int)visible.size())return;const auto& e=*visible[(size_t)row];
  g.fillAll(juce::Colour(selectedRow?0xff28483f:0xff141414));
  g.setColour(juce::Colour(e.implementationType?0xff61fbd8:0xffd9d9d9));g.setFont(juce::Font(juce::FontOptions(17.f)));
  g.drawFittedText(text(e.name),10,3,width-190,24,juce::Justification::centredLeft,1);
  g.setFont(juce::Font(juce::FontOptions(12.f)));g.setColour(juce::Colour(0xffa5a5a5));
  g.drawFittedText(e.implementationType?"PLAYABLE ORIGINAL":e.announced?"ANNOUNCED / REFERENCE":"REFERENCE ONLY",width-180,5,170,20,juce::Justification::centredRight,1);
  juce::String subtitle=text(e.category);
  if(text(e.plugin).isNotEmpty())subtitle+=" | "+text(e.plugin);
  else if(text(e.basedOn).isNotEmpty())subtitle+=" | "+text(e.basedOn);
  g.drawFittedText(subtitle,10,28,width-20,height-28,juce::Justification::centredLeft,1);
 }
 void selectedRowsChanged(int) override {refreshDetails();}
 void refreshDetails(){
  const auto* e=selected();action.setEnabled(e&&e->implementationType>0);
  if(!e){details.setText("No matching entries.",false);action.setButtonText("NO MATCHES");return;}
  juce::String info=text(e->name)+"\n\n";
  info+=e->implementationType?"PLAYABLE ORIGINAL\nOriginal Virtual Cortex processor.\n\n":"REFERENCE ONLY\nNo playable model is included.\n\n";
  info+="Section: "+text(e->section)+"\nCategory: "+text(e->category)+"\n";
  const std::array<std::pair<const char*,const char*>,6> fields{{{"Based on",e->basedOn},{"Added in CorOS",e->added},{"Previous name",e->previous},{"Updated in CorOS",e->updated},{"Replaces",e->replaces},{"Required plugin",e->plugin}}};
  for(const auto& f:fields)if(text(f.second).isNotEmpty())info+=text(f.first)+": "+text(f.second)+"\n";
  if(e->announced)info+="\nSource status: announced, not yet released.\n";
  if(e->implementationType>=23)info+="\nPlacement: global post-chain processor.\n";
  else if(e->implementationType>0)info+="\nAction adds this original processor to the selected grid slot. Enable GRID ENGINE to hear grid processing.\n";
  details.setText(info,false);details.setCaretPosition(0);
  action.setButtonText(e->implementationType>=23?"OPEN POST-CHAIN CONTROLS":e->implementationType>0?"ADD ORIGINAL TO SELECTED SLOT":"MODEL NOT AVAILABLE");
 }
 void rebuild(){
  const auto query=search.getText().trim();const auto section=sections.getSelectedId()>1?sections.getText():juce::String();const int filter=availability.getSelectedId();
  visible.clear();int originals=0,references=0,announced=0;
  for(const auto& e:vc::deviceCatalog){
   if(e.implementationType>0)++originals;else if(e.announced)++announced;else ++references;
   if(section.isNotEmpty()&&section!=text(e.section))continue;
   if((filter==2&&e.implementationType==0)||(filter==3&&(e.implementationType>0||e.announced))||(filter==4&&!e.announced))continue;
   const juce::String searchable=text(e.name)+" "+text(e.category)+" "+text(e.basedOn)+" "+text(e.previous)+" "+text(e.plugin)+" "+text(e.section);
   if(query.isNotEmpty()&&!searchable.containsIgnoreCase(query))continue;
   visible.push_back(&e);
  }
  list.deselectAllRows();list.updateContent();if(!visible.empty())list.selectRow(0);refreshDetails();
  counts.setText(juce::String(originals)+" playable original processors | "+juce::String(references)+" released reference entries | "+juce::String(announced)+" announced reference entries | Showing "+juce::String(visible.size()),juce::dontSendNotification);
 }
};
