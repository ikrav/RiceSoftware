#include "RiceEvent.hh"
#include "RiceFileInput.hh"

#include "TFile.h"
#include "TTree.h"

const TString fileNameIn = "test.root";

const int eventToDraw = 3;

void exampleReadRiceRootFormat(){

  // Open file with RICE data in ROOT format
  TFile *fin = new TFile(fileNameIn.Data());
  TTree *tree = (TTree*)fin->Get("eventTree");
  if( tree == 0 ){
    printf("Failed to find the eventTree\n");
    return;
  }

  // Configure the tree
  RiceEvent *event = 0;
  tree->SetBranchAddress("event",&event);

  // Loop over events
  for(int iev = 0; iev < tree->GetEntries(); iev++){

    tree->GetEntry(iev);
    if( !event ){
      break;
    }

    event->printSummary();

    // Draw all waveforms for one event
    if( iev == eventToDraw ){
      // Note that TGraph objects plotted on the canvas
      // belong to the event object, and disappear from memory - and the canvas -
      // once we move to the next event. The saved PDF should have the figure, of course.
      // TCanvas *wfCanvas = event->getWaveformsDisplay();
      // wfCanvas->Update();
      // wfCanvas->Print("test.pdf");
      // delete wfCanvas;
    }

    // See RiceEvent header file for the list of available accessors

  }

  fin->Close();

  return;
}
