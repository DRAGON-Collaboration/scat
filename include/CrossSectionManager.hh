#ifndef CrossSectionManager_h
#define CrossSectionManager_h 1

#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TFile.h>
#include <TFolder.h>
#include <TTree.h>
#include <stdio.h>
#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph2D.h>
#include <TSystem.h>
#include <TSpline.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TF2.h>
#include "KinManager.hh"
#include "InputManager.hh"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <ctime>

class CrossSectionManager {

public:
  CrossSectionManager(InputManager* aInMgr);
  ~CrossSectionManager();

  void GenerateCrossSection();
  bool GenerateEvent();

  bool GetEvent();
  double GetN_scat();
  double GetX();
  double GetY();
  double GetZ();
  double GetRcl_Theta_lab();
  double GetPhi();
  double GetRcl_E_lab();
  double GetBeam_E_cm();
  double GetIonA();
  double GetIonZ();
  double GetTheta_cm();
  double GetEjc_E_lab();
  double GetEjc_Theta_lab();

  double GetTargetLength();
  double GetTargetOffset();

  double res(Double_t ecm, Double_t thetacm);
  double res_exp(Double_t ecm, Double_t thetacm);
  double ruth(Double_t z_a, Double_t z_b, Double_t ecm, Double_t thetacm);

private:
  double Rcl_theta_lab, phi, theta_cm/*rad*/, Rcl_E_lab/*MeV*/, Beam_E_cm, n_scat/*total number of scattering events*/;
  double Ejc_theta_lab/*rad*/, Ejc_E_lab/*MeV*/, n_beam/*total number of scattering events*/; /////////////////////////////
  double zmin, zmax;//beam axis limits (cm)
  double phi_min, phi_max;//azmuthal angular limits
  double TargetLength, TargetOffset; //length of target & offset from orign in cm
  double pos[3]; // position of scattering event {x,y,z} in cm
  double a[3]; // mass numbers: {beam,target,recoil}
  double z[3]; // charges in Z
  Double_t z_max, z_min; // beam axis limits in cm
  int ebin_min;
  int EventCount;//tracks number of events
  clock_t time; //used to estimate time left in simulation for very long runs
  int prev_count;
  int pdone, pdone_prev;//percentage of scattering events completed
  bool Event;//is there event from GenerateCrossSection()
  // Kinematic variables.
  string cxFile;
  enum {BEAM=0, TARGET=1, RECOIL=2};

  string GeomType;

  TFile* CSfile;
  TH2F hE;//beam energy profile through target
  TH2F hE_lim;
  TH2F hEtheta;//contains cs data for all energy and theta bins
  TH1F hEscat;//scattering pdf for given energy
  TH1F hEscat_lim;

  KinManager* KinMgr;
  InputManager* InMgr;

};

#endif
