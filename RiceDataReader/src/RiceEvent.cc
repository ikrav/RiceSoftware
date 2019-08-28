#include "RiceEvent.hh"


RiceEvent::RiceEvent(){

  _isFilled = false;
  _runNumber = -1;
  _eventNumber = -1;
  _triggerMode = UNDEFINED;
  
  _nChannelsPresent = 0;
  for(int iChannel=0; iChannel < _nChannelsMax; iChannel++){
    _T0[iChannel] = 0;
    _dT[iChannel] = 0;
    _Voffset[iChannel] = 0;
    _Vres[iChannel] = 0;
    _waveformGraphs[iChannel] = nullptr;
  }

}

RiceEvent::~RiceEvent(){

  if( isFilled() )
    for(int iChannel=0; iChannel < _nChannelsMax; iChannel++){    
      if( _waveformGraphs[iChannel] != nullptr )
	delete _waveformGraphs[iChannel];
    }

}

void RiceEvent::setHeaderInfo(unsigned long int runNumber, unsigned long int eventNumber,
			      TriggerMode triggerMode){
  _runNumber = runNumber;
  _eventNumber = eventNumber;
  _triggerMode = triggerMode;
}

void RiceEvent::setChannel(int channelIndex, float T0, float dT, float Voffset, float Vres,
		std::vector<float> voltageCounts)
{

  if( channelIndex >= _nChannelsMax ){
    printf("RiceEvent::Error: invalid channel index in setChannel call\n");
    return;
  }

  _T0     [channelIndex] = T0;
  _dT     [channelIndex] = dT;
  _Voffset[channelIndex] = Voffset;
  _Vres   [channelIndex] = Vres;

  // Construct the waveform graph
  _waveformGraphs[channelIndex] = new TGraph(0);
  for(uint iSample = 0; iSample < voltageCounts.size(); iSample++){
    float tSample = T0 + dT * iSample;
    float vSample = Voffset + Vres * voltageCounts.at(iSample);
    _waveformGraphs[channelIndex]->SetPoint( _waveformGraphs[channelIndex]->GetN(),
					     tSample, vSample);
  }
  
}

void RiceEvent::setNChannelsPresent(int nChannelsPresent)
{
  _nChannelsPresent = nChannelsPresent;
}

void RiceEvent::setTimestamp(int yearIn, int dayIn, int hourIn, int minuteIn, int secondIn, 
			     float microsecondIn){

  _utcTimestamp.year        = yearIn;
  _utcTimestamp.day         = dayIn;
  _utcTimestamp.hour        = hourIn;
  _utcTimestamp.minute      = minuteIn;
  _utcTimestamp.second      = secondIn;
  _utcTimestamp.microsecond = microsecondIn;
};

void RiceEvent::addHit(int channel, float time){
  HitInfo thisHit(channel, time);
  _hits.push_back(thisHit);
}

void RiceEvent::setFilled(bool status)
{
  _isFilled = status;
}

bool RiceEvent::isFilled(){
  return _isFilled;
}

TGraph *RiceEvent::getWaveform(int channelIndex){

  TGraph *grOut = nullptr;
  if( !isFilled() ){
    printf("RiceEvent::getWaveform ERROR: event is not filled\n");
    return grOut;
  }else if( channelIndex >= _nChannelsMax ){
    printf("RiceEvent::getWaveform ERROR: too large channel index requested\n");
  }else if( _waveformGraphs[channelIndex] == nullptr ){
    printf("RiceEvent::getWaveform ERROR: Waveform from an invalid channel is requested, returning null\n");
    return grOut;
  }

  grOut = _waveformGraphs[channelIndex];
  return grOut;
}

std::vector<RiceEvent::HitInfo> RiceEvent::getHits(){
  return _hits;
}

void RiceEvent::printSummary()
{

  printf("\nRiceEvent record summary:\n");
  printf("   run number %lu     event number %lu\n", _runNumber, _eventNumber);
  printf("   year %d day %d time %02d:%02d:%02d mksec %.1f\n", 
	 _utcTimestamp.year, _utcTimestamp.day, 
	 _utcTimestamp.hour, _utcTimestamp.minute, _utcTimestamp.second,
	 _utcTimestamp.microsecond);
  printf("   trigger: %s\n", triggerModeNameLookup( _triggerMode ).Data() );
  printf("   number of waveforms: %d\n", _nChannelsPresent);
  printf("   hit channels:\n");
  for(uint ichan = 0; ichan < _hits.size(); ichan++){
    printf("       channel %3d      daq hit time %10.1f\n",
	   _hits.at(ichan).channel, _hits.at(ichan).time);
  }
}

void RiceEvent::getRunAndEventNumber(unsigned long int &run, unsigned long int &event){
  run = _runNumber;
  event = _eventNumber;
}

bool RiceEvent::isGeneralTrigger(){
  
  if( _triggerMode == GENERAL )
    return true;
  
  return false;
}

bool RiceEvent::isForcedTrigger(){
  
  if( _triggerMode == FORCED )
    return true;
  
  return false;
}

TString RiceEvent::triggerModeNameLookup(TriggerMode mode){

  // The strings below match exactly what is recorded as the trigger mode
  // in RICE data files
  TString result = "blank";
  if( mode == UNDEFINED ) result = "Undefined";
  else if( mode == GENERAL   ) result = "General";
  else if( mode == FORCED    ) result = "FORCED TRIGGER";
  else                         result = "Unrecognized";

  return result;
}

int RiceEvent::getChannelsPresent(){
  return _nChannelsPresent;
}

int RiceEvent::getChannelsMax(){
  return _nChannelsMax;
}

TCanvas *RiceEvent::getWaveformsDisplay(){

  if( !isFilled() ){
    printf("RiceEvent::getWaveformsDisplay: ERROR cannot plot waveforms, event is not filled\n");
    return nullptr;
  }

  TCanvas *canvas = new TCanvas("c1", "c1", 10,10,800,800);
  int ncols = 4;
  int nrows = getChannelsPresent() / ncols;
  canvas->Divide(ncols, nrows);
  for(int i=0; i< getChannelsPresent(); i++ ){
    canvas->cd( i+1 );
    if( _waveformGraphs[ i ] != nullptr ){
      TGraph *graph = (TGraph*) _waveformGraphs[ i ]->Clone();
      graph->Draw("AL");
      // _waveformGraphs[ i ]->Draw("AL");
    }
  }

  return canvas;
}
