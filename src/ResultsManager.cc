#include "ResultsManager.hh"

ResultsManager::ResultsManager(InputManager *aInMgr) {

  InMgr = aInMgr;

  std::string filename;
  InMgr->GetVariable("Events",filename);
  f = new TFile(filename.c_str());

}

ResultsManager::~ResultsManager() {

}

void ResultsManager::CreateTable() {

  std::string tablename;
  InMgr->GetVariable("Table",tablename);

  std::ofstream table;
  table.open(tablename.c_str());

  int beamA, beamZ, targetA, targetZ, pna, colSize;
  double ObsPoint_E, eloss, z_origin, targetPres, targetTemp;

  InMgr->GetVariable("beamA", beamA);
  InMgr->GetVariable("beamZ", beamZ);
  InMgr->GetVariable("targetA", targetA);
  InMgr->GetVariable("targetZ", targetZ);
  InMgr->GetVariable("mcFactor", pna);
  InMgr->GetVariable("colsize", colSize);
  InMgr->GetVariable("beamE",ObsPoint_E);
  InMgr->GetVariable("eloss", eloss);
  InMgr->GetVariable("z_origin", z_origin);
  ObsPoint_E = ObsPoint_E - ((z_origin-7.125)/37)*eloss;
  InMgr->GetVariable("targetP", targetPres);
  targetTemp = 21.0; //default

  std::string header0 = "beamA	beamZ	targetA	targetZ	\n";
  std::cout << header0;
  table << header0;

  std::cout << beamA << "	" << beamZ << "	" << targetA << "	" << targetZ << "\n \n";
  table << beamA << "	" << beamZ << "	" << targetA << "	" << targetZ << "\n \n";

  std::string header1 = "ObsPoint_E	pnA*1s	targetPres	targetTemp	colSize(mm)	\n";
  std::cout << header1;
  table << header1;

  std::cout << ObsPoint_E << "	" << pna << "	" << targetPres << "	" << targetTemp << "	" << colSize << "\n \n";
  table << ObsPoint_E << "	" << pna << "	" << targetPres << "	" << targetTemp << "	" << colSize << "\n \n";

  std::string header2 = "Angle	Total	Error	Recoils	Error	Ejectiles	Error	\n";
  std::cout << header2;
  table << header2;

  const char* DetAngles[13] = {"22_5","25","30","35","40","45","55","60","65","75","90","120","135"};
  int Tot, errTot, Rcl, errRcl, Ejc, errEjc;
  TTree *t;

  for(int i=0; i<13; i++) {

    string totStr = "Total_Detector/angle_";
    totStr.append(DetAngles[i]);
    t = (TTree*)f->Get(totStr.c_str());
    Tot = t->GetEntries();
    errTot = (int) Tot / sqrt(Tot);
    if (Tot<=0) {Tot=0, errTot=0;}

    string rclStr = "Recoils_ONLY/Recoils_ONLY_angle_";
    rclStr.append(DetAngles[i]);
    t = (TTree*)f->Get(rclStr.c_str());
    Rcl = t->GetEntries();
    errRcl = (int) Rcl / sqrt(Rcl);
    if (Rcl<=0) {Rcl=0, errRcl=0;}

    string ejcStr = "Ejectiles_ONLY/Ejectiles_ONLY_angle_";
    ejcStr.append(DetAngles[i]);
    t = (TTree*)f->Get(ejcStr.c_str());
    Ejc = t->GetEntries();
    errEjc = (int) Ejc / sqrt(Ejc);
    if (Ejc<=0) {Ejc=0, errEjc=0;}

    if (i==0) {
      std::cout << 22.5 << "	" << Tot << "	" << errTot << "	" << Rcl << "	" << errRcl << "	" << Ejc << "	" << errEjc << endl;
      table << 22.5 << "	" << Tot << "	" << errTot << "	" << Rcl << "	" << errRcl << "	" << Ejc << "	" << errEjc << "\n";
    } else {
      std::cout << DetAngles[i] << "	" << Tot << "	" << errTot << "	" << Rcl << "	" << errRcl << "	" << Ejc << "	" << errEjc << endl;
      table << DetAngles[i] << "	" << Tot << "	" << errTot << "	" << Rcl << "	" << errRcl << "	" << Ejc << "	" << errEjc << "\n";
    }

  }

  table.close();

}
