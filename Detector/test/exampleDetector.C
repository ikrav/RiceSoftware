#include "Detector.hh"
#include "TFile.h"

const int year = 2005; // only years 2000-2012 are available

void exampleDetector(){

  // Create a Detector object
  Detector detector(year);

  // Print all available info
  detector.printConfiguration();

  // Access some of the fields for a particular channel/antenna
  int iChannel = 5;
  if( iChannel < detector.getNumberOfChannels() ){
    Detector::Channel channel = detector.getChannelConfiguration(iChannel);
    // See all available fields in the Detector.hh file
    printf("\nChannel %2d is %s and is found at (%f, %f, %f)\n",
	   iChannel, channel.name.String().Data(), channel.x, channel.y, channel.z);
  }

  // Example of saving the detector object in a ROOT file
  TFile fout("test.root", "recreate");
  detector.Write("detector");
  fout.Close();

}
