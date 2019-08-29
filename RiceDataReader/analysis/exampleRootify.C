#include "RiceEvent.hh"
#include "RiceFileInput.hh"

#include "TFile.h"
#include "TTree.h"

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

// File to run on and save
const TString fileNameIn = fileName2005;
const TString fileNameOut = "test.root";

void exampleRootify(){

  // Open a raw RICE data file in text format
  RiceFileInput riceFile(fileNameIn.Data());

  // Open a ROOT file where event objects will be written, configure output tree, etc
  TFile fout(fileNameOut.Data(), "recreate");
  fout.cd();
  TTree *treeOut = new TTree("eventTree","RICE events tree");
  
  RiceEvent *eventOut = 0;
  treeOut->Branch("event", &eventOut);

  // Loop over events
  int eventsFound = 0;
  while( !riceFile.reachedEndOfFile() ){

    eventOut = riceFile.getNextEvent();
    if( eventOut == nullptr ){
      printf("ERROR: couldn't parse an event\n");
      break;
    }
    eventsFound++;
    
    treeOut->Fill();

  } // end of the event loop
  
  printf("Finished running over the data file. Found %d events\n", eventsFound);

  treeOut->Write();
  fout.Close();
  printf("Data are saved in %s\n", fileNameOut.Data());
}
