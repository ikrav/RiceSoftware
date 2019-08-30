{
  printf("Load RICE libraries\n");

  gSystem->AddIncludePath("-I${RICE_SOFTWARE_DIR}/Detector/include");
  TString loadDetector = TString::Format(".L %s/Detector/src/Detector.cc+",
					 gSystem->Getenv("RICE_SOFTWARE_DIR"));
  gROOT->ProcessLine(loadDetector);
  
}
