graphics-assignment-1
=====================

Assignment 1

Ting-Chun Wang
Ena Hariyoshi

Optional features:

1. -ml: multiple spheres that shade each other  
   usage: -ml n x1 y1 r1 x2 y2 r2 ... (up to 5 spheres)  
   n:  number of spheres  
   x1: x coordinate of the center of sphere 1 (-1 ~ 1)  
   y1: y coordinate of the center of sphere 1 (-1 ~ 1)  
   r1: radius of sphere 1  
   example: -ml 3 -0.5 0 0.4 0.5 0.3 0.2 0.5 -0.3 0.2  
   note: The radius of the first sphere will be used as unit length  
  
2. -cb: changes shape to cube

3. -tn: toon shading

4. pressing 's' during execution: save files as "file.jpg"

Examples:  
-kd 0.5 0.5 0.5 -ks 0.5 0.5 0.5 -sp 200 -ka 0.01 0.03 0.01 -pl 1 1 1 0 1 0 -pl 0 -2 -2 1 1 1 -dl -1 -1 -1 0 1 0 -dl 0 -2 -2 1 1 1

-ka 0.03 0.02 0.05 -ks 0.2 0.1 0.5 -pl 2 3 4 0.2 0.5 0.2 -dl 1 1 1 0.7 0.7 0.7 -sp 2
