#include "Detector.hh"
#include "TString.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

Detector::Detector(){
  year = -999;
  isValid = false;
}

Detector::Detector(int yearIn) {

  year = yearIn;
  isValid = false;

  const TString infoFiles[13] = {
    "antennaloc_2000.dat",
    "antennaloc_2001.dat",
    "antennaloc_2002.dat",
    "antennaloc_2003.dat",
    "antennaloc_2004.dat",
    "antennaloc_2005.dat",
    "antennaloc_2006.dat",
    "antennaloc_2007.dat",
    "antennaloc_2008.dat",
    "antennaloc_2009.dat",
    "antennaloc_2010.dat",
    "antennaloc_2010.dat", // same as 2010
    "antennaloc_2010.dat"  // same as 2010
  };
  TString packagePath = std::getenv("RICE_SOFTWARE_DIR");

  // Year 2000 corresponds to the index zero of the array, etc.
  if( year < 2000 || year > 2012 ){
    printf("Detector constructor ERROR: year %d is not supported!\n", year);
    return;
  } else if( year == 2011 || year == 2012){
    printf("NOTE: for year %d using the configuration of the year 2010\n", year);
  };
  int element  = year - 2000;
  TString infoFullFileName = TString::Format("%s/Detector/GeometryData/%s", 
  					     packagePath.Data(), infoFiles[element].Data());

  // Opoen the data file with the configuration
  ifstream fileStream;
  fileStream.open(infoFullFileName.Data());
  if( ! fileStream.is_open() ){
    printf("Detector::Detector: ERROR, can't open input file %s\n",
           infoFullFileName.Data());
    return;
  }

  // Ignore the first line, then read all available lines in the right format
  std::string line;
  bool keepGoing = true;
  std::getline(fileStream, line); // ignore information from this line
  // loop over channels
  while( keepGoing && ! (fileStream.peek()==EOF) ) {

    if( !std::getline(fileStream, line) ){
      printf("Detector::Detector: ERROR unexpected read failure\n");
      break;
    }
    std::istringstream iss(line);
    std::string channelName, cableType;
    float x,y,z, delayCable, delayAdded, gainInIce, gainSurface, freqMin, freqMax;
    float rms, threshold, dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7;
    if( ( iss >> x >> y >> z
	  >> delayCable >> rms >> delayAdded >> threshold
	  >> dummy1 >> gainInIce >> gainSurface 
	  >> channelName >> cableType >> freqMin >> freqMax
	  >> dummy2 >> dummy3 >> dummy4 >> dummy5 >> dummy6 >> dummy7) ){

      Channel channel;
      channel.name = TObjString( channelName.c_str() );
      channel.x = x;
      channel.y = y;
      channel.z = z;
      channel.delayCable = delayCable;
      channel.delayAdded = delayAdded;
      channel.gainInIce = gainInIce;
      channel.gainSurface = gainSurface;
      channel.cableType = TObjString(cableType.c_str());
      channel.freqMin = freqMin;
      channel.freqMax = freqMax;

      channels.push_back( channel );

    } else {
      // No more lines in the righ format
      keepGoing = false;
      break;
    }
  } // end loop over lines

  // If at least one channel is found, configuration is valid
  if( channels.size() > 0 )
    isValid = true;

  return;
}

int Detector::getYear(){
  return year;
}

int Detector::getNumberOfChannels(){
  return channels.size();
}

Detector::Channel Detector::getChannelConfiguration(int ichannel){

  return channels.at(ichannel);
}

bool Detector::isValidConfiguration(){
  return isValid;
}

void Detector::printConfiguration(){

  printf("\nRICE detector configuration\n");
  printf("Configuration year: %d\n", year);
  printf("chan  ant_name              coordinates (m)            delays (ns)         gains (dB)     frequency (MHz) cable-type\n");
  printf("                        X         Y         Z        cable      DAQ     in-ice   surface    min    max\n");
  
  for(uint i=0; i<channels.size(); i++){
    Channel ch = channels[i]; // for brevity
    printf("%2d  %15s  %9.3f %9.3f %9.3f    %6.0f  %6.0f   %8.4f  %8.4f  %5.0f %5.0f  %10s\n",
	   i, ch.name.String().Data(),
	   ch.x, ch.y, ch.z, 
	   ch.delayCable, ch.delayAdded,
	   ch.gainInIce, ch.gainSurface, 
	   ch.freqMin, ch.freqMax,
	   ch.cableType.String().Data());
  }

}
