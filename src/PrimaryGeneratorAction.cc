#include "PrimaryGeneratorAction.hh"

#include "TRandom.h"

namespace {
  TRandom* g_random = new TRandom();
}

PrimaryGeneratorAction::PrimaryGeneratorAction(TreeManager* aTMgr, CrossSectionManager* aCSMgr, InputManager* aInMgr) {

  TMgr  = aTMgr;
  CSMgr = aCSMgr;
  InMgr = aInMgr;
  srand(time(0)); //seed random number generator

  InMgr->GetVariable("targetA",Target_Ion_A);
  InMgr->GetVariable("targetZ",Target_Ion_Z);

  InMgr->GetVariable("beamA",Beam_Ion_A); //for beam particle
  InMgr->GetVariable("beamZ",Beam_Ion_Z);

  InMgr->GetVariable("GeomType",GeomType);

  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);

  particleTable = G4ParticleTable::GetParticleTable();

  EventCounts = 0;

}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  delete particleGun;
}

//------------------------------------------------------------------------
//Generates single scattering event, recoil and ejectile
//------------------------------------------------------------------------

void PrimaryGeneratorAction::IonBeam(G4Event* anEvent) {

  bool Event = FALSE;

  Event = CSMgr->GenerateEvent();//genrates postion/energy/direction of event

  if (Event == true) {//if event occurs within config limits
    particleGun->SetParticleDefinition(particleTable->GetIon(Target_Ion_Z,Target_Ion_A,0));

    double Rcl_E_lab = CSMgr->GetRcl_E_lab();

    particleGun->SetParticleEnergy((Rcl_E_lab)*MeV);

    double Beam_E_cm = CSMgr->GetBeam_E_cm(); //beam before collision, cm frame
    double theta_cm = CSMgr->GetTheta_cm();
    double Rcl_theta_lab = CSMgr->GetRcl_Theta_lab();
    double phi = CSMgr->GetPhi();

    Rcl_theta_lab = g_random->Gaus(Rcl_theta_lab,0.001);

    //Determine direction vector of event
    double Rcl_vx = sin(Rcl_theta_lab)*sin(phi);
    double Rcl_vy = sin(Rcl_theta_lab)*cos(phi);
    double Rcl_vz = cos(Rcl_theta_lab);

    double TargetOffset = CSMgr->GetTargetOffset();

    double x = CSMgr->GetX();
    double y = CSMgr->GetY();
    double z = CSMgr->GetZ()+TargetOffset;//start of target is offset from axis origin

    G4ThreeVector Rcl_vect(Rcl_vx,Rcl_vy,Rcl_vz);
    G4ThreeVector pos(x*cm,y*cm,z*cm);

    //Set variables that fill root tree

    TMgr->Set0E_lab(Rcl_E_lab);
    TMgr->Set0Tht_cm(theta_cm);
    TMgr->Set0Tht_lab(Rcl_theta_lab);
    TMgr->Set0Phi_lab(phi);
    TMgr->Set0x(x);
    TMgr->Set0y(y);
    TMgr->Set0z(z);
    TMgr->Set0E_beam(Beam_E_cm); //beam before collision, cm frame

    particleGun->SetParticlePosition(pos);
    particleGun->SetParticleMomentumDirection(Rcl_vect);
    particleGun->GeneratePrimaryVertex(anEvent);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    particleGun->SetParticleDefinition(particleTable->GetIon(Beam_Ion_Z,Beam_Ion_A,0));

    double Ejc_E_lab = CSMgr->GetEjc_E_lab();

    particleGun->SetParticleEnergy((Ejc_E_lab)*MeV);

    double Ejc_theta_lab = CSMgr->GetEjc_Theta_lab();

    Ejc_theta_lab = g_random->Gaus(Ejc_theta_lab,0.001); //straggling

    //Determine direction vector of event
    double Ejc_vx = sin(Ejc_theta_lab)*sin(phi);
    double Ejc_vy = sin(Ejc_theta_lab)*cos(phi);
    double Ejc_vz = cos(Ejc_theta_lab);

    G4ThreeVector Ejc_vect(Ejc_vx,Ejc_vy,Ejc_vz);

    //Set variables that fill root tree
    TMgr->Set1E_lab(Ejc_E_lab);
    TMgr->Set1Tht_cm(theta_cm);
    TMgr->Set1Tht_lab(Ejc_theta_lab);
    TMgr->Set1Phi_lab(phi);
    TMgr->Set1x(x);
    TMgr->Set1y(y);
    TMgr->Set1z(z);
    TMgr->Set1E_beam(Beam_E_cm);

    particleGun->SetParticlePosition(pos);
    particleGun->SetParticleMomentumDirection(Ejc_vect);
    particleGun->GeneratePrimaryVertex(anEvent);

  }

}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

  IonBeam(anEvent);

}
