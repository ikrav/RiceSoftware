
exampleReadRiceRawFormat.C program is an example of using this package to read RICE text
data files one by one and access event content. To run it, start ROOT
in this directory, as test.C relies on the rootlogon automatucally
executed to load all libraries.

exampleRootify.C converts a data file in the raw RICE text format into
a ROOT-format file with a tree containing event objects of the
class RiceEvent. The file also contains a "Detector" object with 
antenna configurations.

exampleReadRiceRootFormat.C is an example of a program that reads
a data file in ROOT format contining a tree with RiceEvent objects,
and accesses event information from those objects. The example
also reads in the detector configuration object.

cleanup.sh is a script that deletes all .so and .d files in the package
created automaticaly by ROOT, and is advisable to run when there are significant 
changes to the code.

rootlogon.C loads all necessary	libraries and relieas on the
environment variable RICE_SOFTWARE_DIR to be set to the	 top
directory of this package.
