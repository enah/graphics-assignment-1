graphics-assignment-1
=====================

Assignment 1

Optional features:

-ml: multiple spheres that shade each other
usage: -ml n x1 y1 r1 x2 y2 r2 ... (up to 5 spheres)
n:  number of spheres
1: x coordinate of the center of sphere 1 (-1 ~ 1)
y1: y coordinate of the center of sphere 1 (-1 ~ 1)
r1: radius of sphere 1
example: -ml 3 -0.5 0 0.4 0.5 0.3 0.2 0.5 -0.3 0.2
The radius of the first sphere will be used as unit length
  
-cb: changes shape to cube

-tn: uses toon shading

Example:
-kd 0.5 0.5 0.5 -ks 0.5 0.5 0.5 -sp 200 -ka 0.01 0.03 0.01 -pl 1 1 1 0 -2 -2 -pl 1 0 0 1 1 1 -dl -1 -1 -1 0 -2 -2 -dl 0 1 0 1 1 1
