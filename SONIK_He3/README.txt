ROOT C++ Script rootscript2.C

-ROOT C++ script for calculating the cross section based on simulation data
-Compile in ROOT with the command { .L rootscript2.C+ } and run using { rootscipt2() }

-Accesses data created by the script sonik.sh (directories 1mm, 2mm, etc. with directories 22.5deg, 25deg, 35deg, etc.)
-Uses same function as scattering simulation (in /local/astro/scat/src/CrossCalc.hxx) to get input cross section data to calculate ratio
-Constants for nVol (atoms/cm^3) should be set to the same as in the scattering simulation (CrossSectionManager.cc and config.dat)
-Effective target length z (used to get atoms/cm^2) obtained from ROOT Script detView.C


#############################################################################################################################################################

ROOT C++ Script detView.C 

-ROOT C++ script for calculating based on geometry an effective z for each collimator, and angle setup
-Compile in ROOT with the command { .L detView.C+ } and run using { detView() }

-Integrates a weighing factor sigma(z) over z (between 19cm and 21cm) 
-sigma(z) is the double integral of the visibility ratio of the collimator multiplied by the relative intensity of the beam over the x,y plane...
-divided by the double intergal of the relative intensity of the beam over the x,y plane

-Visibility ratio is calculated by the following proccess
--Setup slit (4 vertices) and collimator (20 vertices and a center) with actual sizes and locations
--A camera position x,y,z is specified
--Entire setup is transformed to coordinates centered at the camera, with the z-axis now aligned between the camera and collimator center
--A camera projection matrix is used to transform the scene to a 2D image from the camera's perspective
---This can be viewed by commenting out the first return in the IonViewRatio function and running it individually after compiling, IonViewRatio(x,y,z,colRadius(cm),angle(deg))
--The 2D projection of the collimator is split into 400 pieces (20 divides radially, 20 divides angularly)
---Each piece is checked if within slit (within slit if any ray originating from piece intersects slit lines an odd number of times)
--Each visible piece area is summed up and divided by total area

-Also takes into account how much larger(or smaller) the solid angle is at (x,y,z) than at (0,0,20) 

-Relative intensity of the beam is a 2D Gaussian centered at x=0,y=0 with std deviations 1mm in both x and y (this is what is used in the simulation)

-Program will print Collimator Diameter, Detector Angle, and effective z for all setups (Runtime ~5-10mins, depending on zbins and xybins)

