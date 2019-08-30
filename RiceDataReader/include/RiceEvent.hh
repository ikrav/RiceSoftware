#ifndef RICEEVENT_HH
#define RICEEVENT_HH

#include "TString.h"
#include "TGraph.h"
#include "TCanvas.h"
#include <vector>

class RiceEvent : public TObject {

 public:

  // Constructor, destructor
  RiceEvent();
  ~RiceEvent();

  // Constants, types
  enum TriggerMode {TRIG_UNDEFINED=0, TRIG_GENERAL, TRIG_FORCED, TRIG_UNRECOGNIZED}; // approximate list
  static const int _nTriggerModes = 4;

  enum EventType {TYPE_UNDEFINED=0, TYPE_GENERAL, TYPE_UNBIASED, 
		  TYPE_VETO, TYPE_EXTERNAL1, TYPE_EXTERNAL2, TYPE_UNRECOGNIZED};
  static const int _nEventTypes = 7;

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
		     TriggerMode triggerMode, EventType eventType);
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
  // DAQ trigger modes: reflects DAQ settings.
  Timestamp getTimestamp();
  bool isGeneralTrigger();
  bool isForcedTrigger();
  // Event types: how event is classified. Unbiased events are taken when the
  // trigger setting is "forced trigger". Other events are taken with the trigger
  // mode "General" and are classified after having been recorded. They usually could 
  // be of the sort of Genera, Veto, External-triggered events, etc.
  bool isUnbiasedEvent();
  bool isGeneralEvent();
  bool isVetoEvent();
  bool isExternalTriggerEvent();
  EventType getEventType();
  // Waveforms access
  TGraph *getWaveform(int channelIndex);
  int getChannelsPresent();
  int getChannelsMax();
  std::vector<HitInfo> getHits();
  TCanvas *getWaveformsDisplay();

  // Utility
  void printSummary();
  static TString triggerModeNameLookup(TriggerMode mode);
  static TString eventTypeNameLookup  (EventType type);

 private:
  // Data members
  bool _isFilled; // is the event record filled?

  // Header information
  unsigned long int _runNumber;
  unsigned long int _eventNumber;
  TriggerMode       _triggerMode;
  Timestamp         _utcTimestamp;
  EventType         _eventType;
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

  ClassDef(RiceEvent,1)
};

ClassImp(RiceEvent)

#endif
