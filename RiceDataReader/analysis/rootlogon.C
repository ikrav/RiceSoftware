{
  printf("Loading RICE libraries\n");

  gSystem->AddIncludePath("-I${RICE_SOFTWARE_DIR}/Detector/include");
  gSystem->AddIncludePath("-I${RICE_SOFTWARE_DIR}/RiceDataReader/include");

  TString riceDir = gSystem->Getenv("RICE_SOFTWARE_DIR");
  TString loadCmd;

  loadCmd = TString::Format(".L %s/Detector/src/Detector.cc+",riceDir.Data());
  gROOT->ProcessLine(loadCmd);

  loadCmd = TString::Format(".L %s/RiceDataReader/src/RiceEvent.cc+",riceDir.Data());
  gROOT->ProcessLine(loadCmd);

  loadCmd = TString::Format(".L %s/RiceDataReader/src/RiceFileInput.cc+",riceDir.Data());
  gROOT->ProcessLine(loadCmd);

}
