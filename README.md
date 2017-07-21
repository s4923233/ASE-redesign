#ASE-redesign

This is the ASE resubmission project.
It needs to be built using C++11.
It uses the NGL library and the Eigen library for matrix calculations.

The program implements a 2D Mac-Grid to be used with fluid simulators.
Simple visualisation methods are provided to track different features of a fluid simulator like particles position and active cells.

The project includes also a fluid simulator wich implements the FLIP routine as described in (Bridson,2011).

#Limitations:
A set of methods to implement a pressure solver (Bridson,2011) are also included, but the pressure solver is not fully working.
The simulation assumes the presence of a solid bounding box, which contains the fluid.


#References:
Bridson,R, 2011. Fluid Simulation for Computer Graphics. 2nd ed.