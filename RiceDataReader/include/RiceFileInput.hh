#ifndef RICEFILEINPUT_HH
#define RICEFILEINPUT_HH

#include "RiceEvent.hh"
#include <string>
#include <fstream>

class RiceFileInput {

 public:

  // Types, structs, etc
  enum StringPattern {HeaderEndMarker=0, ChannelBeginMarker,
		      EventEndMarker, GeneralSeparator,
		      EventNumberPattern, TriggerModePattern,
		      DateTimePattern, 
		      HitChannelsStartPattern, HitChannelPattern};


  // Constructor, destructor
  RiceFileInput(TString fileName);
  ~RiceFileInput();

  // Get and set methods
  RiceEvent* getNextEvent();
  bool readEventHeader        (RiceEvent *event);
  bool readEventAllChannels   (RiceEvent *event);
  bool readEventSingleChannel (RiceEvent *event);
  bool readEventEndMarker     ();
  bool getNextChannelDataValue(float &var);
  bool getNextHit             (int &channel, float &time);

  // Utility
  bool reachedEndOfFile();
  static TString patternLookup(StringPattern pattern);

 private:

  TString _fileName;
  ifstream _inputFileStream;
  bool     _isEOF;

};

#endif

