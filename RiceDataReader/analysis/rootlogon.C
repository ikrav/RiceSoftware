{
  printf("Loading RICE libraries\n");
  gSystem->AddIncludePath("-I../include/");
  gROOT->ProcessLine(".L ../src/RiceEvent.cc+");
  gROOT->ProcessLine(".L ../src/RiceFileInput.cc+");
}
