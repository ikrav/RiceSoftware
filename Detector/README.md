
This area contains data and code for manipulations with the configuration
of antennas of the RICE array.

GeometryData/ directory has one file for each year that contains data for
all channels in an older RICE format. 

include/ and src/ directories define and implement the Detector class that
reads configuration for a specified year in its constructor from the
data/ area. 

test/ directory contains an example script of how to create and
write a Detector object for a particular year.
