#include "DetectorConstruction.hh"

using namespace std;

DetectorConstruction::DetectorConstruction(TreeManager* aTMgr, CrossSectionManager* aCSMgr, InputManager* aInMgr) {
  TMgr = aTMgr;
  CSMgr = aCSMgr;
  InMgr = aInMgr;
}

DetectorConstruction::~DetectorConstruction() {

}

G4VPhysicalVolume* DetectorConstruction::Construct() {

  //------------------------------------------------------ materials

  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;
  G4int nel;

  Al = new G4Material("Aluminum", z= 13., a= 26.98*g/mole, density= 2.7*g/cm3);
  Si = new G4Material("Silicon", z= 14., a= 28.085*g/mole, density= 2.3290*g/cm3);

  double P;
  int targetA, targetZ;

  InMgr->GetVariable("targetP",P);//torr
  InMgr->GetVariable("targetA",targetA);//target mass number
  InMgr->GetVariable("targetZ",targetZ);//target atomic number

  string targetType;

  if      (targetZ == 2 && targetA == 4) {targetType = "4He";}
  else if (targetZ == 2 && targetA == 3) {targetType = "3He";}
  else if (targetZ == 1 && targetA == 1) {targetType = "H";}
  else {
    cerr << "Error in <DetectorConstruction::Construct>: Invalid target type" << endl;
    exit(1);
  }

  const Double_t temp = 21.0;//K
  const Double_t k = 1.3806e-23; // Boltzmann const J/k
  const double AMU = 1.6605e-27;//kg

  Double_t Nvol;

  cout << targetType << endl;

  if (targetType == "4He" || targetType == "3He") Nvol = P*133.3/(k*(temp+273.15));//atom/m3
  if (targetType == "H")                          Nvol = 2.*P*133.3/(k*(temp+273.15));//atom/m3

  double targetDen = Nvol*targetA*AMU;//kg/m3

  Target = new G4Material("Target", targetZ, double(targetA)*g/mole, targetDen*kg/m3);

//  cout << targetA << endl;

  //------------------------------------------------------ volumes

  //------------------------------ experimental hall (world volume)
  //------------------------------ beam line along x axis

  G4double expHall_x = 1.0*m;
  G4double expHall_y = 1.0*m;
  G4double expHall_z = 3.0*m;

  experimentalHall_box  = new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);

  TargetLength = CSMgr->GetTargetLength();
  TargetOffset = CSMgr->GetTargetOffset();

  string GeomType;
  InMgr->GetVariable("GeomType",GeomType);

  if (GeomType == "SONIK") ConstructSONIK();
  else if (GeomType == "Regular") ConstructRegular();
  else {
    cerr << "Error in <DetectorConstruction::Construct>: GeomType " << GeomType << " does not exist" << endl;
    exit(1);
  }

  return experimentalHall_phys;

}

//-----------------------------------------------------------------------------------
//Constructs SONIK geometry
//-----------------------------------------------------------------------------------

void DetectorConstruction::ConstructSONIK() {

  experimentalHall_log  = new G4LogicalVolume(experimentalHall_box,Al,"expHall_log",0,0,0);
  experimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),experimentalHall_log,"expHall",0,false,0);

  //------------------------------ Main Cylinder

  G4double MainCyl_CaseDiam = 30.*mm;

  G4double MainCyl_CaseDepth = TargetLength*cm;

  G4Tubs* MainCyl = new G4Tubs("MainCyl",0.,MainCyl_CaseDiam/2.,MainCyl_CaseDepth/2.,0.,360.*deg);

  mainCyl_log = new G4LogicalVolume(MainCyl,Target,"MainCyl",0,0,0);

  mainCyl_phys = new G4PVPlacement(0,G4ThreeVector(0,0,TargetOffset*cm+MainCyl_CaseDepth/2.),mainCyl_log,"MainCyl",experimentalHall_log,false,0);

  G4VisAttributes * MainCyl_VisAtt = new G4VisAttributes(G4Colour(0.,1.,0.));
  MainCyl_VisAtt->SetForceSolid(true);
  mainCyl_log->SetVisAttributes(MainCyl_VisAtt);

  //------------------------------ Aperture

  G4double Aperture_Width = 2.*mm;
  G4double Aperture_Length = 10.*mm;
  G4double Aperture_Depth = 3.175*mm;
  G4double Aperture_Diam = 2.*mm;
  //stupid setting so you can view arpertures in OGLI
  G4double Aperture_Offset = 0.00*mm;
  //G4double Aperture_Offset = 0.*mm;

  G4ThreeVector Vect(0.,0.,0.);

  G4Tubs* Aperture_Cyl = new G4Tubs("Aperture_Cyl",0.,Aperture_Diam/2.,Aperture_Depth/2.,0.,360.*deg);

  G4Box* Aperture_Box = new G4Box("Aperture_Box",Aperture_Length/2.,Aperture_Width/2.,Aperture_Depth/2.);

  G4UnionSolid* Aperture_Union = new G4UnionSolid("Aperture_Union",Aperture_Box, Aperture_Cyl,0,G4ThreeVector(Aperture_Length/2.+Aperture_Offset,0,0)); 

  G4UnionSolid* Aperture_Union2 = new G4UnionSolid("Aperture_Union2",Aperture_Union,Aperture_Cyl,0,G4ThreeVector(-Aperture_Length/2.-Aperture_Offset,0,0)); 

  G4LogicalVolume* Aperture_log = new G4LogicalVolume(Aperture_Union2,Target,"Aperture_Union2",0,0,0);

  G4VisAttributes* ApertureVisAtt = new G4VisAttributes(G4Colour(1.,0.,0.));
  ApertureVisAtt->SetForceSolid(true);
  Aperture_log->SetVisAttributes(ApertureVisAtt);

  //------------------------------ Si detectors

  G4double Si_Diam = 24.*mm;
  G4double Si_Depth = 10.*mm;

  G4Tubs* Si_Cyl = new G4Tubs("Si_Cyl",0.,Si_Diam/2.,Si_Depth/2.,0.,360.*deg);

  G4LogicalVolume* Si_log = new G4LogicalVolume(Si_Cyl,Si,"Si",0,0,0);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  TrackerSD* aTrackerSD = new TrackerSD("Si",TMgr,InMgr);
  SDman->AddNewDetector(aTrackerSD);
  Si_log->SetSensitiveDetector(aTrackerSD);

  //------------------------------ Si colimators

  double SiCol_Diam;
  InMgr->GetVariable("colsize",SiCol_Diam);
  SiCol_Diam *= mm;

  G4double SiCol_Depth = 1.*mm;

  G4Tubs* SiCol_Cyl = new G4Tubs("SiCol_Cyl",0.,SiCol_Diam/2.,SiCol_Depth/2.,0.,360.*deg);

  G4LogicalVolume* SiCol_log = new G4LogicalVolume(SiCol_Cyl,Target,"SiCol_Cyl",0,0,0);

  //------------------------------ TubeUpper

  G4double Tube_Diam = 12.*mm;
  G4double Tube_Length = 105.225*mm;

  G4Tubs* TubeUpper_Cyl = new G4Tubs("TubeUpper_Cyl",0.,Tube_Diam/2.,Tube_Length/2.,0.,360.*deg);

  G4LogicalVolume* TubeUpper_log = new G4LogicalVolume(TubeUpper_Cyl,Target,"TubeUpper_Cyl",0,0,0);

   //----------------------------- TubeLower

  G4double TubeLower_Diam = 15.*mm;

   //------------------------------ Row 0 (phi = 0*deg)

  G4double Origin0_x = 0.;//point on z axis that apertures point to
  G4double Origin0_y = 0.;
  G4double Origin0_z;
  InMgr->GetVariable("z_origin",Origin0_z);
  Origin0_z*=cm;

   //----------------00

  G4RotationMatrix* Aperture00_Rot = new G4RotationMatrix();

  G4double Aperture00_theta;
  InMgr->GetVariable("detAngle",Aperture00_theta);
  Aperture00_theta *= deg;

  G4double Aperture00_phi = 0.*deg;
  G4double Aperture00_dist = 61.588*mm;
  Aperture00_Rot->rotateX(Aperture00_theta);

  G4double Aperture00_x = Origin0_x + Aperture00_dist*sin(Aperture00_theta)*sin(Aperture00_phi);
  G4double Aperture00_y = Origin0_y + Aperture00_dist*sin(Aperture00_theta)*cos(Aperture00_phi);
  G4double Aperture00_z = Origin0_z + Aperture00_dist*cos(Aperture00_theta);

   //--------Si detector

  G4double Si00_x = Aperture00_x + (Aperture_Depth+Tube_Length*2.+Si_Depth+SiCol_Depth*2.)/2.*sin(Aperture00_theta)*sin(Aperture00_phi);
  G4double Si00_y = Aperture00_y + (Aperture_Depth+Tube_Length*2.+Si_Depth+SiCol_Depth*2.)/2.*sin(Aperture00_theta)*cos(Aperture00_phi);
  G4double Si00_z = Aperture00_z + (Aperture_Depth+Tube_Length*2.+Si_Depth+SiCol_Depth*2.)/2.*cos(Aperture00_theta);

  G4PVPlacement* Si00_phys = new G4PVPlacement(Aperture00_Rot,G4ThreeVector(Si00_x,Si00_y,Si00_z),Si_log,"Si00",experimentalHall_log,false,0);

  G4VisAttributes * Si_VisAtt = new G4VisAttributes(G4Colour(1.,0.,0.));
  Si_VisAtt->SetForceSolid(true);
  Si_log->SetVisAttributes(Si_VisAtt);

   //--------Si detector colimator

  G4double SiCol00_x = Aperture00_x + (Aperture_Depth+Tube_Length*2.+SiCol_Depth)/2.*sin(Aperture00_theta)*sin(Aperture00_phi);
  G4double SiCol00_y = Aperture00_y + (Aperture_Depth+Tube_Length*2.+SiCol_Depth)/2.*sin(Aperture00_theta)*cos(Aperture00_phi);
  G4double SiCol00_z = Aperture00_z + (Aperture_Depth+Tube_Length*2.+SiCol_Depth)/2.*cos(Aperture00_theta);

  G4PVPlacement* SiCol00_phys = new G4PVPlacement(Aperture00_Rot,G4ThreeVector(SiCol00_x,SiCol00_y,SiCol00_z),SiCol_log,"SiCol00",experimentalHall_log,false,0);

  G4VisAttributes* SiCol_VisAtt = new G4VisAttributes(G4Colour(0.,1.,0.));
  SiCol_VisAtt->SetForceSolid(true);
  SiCol_log->SetVisAttributes(SiCol_VisAtt);

   //--------Upper tube

  G4PVPlacement* Aperture00_phys = new G4PVPlacement(Aperture00_Rot,G4ThreeVector(Aperture00_x,Aperture00_y,Aperture00_z),Aperture_log,"Aperture00",experimentalHall_log,false,0);

  G4double TubeUpper00_x = Aperture00_x + (Aperture_Depth+Tube_Length)/2.*sin(Aperture00_theta)*sin(Aperture00_phi);
  G4double TubeUpper00_y = Aperture00_y + (Aperture_Depth+Tube_Length)/2.*sin(Aperture00_theta)*cos(Aperture00_phi);
  G4double TubeUpper00_z = Aperture00_z + (Aperture_Depth+Tube_Length)/2.*cos(Aperture00_theta);

  G4PVPlacement* TubeUpper00_phys = new G4PVPlacement(Aperture00_Rot,G4ThreeVector(TubeUpper00_x,TubeUpper00_y,TubeUpper00_z),TubeUpper_log,"TubeUpper00",experimentalHall_log,false,0);

  G4VisAttributes * TubeVisAtt = new G4VisAttributes(G4Colour(0.,0.,1.));
  TubeVisAtt->SetForceSolid(true);
  TubeUpper_log->SetVisAttributes(TubeVisAtt);

   //--------Lower Tube

  G4double TubeLower00_x = (Origin0_x + Aperture00_x - Aperture_Depth/2.*sin(Aperture00_theta)*sin(Aperture00_phi))/2.;
  G4double TubeLower00_y = (Origin0_y + Aperture00_y - Aperture_Depth/2.*sin(Aperture00_theta)*cos(Aperture00_phi))/2.;
  G4double TubeLower00_z = (Origin0_z + Aperture00_z - Aperture_Depth/2.*cos(Aperture00_theta))/2.;

  G4double TubeLower00_Length = pow(pow(Origin0_x-TubeLower00_x,2.)+pow(Origin0_y-TubeLower00_y,2.)+pow(Origin0_z-TubeLower00_z,2.),0.5)*2.;

  G4Tubs* TubeLower00_Cyl = new G4Tubs("TubeLower_Cyl",0.,TubeLower_Diam/2.,TubeLower00_Length/2.,0.,360.*deg);
  G4LogicalVolume* TubeLower00_log = new G4LogicalVolume(TubeLower00_Cyl,Target,"TubeLower_Cyl",0,0,0);

  G4PVPlacement* TubeLower00_phys = new G4PVPlacement(Aperture00_Rot,G4ThreeVector(TubeLower00_x,TubeLower00_y,TubeLower00_z),TubeLower00_log,"TubeLower00",experimentalHall_log,false,0);

  G4VisAttributes* TubeLowerVisAtt = new G4VisAttributes(G4Colour(0.,1.,0.));
  TubeLowerVisAtt->SetForceSolid(true);
  TubeLower00_log->SetVisAttributes(TubeLowerVisAtt);

}

//-----------------------------------------------------------------------------------
//Constructs regular DRAGON geometry
//-----------------------------------------------------------------------------------

void DetectorConstruction::ConstructRegular() {

  experimentalHall_log  = new G4LogicalVolume(experimentalHall_box,Target,"expHall_log",0,0,0);
  experimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),experimentalHall_log,"expHall",0,false,0);

//--------Si detector
  //position on beam axis where normal vector to detector face intersects (origin)
  Double_t Si00_xorigin = 0.*cm;
  Double_t Si00_yorigin = 0.*cm;
  Double_t Si00_zorigin = -1.001*cm;
  //detector face normal vector angle relative to beam axis
  Double_t Si00_theta = 30.*deg;//polar
  Double_t Si00_phi   = 0.*deg;//azmuthal

  Double_t Si_Depth    = 1.*cm;
  Double_t Si_Diam     = 1.*cm;
  //distance between centre of detector face and origin
  Double_t Si00_dist   = 6.73*cm;
//  Double_t Si00_dist   = 6.85*cm;
  //detector colimator thickness
  G4double SiCol_Depth = 0.079*cm;
  //geometric centre of detector position
  G4double Si00_x = Si00_xorigin+(Si_Depth/2.+SiCol_Depth+Si00_dist)*sin(Si00_theta)*sin(Si00_phi);
  G4double Si00_y = Si00_yorigin+(Si_Depth/2.+SiCol_Depth+Si00_dist)*sin(Si00_theta)*cos(Si00_phi);
  G4double Si00_z = Si00_zorigin+(Si_Depth/2.+SiCol_Depth+Si00_dist)*cos(Si00_theta);

  G4RotationMatrix* Si00_Rot = new G4RotationMatrix();
  Si00_Rot->rotateX(Si00_theta);

  G4Tubs* Si_Cyl = new G4Tubs("Si_Cyl",0.,Si_Diam/2.,Si_Depth/2.,0.,360.*deg);

  G4LogicalVolume* Si_log = new G4LogicalVolume(Si_Cyl,Si,"Si",0,0,0);

  G4PVPlacement* Si00_phys = new G4PVPlacement(Si00_Rot,G4ThreeVector(Si00_x,Si00_y,Si00_z),Si_log,"Si00",experimentalHall_log,false,0);

  G4VisAttributes* Si_VisAtt = new G4VisAttributes(G4Colour(1.,0.,0.));
  Si_VisAtt->SetForceSolid(true);
  Si_log->SetVisAttributes(Si_VisAtt);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  TrackerSD* aTrackerSD = new TrackerSD("Si",TMgr,InMgr);
  SDman->AddNewDetector(aTrackerSD);
  Si_log->SetSensitiveDetector(aTrackerSD);

  //------------------------------ Si colimator

  G4double SiCol_Diam = 4.*cm;
  G4double SiCol2_Diam;
  InMgr->GetVariable("colsize",SiCol2_Diam);
  SiCol_Diam *= mm;
  //geometric centre position
  G4double SiCol_x = Si00_xorigin+(SiCol_Depth/2.+Si00_dist)*sin(Si00_theta)*sin(Si00_phi);
  G4double SiCol_y = Si00_yorigin+(SiCol_Depth/2.+Si00_dist)*sin(Si00_theta)*cos(Si00_phi);
  G4double SiCol_z = Si00_zorigin+(SiCol_Depth/2.+Si00_dist)*cos(Si00_theta);

  G4Tubs* SiCol_Cyl = new G4Tubs("SiCol_Cyl",0.,SiCol_Diam/2.,SiCol_Depth/2.,0.,360.*deg);
  G4LogicalVolume* SiCol_log = new G4LogicalVolume(SiCol_Cyl,Al,"SiCol_Cyl",0,0,0);

  G4PVPlacement* SiCol_phys = new G4PVPlacement(Si00_Rot,G4ThreeVector(SiCol_x,SiCol_y,SiCol_z),SiCol_log,"SiCol",experimentalHall_log,false,0);

  G4Tubs* SiCol2_Cyl = new G4Tubs("SiCol2_Cyl",0.,SiCol2_Diam/2.,SiCol_Depth/2.,0.,360.*deg);
  G4LogicalVolume* SiCol2_log = new G4LogicalVolume(SiCol2_Cyl,Target,"SiCol2_Cyl",0,0,0);

  G4PVPlacement* SiCol2_phys = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),SiCol2_log,"SiCol2",SiCol_log,false,0);

  G4VisAttributes* SiCol_VisAtt = new G4VisAttributes(G4Colour(1.,1.,1.));
  SiCol_VisAtt->SetForceWireframe(true);
  SiCol2_log->SetVisAttributes(SiCol_VisAtt);

//--------Colimator plane
//colimator that sits on ridge in chamber, not the detector colimator

  G4double col_depth = 0.07*cm;

  G4Box* col_box  = new G4Box("col_box",10.*cm,col_depth/2.,10.*cm);

  G4LogicalVolume* col_log = new G4LogicalVolume(col_box,Al,"col_log",0,0,0);

  G4double col_offset = (1.04-0.15)*cm;

  G4PVPlacement* col_phys = new G4PVPlacement(0,G4ThreeVector(0.,col_offset-col_depth/2.,0.),col_log,"col",experimentalHall_log,false,0);

//--------Colimator hole

  G4double col_diam = 0.5*cm;
  G4double col_length = 0.5*cm;
  G4double col_width = 0.5*cm;

  G4RotationMatrix* col_Rot = new G4RotationMatrix();
  col_Rot->rotateX(90.*deg);

  G4Tubs* col_Cyl = new G4Tubs("col_Cyl",0.,col_diam/2.,col_depth/2.,0.,360.*deg);

  G4Box* col2_Box = new G4Box("col2_Box",col_length/2.,col_width/2.,col_depth/2.);

  G4UnionSolid* col_Union = new G4UnionSolid("col_Union",col2_Box, col_Cyl,0,G4ThreeVector(col_length/2.+0.02*cm,0,0)); 

  G4UnionSolid* col_Union2 = new G4UnionSolid("col_Union2",col_Union,col_Cyl,0,G4ThreeVector(-col_length/2.-0.02*cm,0,0)); 

  G4LogicalVolume* col2_log = new G4LogicalVolume(col_Union2,Target,"col_Union2",0,0,0);

  G4PVPlacement* col2_phys = new G4PVPlacement(col_Rot,G4ThreeVector(0.,0.,0.62*cm),col2_log,"col2",col_log,false,0);

}
