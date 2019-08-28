#ifndef RICEEVENT_HH
#define RICEEVENT_HH

#include "TString.h"
#include "TGraph.h"
#include "TCanvas.h"
#include <vector>

class RiceEvent {

 public:

  // Constructor, destructor
  RiceEvent();
  ~RiceEvent();

  // Constants, types
  enum TriggerMode {UNDEFINED=0, GENERAL, FORCED, UNRECOGNIZED}; // approximate list
  static const int _nTriggerModes = 4;

  struct Timestamp {
    int year;
    int day; // from 1 to 365/366
    int hour;
    int minute;
    int second;
    float microsecond;
    //    
  Timestamp(): year(0), day(0), hour(0), minute(0), second(0), microsecond(0){};
  };

  struct HitInfo {
    int channel;
    float time;
    //
    HitInfo(): channel(0), time(0){};
    HitInfo(int chanIn, float timeIn): channel(chanIn), time(timeIn){};
  };
  
  // Set methods  
  void setHeaderInfo(unsigned long int runNumber, unsigned long int eventNumber,
		     TriggerMode triggerMode);
  void setChannel(int channelIndex, float T0, float dT, float Voffset, float Vres,
		  std::vector<float> voltageCounts);
  void setNChannelsPresent(int nChannelsPresent);
  void setTimestamp(int yearIn, int dayIn, int hourIn, int minuteIn, int secondIn, 
		    float microsecondIn);
  void addHit(int channel, float time);
  void setFilled(bool status);

  // Get methods
  bool isFilled();
  void getRunAndEventNumber(unsigned long int &runNumber, unsigned long int &eventNumber);
  bool isGeneralTrigger();
  bool isForcedTrigger();
  TGraph *getWaveform(int channelIndex);
  int getChannelsPresent();
  int getChannelsMax();
  std::vector<HitInfo> getHits();
  TCanvas *getWaveformsDisplay();

  // Utility
  void printSummary();
  static TString triggerModeNameLookup(TriggerMode mode);

 private:
  // Data members
  bool _isFilled; // is the event record filled?

  // Header information
  unsigned long int _runNumber;
  unsigned long int _eventNumber;
  TriggerMode       _triggerMode;
  Timestamp         _utcTimestamp;
  std::vector<HitInfo> _hits;

  static const int _nChannelsMax = 24; // Maximum physically possible number of channels

  // Waveforms
  int   _nChannelsPresent; // Number of channels found in the event record
  // The waveform is constructed from the data points from a RICE txt data file as:
  // time[iSample] = _T0 + _dT * iSample;
  // voltage[iSample] = _Voffset + _Vres * (count-read-from-file)
  float _T0[_nChannelsMax]; // Start of the waveform time , seconds
  float _dT[_nChannelsMax]; // Time between consequtive samples in the waveform, seconds
  float _Voffset[_nChannelsMax]; // The voltage offset from zero of the waveform points, volts
  float _Vres[_nChannelsMax]; // Voltage per unit count in original RICE files, volts
  
  TGraph *_waveformGraphs[_nChannelsMax];

};

#endif
