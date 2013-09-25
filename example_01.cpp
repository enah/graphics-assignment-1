#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif


#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;

float ka[3];
float kd[3];
float ks[3];
float sp;
int pl_num;
float pl_pos[5][3];
float pl_color[5][3];
int dl_num;
float dl_dir[5][3];
float dl_color[5][3];

enum SHAPE { SPHERE, CUBE };

bool toon;
bool shape;
bool multiple;
int mul_num;
float mul_c_x[5], mul_c_y[5], mul_r[5];

//****************************************************
// Simple init function
//****************************************************
void initScene() {

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel centers 
  // Note: Need to check for gap
  // bug on inst machines.
}

//****************************************************
// Draw a filled circle.  
//****************************************************

/*void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);

  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by loopig over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));

  for (i = minI; i < maxI; i++) {
    for (j = minJ; j < maxJ; j++) {

      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      float dist = sqrt(sqr(x) + sqr(y));

      if (dist <= radius) {

        // This is the front-facing Z coordinate
        float z = sqrt(radius*radius-dist*dist);

        setPixel(i,j, 1.0, 0.0, 0.0);

        // This is amusing, but it assumes negative color values are treated reasonably.
        // setPixel(i,j, x/radius, y/radius, z/radius );
      }


    }
  }


  glEnd();
}*/

void shade(float centerX, float centerY, float radius, int shape) {
    
    glBegin(GL_POINTS);
    
    int minI = max(0,(int)floor(centerX-radius));
    int maxI = min(viewport.w-1,(int)ceil(centerX+radius));
    
    int minJ = max(0,(int)floor(centerY-radius));
    int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));    
    
    float r_final, g_final, b_final;
    
    for (int i = minI; i < maxI; i++) {
        for (int j = minJ; j < maxJ; j++) {
            
            float x = (i+0.5-centerX);
            float y = (j+0.5-centerY);
            float z;
            float cos_theta;
            float specular;
            
            float dist = sqrt(sqr(x) + sqr(y));            
            if (shape == SPHERE && dist <= radius || shape == CUBE) {
                
                if (shape == SPHERE)
                    z = sqrt(radius*radius-dist*dist);
                else
                    z = radius;
                
                r_final = g_final = b_final = 0;
                
                // point light source
                for (int idx = 0; idx < pl_num; idx++) {
                    
                    // diffuse part
                    float pl_d[3] = {pl_pos[idx][0]*radius-x, pl_pos[idx][1]*radius-y, pl_pos[idx][2]*radius-z};
                    float pl_d_sum = sqrt(sqr(pl_d[0]) + sqr(pl_d[1]) + sqr(pl_d[2]));
                    if (shape == SPHERE)
                        cos_theta = (pl_d[0]*x + pl_d[1]*y + pl_d[2]*z) / (pl_d_sum * radius);
                    else
                        cos_theta = pl_d[2] / pl_d_sum;
                    r_final += kd[0] * max(0.0f, cos_theta) * pl_color[idx][0];
                    g_final += kd[1] * max(0.0f, cos_theta) * pl_color[idx][1];
                    b_final += kd[2] * max(0.0f, cos_theta) * pl_color[idx][2];
                    
                    // specular part
                    if (shape == SPHERE)
                        specular = pow(max(0.0f, -pl_d[2]/pl_d_sum + 2*cos_theta*z/radius), sp);
                    else
                        specular = pow(max(0.0f, cos_theta), sp);
                    r_final += ks[0] * specular * pl_color[idx][0];
                    g_final += ks[1] * specular * pl_color[idx][1];
                    b_final += ks[2] * specular * pl_color[idx][2];
                    
                    // ambient part
                    r_final += ka[0] * pl_color[idx][0];
                    g_final += ka[1] * pl_color[idx][1];
                    b_final += ka[2] * pl_color[idx][2];
                }
                
                // directional light source
                for (int idx = 0; idx < dl_num; idx++) {
                    
                    // diffuse part
                    float dl_d_sum = sqrt(sqr(dl_dir[idx][0])+sqr(dl_dir[idx][1])+sqr(dl_dir[idx][2]));
                    if (shape == SPHERE)
                        cos_theta = -(dl_dir[idx][0]*x + dl_dir[idx][1]*y + dl_dir[idx][2]*z) / (dl_d_sum * radius);
                    else
                        cos_theta = -dl_dir[idx][2] / dl_d_sum;
                    r_final += kd[0] * max(0.0f, cos_theta) * dl_color[idx][0];
                    g_final += kd[1] * max(0.0f, cos_theta) * dl_color[idx][1];
                    b_final += kd[2] * max(0.0f, cos_theta) * dl_color[idx][2];
                    
                    // specular part
                    if (shape == SPHERE)
                        specular = pow(max(0.0f, dl_dir[idx][2]/dl_d_sum + 2*cos_theta*z/radius), sp);
                    else
                        specular = pow(max(0.0f, cos_theta), sp);
                    r_final += ks[0] * specular * dl_color[idx][0];
                    g_final += ks[1] * specular * dl_color[idx][1];
                    b_final += ks[2] * specular * dl_color[idx][2];
                    
                    // ambient part
                    r_final += ka[0] * dl_color[idx][0];
                    g_final += ka[1] * dl_color[idx][1];
                    b_final += ka[2] * dl_color[idx][2];
                }
                if (toon) {
                    r_final = float(int(r_final*10)) / 10;
                    g_final = float(int(g_final*10)) / 10;
                    b_final = float(int(b_final*10)) / 10;
                }
                if (toon) {
                    r_final = float(int(r_final*10)) / 10;
                    g_final = float(int(g_final*10)) / 10;
                    b_final = float(int(b_final*10)) / 10;
                }
                setPixel(i, j, r_final, g_final, b_final);
            }
            
            
        }
    }
    glEnd();
}

void shade_multiple(int s_num, float centerX[], float centerY[], float radius[]) {
    
    glBegin(GL_POINTS);
    
    for (int s_idx = 0; s_idx < s_num; s_idx++) {
    
        int minI = max(0,(int)floor(centerX[s_idx]-radius[s_idx]));
        int maxI = min(viewport.w-1,(int)ceil(centerX[s_idx]+radius[s_idx]));
    
        int minJ = max(0,(int)floor(centerY[s_idx]-radius[s_idx]));
        int maxJ = min(viewport.h-1,(int)ceil(centerY[s_idx]+radius[s_idx]));
    
        float r_final, g_final, b_final;
    
        for (int i = minI; i < maxI; i++) {
            for (int j = minJ; j < maxJ; j++) {
            
                float x_c = (i+0.5-centerX[s_idx]); // relative to center of circle
                float y_c = (j+0.5-centerY[s_idx]);
                float x_o = (i+0.5-viewport.w/2);   // relative to origin
                float y_o = (j+0.5-viewport.h/2);
                    
                float z;
                float cos_theta;
                float specular;
            
                float dist = sqrt(sqr(x_c) + sqr(y_c));
                if (dist <= radius[s_idx]) {
                
                    z = sqrt(radius[s_idx]*radius[s_idx]-dist*dist);
                
                    r_final = g_final = b_final = 0;
                
                    // point light source
                    for (int idx = 0; idx < pl_num; idx++) {
                        
                        float pl_d[3] = {pl_pos[idx][0]*radius[0]-x_o, pl_pos[idx][1]*radius[0]-y_o, pl_pos[idx][2]*radius[0]-z};
                        float pl_d_sum = sqrt(sqr(pl_d[0]) + sqr(pl_d[1]) + sqr(pl_d[2]));
                        bool shaded = 0;
                        for (int s = 0; s < s_num; s++) {
                            if (s == s_idx) continue;
                            float x_rc = x_o - (centerX[s]-viewport.w/2);
                            float y_rc = y_o - (centerY[s]-viewport.h/2);
                            shaded = (sqr(pl_d[0]*x_rc + pl_d[1]*y_rc + pl_d[2]*z) - sqr(pl_d_sum) * (x_rc*x_rc + y_rc*y_rc + z*z - sqr(radius[s]))) > 0;
                            shaded = shaded & ((pl_d[0]*x_rc + pl_d[1]*y_rc + pl_d[2]*z) < 0);
                            if (shaded) break;
                        }
                        
                        if (!shaded) {
                            
                            // diffuse part
                            cos_theta = (pl_d[0]*x_c + pl_d[1]*y_c + pl_d[2]*z) / (pl_d_sum * radius[s_idx]);
                            r_final += kd[0] * max(0.0f, cos_theta) * pl_color[idx][0];
                            g_final += kd[1] * max(0.0f, cos_theta) * pl_color[idx][1];
                            b_final += kd[2] * max(0.0f, cos_theta) * pl_color[idx][2];
                    
                            // specular part
                            specular = pow(max(0.0f, -pl_d[2]/pl_d_sum + 2*cos_theta*z/radius[s_idx]), sp);
                            r_final += ks[0] * specular * pl_color[idx][0];
                            g_final += ks[1] * specular * pl_color[idx][1];
                            b_final += ks[2] * specular * pl_color[idx][2];
                        }
                    
                        // ambient part
                        r_final += ka[0] * pl_color[idx][0];
                        g_final += ka[1] * pl_color[idx][1];
                        b_final += ka[2] * pl_color[idx][2];
                    }
                
                    // directional light source
                    for (int idx = 0; idx < dl_num; idx++) {
                    
                        float dl_d_sum = sqrt(sqr(dl_dir[idx][0])+sqr(dl_dir[idx][1])+sqr(dl_dir[idx][2]));
                        
                        bool shaded = 0;
                        for (int s = 0; s < s_num; s++) {
                            if (s == s_idx) continue;
                            float x_rc = x_o - (centerX[s]-viewport.w/2);
                            float y_rc = y_o - (centerY[s]-viewport.h/2);
                            shaded = (sqr(dl_dir[idx][0]*x_rc + dl_dir[idx][1]*y_rc + dl_dir[idx][2]*z) - sqr(dl_d_sum) * (x_rc*x_rc + y_rc*y_rc + z*z - sqr(radius[s]))) > 0;
                            shaded = shaded & ((dl_dir[idx][0]*x_rc + dl_dir[idx][1]*y_rc + dl_dir[idx][2]*z) > 0);
                            if (shaded) break;
                        }
                        
                        if (!shaded) {
                            
                            // diffuse part
                            cos_theta = -(dl_dir[idx][0]*x_c + dl_dir[idx][1]*y_c + dl_dir[idx][2]*z) / (dl_d_sum * radius[s_idx]);
                            r_final += kd[0] * max(0.0f, cos_theta) * dl_color[idx][0];
                            g_final += kd[1] * max(0.0f, cos_theta) * dl_color[idx][1];
                            b_final += kd[2] * max(0.0f, cos_theta) * dl_color[idx][2];
                    
                            // specular part
                            specular = pow(max(0.0f, dl_dir[idx][2]/dl_d_sum + 2*cos_theta*z/radius[s_idx]), sp);
                            r_final += ks[0] * specular * dl_color[idx][0];
                            g_final += ks[1] * specular * dl_color[idx][1];
                            b_final += ks[2] * specular * dl_color[idx][2];
                        }
                        // ambient part
                        r_final += ka[0] * dl_color[idx][0];
                        g_final += ka[1] * dl_color[idx][1];
                        b_final += ka[2] * dl_color[idx][2];
                    }
                    
                    if (toon) {
                        r_final = float(int(r_final*10)) / 10;
                        g_final = float(int(g_final*10)) / 10;
                        b_final = float(int(b_final*10)) / 10;
                    }
                    setPixel(i, j, r_final, g_final, b_final);
                }
            }
            
        }
    }
    glEnd();
}


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

    glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

    glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
    glLoadIdentity();				        // make sure transformation is "zero'd"


    // Start drawing
    
    if (!multiple)
        shade(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0, shape);
    else {
        //float centerX[3] = {viewport.w / 4, viewport.w * 3/4, viewport.w * 3/4};
        //float centerY[3] = {viewport.h / 2, viewport.h / 4,   viewport.h * 3/4};
        //float radius[3]  = {min(viewport.w, viewport.h) / 5, min(viewport.w, viewport.h) / 6, min(viewport.w, viewport.h) / 6};
        //shade_multiple(3, centerX, centerY, radius);
        shade_multiple(mul_num, mul_c_x, mul_c_y, mul_r);
    }

    glFlush();
    glutSwapBuffers();					// swap buffers (we earlier set double buffer)

}

/*
   windowDump code taken from http://paulbourke.net/miscellaneous/windowdump/
   Write the current view to a file
   The multiple fputc()s can be replaced with
      fwrite(image,width*height*3,1,fptr);
   If the memory pixel order is the same as the destination file format.
*/
int windowDump(void)
{
   int i,j;
   FILE *fptr;
   //static int counter = 0; /* This supports animation sequences */
   char fname[32] = "picture.raw";
   unsigned char *image;

   /* Allocate our buffer for the image */
   // if ((image = malloc(3*viewport.w*viewport.h*sizeof(char))) == NULL) {
   //    fprintf(stderr,"Failed to allocate memory for image\n");
   //    return(0);
   // }

   glPixelStorei(GL_PACK_ALIGNMENT,1);

   /* Open the file */
/*
   if (stereo)
      sprintf(fname,"L_%04d.raw",counter);
   else
*/
   // sprintf(fname,"C_%04d.raw",counter);
   if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"Failed to open file for window dump\n");
      return(0);
   }

   /* Copy the image into our buffer */
   glReadBuffer(GL_BACK_LEFT);
   glReadPixels(0,0,viewport.w,viewport.h,GL_RGB,GL_UNSIGNED_BYTE,image);

   /* Write the raw file */
   /* fprintf(fptr,"P6\n%d %d\n255\n",width,height); for ppm */
   for (j=viewport.h-1;j>=0;j--) {
      for (i=0;i<viewport.w;i++) {
         fputc(image[3*j*viewport.w+3*i+0],fptr);
         fputc(image[3*j*viewport.w+3*i+1],fptr);
         fputc(image[3*j*viewport.w+3*i+2],fptr);
      }
   }
   fclose(fptr);

   // if (stereo) {
   //    /* Open the file */
   //    sprintf(fname,"R_%04d.raw",counter);
   //    if ((fptr = fopen(fname,"w")) == NULL) {
   //       fprintf(stderr,"Failed to open file for window dump\n");
   //       return(FALSE);
   //    }

   //    /* Copy the image into our buffer */
   //    glReadBuffer(GL_BACK_RIGHT);
   //    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

   //    /* Write the raw file */
   //    /* fprintf(fptr,"P6\n%d %d\n255\n",width,height); for ppm */
   //    for (j=height-1;j>=0;j--) {
   //       for (i=0;i<width;i++) {
   //          fputc(image[3*j*width+3*i+0],fptr);
   //          fputc(image[3*j*width+3*i+1],fptr);
   //          fputc(image[3*j*width+3*i+2],fptr);
   //       }
   //    }
   //    fclose(fptr);
   // }

   /* Clean up */
   //counter++;
   free(image);
   return(1);
}



void myKeyboard(unsigned char key, int mouseX, int mouseY) {
    switch (key) {
        case ' ':
            exit(0);
            break;
    	case 's':
	    windowDump();
	    break;
        default:
            break;
    }
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
    //This initializes glut
    glutInit(&argc, argv);
    
    //This tells glut to use a double-buffered window with red, green, and blue channels
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    
    // Initalize theviewport size
    viewport.w = 400;
    viewport.h = 400;

    kd[0] = kd[1] = kd[2] = 0;
    ks[0] = ks[1] = ks[2] = 0;
    sp = 0;
    ka[0] = ka[1] = ka[2] = 0;
    pl_num = dl_num = 0;
    
    shape = SPHERE;
    multiple = false;
    mul_num = 0;
    toon = false;
    
    int n = 1;
    while (n < argc) {
    	
        if (!strncmp(argv[n], "-ka", 3)) {
    	    ka[0] = atof(argv[++n]);
    	    ka[1] = atof(argv[++n]);
    	    ka[2] = atof(argv[++n]);
    	}
        else if (!strncmp(argv[n], "-kd", 3)) {
    	    kd[0] = atof(argv[++n]);
    	    kd[1] = atof(argv[++n]);
    	    kd[2] = atof(argv[++n]);
    	}
        else if (!strncmp(argv[n], "-ks", 3)) {
    	    ks[0] = atof(argv[++n]);
    	    ks[1] = atof(argv[++n]);
    	    ks[2] = atof(argv[++n]);
    	}
        else if (!strncmp(argv[n], "-sp", 3)) {
    	    sp = atof(argv[++n]);
    	}
        else if (!strncmp(argv[n], "-pl", 3)) {
            pl_pos[pl_num][0] = atof(argv[++n]);
            pl_pos[pl_num][1] = atof(argv[++n]);
            pl_pos[pl_num][2] = atof(argv[++n]);
            pl_color[pl_num][0] = atof(argv[++n]);
            pl_color[pl_num][1] = atof(argv[++n]);
            pl_color[pl_num][2] = atof(argv[++n]);
            pl_num++;
    	}
        else if (!strncmp(argv[n], "-dl", 3)) {
            dl_dir[dl_num][0] = atof(argv[++n]);
            dl_dir[dl_num][1] = atof(argv[++n]);
            dl_dir[dl_num][2] = atof(argv[++n]);
            dl_color[dl_num][0] = atof(argv[++n]);
            dl_color[dl_num][1] = atof(argv[++n]);
            dl_color[dl_num][2] = atof(argv[++n]);
            dl_num++;
    	}
        ////////////// bonus options ////////////////////
        else if (!strcmp(argv[n], "-cb")) {  // changes shape to cube
            shape = CUBE;
        }
        else if (!strcmp(argv[n], "-tn")) {  // toon shading
            toon = true;
        }
        // -ml: multiple spheres that shade each other
        // usage: -ml n x1 y1 r1 x2 y2 r2 ... (up to 5 spheres)
        // n:  number of spheres
        // x1: x coordinate of the center of sphere 1 (-1 ~ 1)
        // y1: y coordinate of the center of sphere 1 (-1 ~ 1)
        // r1: radius of sphere 1
        // ex: -ml 3 -0.5 0 0.4 0.5 0.3 0.2 0.5 -0.3 0.2
        // p.s. the radius of the first sphere would be used as unit length
        else if (!strcmp(argv[n], "-ml")) {
            multiple = true;
            mul_num = atof(argv[++n]);
            for (int i = 0; i < mul_num; i++) {
                mul_c_x[i] = atof(argv[++n]); 
                mul_c_y[i] = atof(argv[++n]);
                mul_r[i]   = atof(argv[++n]);
                
                mul_c_x[i] = (mul_c_x[i] + 1) * viewport.w / 2;
                mul_c_y[i] = (mul_c_y[i] + 1) * viewport.h / 2;
                mul_r[i]   = mul_r[i] * min(viewport.w, viewport.h) / 2;
            }
        }
        
    	n++;
    }

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();							// quick function to set up scene

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized
  glutKeyboardFunc(myKeyboard);

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}








