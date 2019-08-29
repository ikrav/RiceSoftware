#include "RiceEvent.hh"
#include "RiceFileInput.hh"
#include "TCanvas.h"
#include "TGraph.h"

const int eventIndex = 3; // event to draw on canvas

// Example data files
const TString fileName2000 = "../samples/2000/data-00-128-test_175.dat";
const TString fileName2001 = "../samples/2001/data-11111_example.dat";
const TString fileName2002 = "../samples/2002/data-02070212011-test_72.dat.rice.to_tape_1.gz";
const TString fileName2003 = "../samples/2003/data-20030816100000-volt_1.dat.rice.flt.ready_for_tdrss";
const TString fileName2004 = "../samples/2004/data-20040916100002-volt_1.dat.rice.flt.ready_for_tdrss";
const TString fileName2005 = "../samples/2005/data-20050720100001-volt_1.dat.rice.flt.ready_for_tdrss";
const TString fileName2006 = "../samples/2006/data-20061101100013-volt_1.dat.rice.flt.ready_for_tdrss";
const TString fileName2007 = "../samples/2007/data-2007213004725-test_102.dat";
const TString fileName2008 = "../samples/2008/data-2008234004708-test_18.dat";
const TString fileName2009 = "../samples/2009/data-2009227042005-test_56.dat";
const TString fileName2010 = "../samples/2010/data-2010232043527-test_3.dat";
const TString fileName2011 = "../samples/2011/data-2011236125846-test_74.dat";
const TString fileName2012 = "../samples/2012/data-2012215093431-ub_72.dat";

// File to run on
const TString fileName = fileName2005;

void exampleReadRiceRawFormat(){

  RiceFileInput riceFile(fileName.Data());

  // Loop over events
  int eventsFound = 0;
  while( !riceFile.reachedEndOfFile() ){

    RiceEvent *event = riceFile.getNextEvent();
    if( event == nullptr ){
      printf("ERROR: couldn't parse an event\n");
      break;
    }
    eventsFound++;

    //
    // Print summary of each event    
    //
    event->printSummary();
    
    // Plot waveforms of an event of choice
    
    if( eventIndex == eventsFound ){
      // Note that TGraph objects plotted on the canvas
      // belong to the event object, and disappear from memory - and the canvas -
      // once we move to the next event. The saved PDF should have the figure, of course.
      // TCanvas *wfCanvas = event->getWaveformsDisplay();
      // wfCanvas->Update();
      // wfCanvas->Print("test.pdf");
      // delete wfCanvas;
    }

    // 
    // Event info from the header
    //

    // // Get event and run number
    unsigned long int runNumber, eventNumber;
    event->getRunAndEventNumber(runNumber, eventNumber);
    printf("        retrieved from event: run= %lu   event= %lu\n", runNumber, eventNumber);

    // Get trigger info
    bool isGeneralTrigger = event->isGeneralTrigger();
    bool isForcedTrigger = event->isForcedTrigger();
    if( isGeneralTrigger ){
      printf("        retrieved from event: this is a general trigger event\n");
    }else if( isForcedTrigger ){
      printf("        retrieved from event: this is a forced trigger event\n");
    }else{
      printf("        retrieved from event: neither general nor forced trigger event\n");
    }

    //
    // Get waveforms
    //
    int nWaveforms = event->getChannelsPresent();
    std::vector<TGraph*> wfCollection;
    for(int ichan = 0; ichan < nWaveforms; ichan++){
      TGraph *graph = event->getWaveform(ichan); // TGraph belongs to event, don't delete later
      if( graph != 0 ){
    	wfCollection.push_back(graph);
      }else{
    	// sanity check failed
    	printf("ERROR expected waveform graph is null\n");
      }
    }
    printf("        retrieved from event: %lu waveform graphs\n", wfCollection.size());

    // 
    // Get hits from the event record, as detected by RICE DAQ
    //
    std::vector<RiceEvent::HitInfo> hitList = event->getHits();
    // example of how to look at them
    printf("        retrieved from event: %lu hits\n", hitList.size());
    printf("              ");
    for(uint ihit = 0; ihit < hitList.size(); ihit++){
      RiceEvent::HitInfo thisHit = hitList.at(ihit);
      int chan = thisHit.channel;
      float time = thisHit.time;
      printf("%3d(%.1f)", chan, time);
    }
    printf("\n");

    // Clean up
    delete event;
  } // end of the event loop
  
  printf("Finished running over the data file. Found %d events\n", eventsFound);

}
