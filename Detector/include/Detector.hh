#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "TObjString.h"
#include <vector>

class Detector : public TObject {
public:

  // Types, constants
  struct Channel {
    TObjString name;        // Antenna name
    float x, y, z;          // Antenna coordinates
    float delayCable;       // Cable delay
    float delayAdded;       // Adjustment to account for channel differences in propagation
                            // through DAQ electronics modules
    float gainInIce;        // In-ice amplifier gain
    float gainSurface;      // Surface amplifier gain
    TObjString cableType;   // Type of the coaxial table for this channel
    float freqMin, freqMax; // Bandwidth of the cable
  };

  // Constructor, destructor
  Detector();
  Detector(int year);

  // Accessors
  int     getYear();
  bool    isValidConfiguration();
  int     getNumberOfChannels();
  Channel getChannelConfiguration(int ichannel);

  // Utility
  void printConfiguration();

private:

  int year;
  std::vector<Channel> channels;
  bool isValid;

  ClassDef(Detector,1)
};

ClassImp(Detector)

#endif
