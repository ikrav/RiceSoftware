#include "RiceFileInput.hh"
#include <sstream>
#include <string>
#include <cstdlib>

RiceFileInput::RiceFileInput(TString fileName){

  // Open the input file
  _fileName = fileName;
  _inputFileStream.open(_fileName.Data());
  _isEOF    = false;
  if( ! _inputFileStream.is_open() ){
    printf("RiceFileInput::RiceFileInput: ERROR, can't open input file %s\n",
	   _fileName.Data());
  }
}

RiceFileInput::~RiceFileInput(){

  if( _inputFileStream.is_open() )
    _inputFileStream.close();
}

RiceEvent* RiceFileInput::getNextEvent(){

  // First check if we are already at the end
  if( reachedEndOfFile() ){
    return nullptr;
  }

  RiceEvent * event = new RiceEvent();
  if( !readEventHeader(event) ){
    printf("RiceFileInput::getNextEvent: ERROR failed to parse event header\n");
    delete event;
    return nullptr;
  }

  if( !readEventAllChannels(event) ){
    printf("RiceFileInput::getNextEvent: ERROR failed to parse channels data\n");
    delete event;
    return nullptr;
  }

  // Finally, read end of event marker
  if( !readEventEndMarker() ){
    printf("RiceFileInput::getNextEvent: ERROR no expected event end marker\n");
    delete event;
    return nullptr;
  }
  event->setFilled(true);
  
  // Check if we reached end of file
  if( _inputFileStream.peek()==EOF ){
    _isEOF = true;
  }

  return event;
}

bool RiceFileInput::readEventHeader(RiceEvent *event){

  // Is the file ok?
  bool result = false;
  if( !_inputFileStream.is_open() ){
    printf("RiceFileInput::readEventHeader: ERROR input file not available\n");
    return result;
  }

  unsigned long int eventNumber = -1;
  unsigned long int runNumber = -1;
  RiceEvent::TriggerMode triggerMode = RiceEvent::TRIG_UNDEFINED;
  RiceEvent::EventType   eventType   = RiceEvent::TYPE_UNDEFINED;
  bool isYear2000 = false;

  //
  //  PARSE EVENT HEADER INFORMATION 
  //
  // Read the event header line by line and extract desired information
  bool keepReading = true;
  std::string line;
  while( keepReading) {
    if( ! std::getline(_inputFileStream, line)  ) {
      printf("RiceFileInput::readEventHeader: ERROR unexpected read termination\n");
      return result;
    }

    // Is this the line with the event number?
    if( line.find(patternLookup(EventNumberPattern)) != std::string::npos ){
      std::string dummy1, dummy2;
      std::istringstream iss(line);
      if( !(iss >> dummy1 >> dummy2 >> eventNumber ) ){
	printf("RiceFileInput::readEventHeader: ERROR getting event number\n");
	return result;
      }
      
      // Is this the line with the trigger mode info?
    }else if( line.find(patternLookup(TriggerModePattern)) != std::string::npos ){   
      // Determine which of the possible trigger modes it is
      bool matchFound = false;
      for(int iMode = 0; 
	  iMode < RiceEvent::_nTriggerModes; iMode++){
	RiceEvent::TriggerMode mode = static_cast<RiceEvent::TriggerMode>(iMode);
	if( line.find( RiceEvent::triggerModeNameLookup( mode ) ) != std::string::npos ){
	  triggerMode = mode;
	  matchFound = true;
	  break; // break out of the loop over modes
	}
      } // end loop over modes
      // If we tried all names but found no match
      if( !matchFound)
	triggerMode = RiceEvent::TRIG_UNRECOGNIZED;

      // Is this the line with the event classification info?
    }else if( line.find(patternLookup(EventTypePattern)) != std::string::npos ){   
      // Determine which of the possible event types it is
      bool matchFound = false;
      for(int iType = 0; 
	  iType < RiceEvent::_nEventTypes; iType++){
	RiceEvent::EventType type = static_cast<RiceEvent::EventType>(iType);
	if( line.find( RiceEvent::eventTypeNameLookup( type ) ) != std::string::npos ){
	  eventType = type;
	  matchFound = true;
	  break; // break out of the loop over modes
	}
      } // end loop over types
      // If we tried all names but found no match
      if( !matchFound)
	eventType = RiceEvent::TYPE_UNRECOGNIZED;

      // Is this the line with the UTC date/time?
    }else if( line.find(patternLookup(DateTimePattern)) != std::string::npos ){   
      std::string dummy1, dummy2, dummy3, dummy4, dummy5, dummy6;
      std::string timeString;
      int year, day, hour, minute, second;
      float microsecond;
      // Parse the line
      std::istringstream iss(line);
      // First, get all info except for the microsecons counter
      if( !(iss >> dummy1 >> year >> dummy2 >> day 
	    >> dummy3 >> dummy4 >> timeString ) ){
	printf("RiceFileInput::readEventHeader: ERROR getting event date and time\n");
	return result;
      }
      // In year 2000, the year was saved as "00", so adjust that
      if( year == 0 ){
	year = 2000;
	isYear2000 = true;
      }
      // Extract time from hours:minutes:seconds format
      std::string field;
      std::vector <std::string> fields;
      std::istringstream timeIss(timeString);
      while(getline(timeIss, field, ':'))
	fields.push_back(field);
      if( fields.size() != 3 ){
	printf("RiceFileInput::readEventHeader: ERROR parsing the time string\n");
	return result;
      }
      hour   = std::atoi(fields[0].c_str());
      minute = std::atoi(fields[1].c_str());
      second = std::atoi(fields[2].c_str());

      // Second, get the microseconds field. Note that in the year 2000, the
      // field was not available. Thus we do the streaming, but do not check 
      // whether it succeeded. If it fails, the variable will be zero.
      microsecond = 0;
      if( !isYear2000 ){
	if( !(iss >> dummy5 >> dummy6 >> microsecond) ){
	  printf("RiceFileInput::readEventHeader: ERROR parsing the microseconds value\n");
	  return result;
	}
      }

      // Store the date and time
      event->setTimestamp(year, day, hour, minute, second, microsecond);

      // Is this the line where the hits list starts?
    }else if( line.find(patternLookup(HitChannelsStartPattern)) != std::string::npos ){   
      int channel = -1;
      float time = 0;
      // This loop will read next lines one by one and will extract
      // the channel number and time of the hit from the lines
      while( getNextHit(channel, time) ){
	event->addHit(channel, time);
      }

    } else if( line.find(patternLookup(HeaderEndMarker)) != std::string::npos ){
      // End of the event header is reached, return
      keepReading = false;
    }
  } // end while loop reading the event header
  //
  //  END PARSING EVENT HEADER 
  //

  // Get the run number from the file name. The file name is assumed to be
  // of the type <prefix>-<run number>-<other stuff>, so get the second field from the name
  // with delimiter being "-".
  std::istringstream fnameStream(_fileName.Data());
  std::string prefix = "20";
  std::string str1 = "";
  std::string str2 = "";
  std::getline(fnameStream, str1, '-'); //dummy call
  std::getline(fnameStream, str1, '-'); // for years 2001-2012 this yields full run number
  std::getline(fnameStream, str2, '-'); // for year 2000, this has extra part of run number
  std::string runNumberString = "";
  if( !isYear2000 ){
    runNumberString = str1;
  }else{
    printf("here %s  %s  %s\n", prefix.c_str(), str1.c_str(), str2.c_str());
    runNumberString = prefix + str1 + str2;
  }
  // now str should contain the run number, convert it to a number
  std::istringstream runNumberStream(runNumberString);
  if( !( runNumberStream >> runNumber ) ){
    printf("RiceFileInput::readEventHeader: ERROR getting run number\n"); 
    return result;
  }

  // Push the information into the event structure
  event->setHeaderInfo(runNumber, eventNumber,
		       triggerMode, eventType);

  result = true;
  return result;
}


bool RiceFileInput::readEventAllChannels(RiceEvent *event){

  bool result = false;
  if( !_inputFileStream.is_open() ){
    printf("RiceFileInput::readEventAllChannels: ERROR input file not available\n");
    return result;
  }

  while( event->getChannelsPresent() < event->getChannelsMax() )
    {
      if( ! readEventSingleChannel(event) )
	break;
    }

  // Did we find at least one channel in this event record?
  result = false;
  if( event->getChannelsPresent() >0 )
    result = true;

  return result;
}

bool RiceFileInput::readEventSingleChannel(RiceEvent *event){

  bool result = false;
  if( !_inputFileStream.is_open() ){
    printf("RiceFileInput::readSingleChannel: ERROR input file not available\n");
    return result;
  }
  
  // Read the marker that indicates beginning of the channel's record
  std::string line;
  std::streampos position = _inputFileStream.tellg();
  if( ! std::getline(_inputFileStream, line)  )
    return result;

  // Check if we are at the end of the event and no more channels are there
  if( line.find(patternLookup(EventEndMarker)) != std::string::npos ){
    // If this is the event end marker, rewind where we are in reading
    // the file and exit
    _inputFileStream.seekg(position);
    return result;
  }      

  // Check if the first line we just read matches the expected channel-begin
  if( line.find(patternLookup(ChannelBeginMarker)) == std::string::npos ){
    printf("RiceFileInput::readEventSingleChannel: ERROR unexpected line at the");
    printf("   beginning of a channel record\n");
    return result;
  }

  // Read the first for values: T0, dT, Yoff, Yres
  float T0, dT, Yoff, Yres;
  if( !( getNextChannelDataValue(T0) &&
	 getNextChannelDataValue(dT) &&
	 getNextChannelDataValue(Yoff) &&
	 getNextChannelDataValue(Yres) ) ) 
    {
      printf("RiceFileInput::readEventSingleChannel: ERROR Failed to read T0/dT/Yoff/Yress in");
      printf("the channel's data\n");
      return result;
    }
  // Read the waveform points
  std::vector<float> vSamples;
  float vOneSample = -999;
  while( getNextChannelDataValue(vOneSample) ){
    vSamples.push_back(vOneSample);
  }
  if( vSamples.size() == 0 ){
    printf("RiceFileInput::readEventSingleChannel: ERROR waveform has no data points\n");
    return result;
  }

  // Add this info to the event data structure and increment the present channels counter
  int thisChannelIndex = event->getChannelsPresent(); // index starts from zero
  event->setChannel(thisChannelIndex, T0, dT, Yoff, Yres, vSamples);
  event->setNChannelsPresent( thisChannelIndex + 1 );
  result = true;

  return result;
}

bool RiceFileInput::getNextChannelDataValue(float &var){

  bool success = false;
  std::string line;
  std::streampos position = _inputFileStream.tellg();
  // Check if we are at the end of the file
  if( ! std::getline(_inputFileStream, line)  )
    return success;
  
  // Check if we are at the end of the event or at the beginning
  // of the new channel
  if( line.find(patternLookup(GeneralSeparator)) != std::string::npos ){
    //if we are, rewind back and return false
    _inputFileStream.seekg(position);
    return success;
  }      

  std::istringstream iss(line);
  if( !(iss >> var ) ){
    // Somehow couldn't read a number, return false
    printf("RiceFileInput::getNextChannelDataValue: ERROR getting next WF sample\n");
    return success;
  }
   
  success = true;
  return success;

}

bool RiceFileInput::readEventEndMarker(){

  bool success = false;
  std::string line;
  // Check if we are at the end of the file
  if( ! std::getline(_inputFileStream, line)  )
    return success;
  
  // Check if we are at the end of the event or at the beginning
  // of the new channel
  if( line.find(patternLookup(EventEndMarker)) != std::string::npos ){
    success = true;
  }      

  return success;
}

bool RiceFileInput::reachedEndOfFile(){
  return _isEOF;
}

TString RiceFileInput::patternLookup(StringPattern pattern){

  TString result = "blank";
  
  if(      pattern == HeaderEndMarker    ) result = "------------------------------";
  else if( pattern == ChannelBeginMarker ) result = "---------------Waveform";
  else if( pattern == EventEndMarker     ) result = "------------------------------";
  else if( pattern == GeneralSeparator   ) result = "---------------";
  else if( pattern == EventNumberPattern ) result = "Event number:";
  else if( pattern == TriggerModePattern ) result = "Trigger mode:";
  else if( pattern == DateTimePattern    ) result = "Year";
  else if( pattern == HitChannelsStartPattern ) result = "Full hit list:";
  else if( pattern == HitChannelPattern  ) result = "Channel";
  else if( pattern == EventTypePattern   ) result = "Event is classified as ";
  else {
    printf("RiceFileInput::patternLookup: ERROR undefined pattern\n");
  }

  return result;
}

bool RiceFileInput::getNextHit(int &channel, float &time){

  bool result = false;

  // Save the current position
  std::streampos position = _inputFileStream.tellg();
  
  // Get the next line and analyze it
  std::string line;
  if( ! std::getline(_inputFileStream, line)  ) 
    return result;

  if( line.find(patternLookup(HitChannelPattern)) != std::string::npos ){   
    std::string dummy1, dummy2;
    std::istringstream iss(line);
    if( (iss >> dummy1 >> channel >> dummy2 >> time ) ){
      result = true;
    }    
  }

  if( !result ){
    // This line does not contain hit channel info, rewind file position before returning
    _inputFileStream.seekg(position);
  }

  return result;
}
