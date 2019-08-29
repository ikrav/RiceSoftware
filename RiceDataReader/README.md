
This packge provides software for analysis of RICE data files in raw (text)
format. These data are found in the dat warehouse in Wisconsin in
   /data/exp/RICE/<year>/

The package structure:
  include/ and src/ provide the tools: RiceEvent is a class for a data structure
  into which an event is read, 
  RiceFileInput is a class that handles reading a RICE text data file event
  by event and creates a RiceEvent structure for the event.

  analysis/ is a directory for analysis scripts, with examples including
  reading raw text and repackaged ROOT format data files, etc. See README
  in that directory.
     The rootlogon.C in that directory loads the RICE tools.

  samples/ directory has a sample data file for each year of RICE datataking
  for testing purposes.

Installation:
  Just check this package out from GitHub, go to the analysis/ directory and
  run user code, e.g.
    root -b -q exampleReadRiceRawFormat.C+
  or
    root -l
    .L exampleReadRiceRawFormat.C+
    exampleReadRiceRawFormat()
 
  Obviously, ROOT needs to be set up. The code is tested for ROOT v5.34 and should
  work at least for that and later, probably for earlier versions too.

How raw RICE data access works (see analysis/exampleReadRiceRawFormat.C for more details):
create file input object and read events one by one, e.g. (ROOT command line
or a file with a script or C++ code):
   RiceFileInput riceFile("..... file name ...");
   RiceEvent *event = riceFile.getNextEvent();
   if(event != nullptr )
        event->getSomethingINeed();
and repeat getNextEvent in the loop until the end of the file.
What "get" methods are available, see in the include/RiceEvent.hh.
   Access to ROOT data format can be seen in the analysis/exampleReadRiceRootFormat.C.

