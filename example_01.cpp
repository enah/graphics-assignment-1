#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

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

#include <time.h>
#include <math.h>


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

void shade(float centerX, float centerY, float radius) {
    
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
            
            float dist = sqrt(sqr(x) + sqr(y));            
            if (dist <= radius) {
                float z = sqrt(sqr(radius) - sqr(dist));
                
                r_final = g_final = b_final = 0;
                
                // point light source
                for (int idx = 0; idx < pl_num; idx++) {
                    
                    // diffuse part
                    float pl_d[3] = {pl_pos[idx][0]*radius-x, pl_pos[idx][1]*radius-y, pl_pos[idx][2]*radius-z};
                    float pl_d_sum = sqrt(sqr(pl_d[0]) + sqr(pl_d[1]) + sqr(pl_d[2]));
                    float cos_theta = (pl_d[0]*x + pl_d[1]*y + pl_d[2]*z) / (pl_d_sum * radius);
                    r_final += kd[0] * max(0.0f, cos_theta) * pl_color[idx][0];
                    g_final += kd[1] * max(0.0f, cos_theta) * pl_color[idx][1];
                    b_final += kd[2] * max(0.0f, cos_theta) * pl_color[idx][2];
                    
                    // specular part
                    float specular = pow(max(0.0f, -pl_d[2]/pl_d_sum + 2*cos_theta*z/radius), sp);
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
                    float cos_theta = -(dl_dir[idx][0]*x + dl_dir[idx][1]*y + dl_dir[idx][2]*z) / (dl_d_sum * radius);
                    r_final += kd[0] * max(0.0f, cos_theta) * dl_color[idx][0];
                    g_final += kd[1] * max(0.0f, cos_theta) * dl_color[idx][1];
                    b_final += kd[2] * max(0.0f, cos_theta) * dl_color[idx][2];
                    
                    // specular part
                    float specular = pow(max(0.0f, dl_dir[idx][2]/dl_d_sum + 2*cos_theta*z/radius), sp);
                    r_final += ks[0] * specular * dl_color[idx][0];
                    g_final += ks[1] * specular * dl_color[idx][1];
                    b_final += ks[2] * specular * dl_color[idx][2];
                    
                    // ambient part
                    r_final += ka[0] * dl_color[idx][0];
                    g_final += ka[1] * dl_color[idx][1];
                    b_final += ka[2] * dl_color[idx][2];
                }
                
                setPixel(i, j, r_final, g_final, b_final);
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
    //circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);
    
    shade(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

    glFlush();
    glutSwapBuffers();					// swap buffers (we earlier set double buffer)

}

void myKeyboard(unsigned char key, int mouseX, int mouseY) {
    switch (key) {
        case ' ':
            exit(0);
            break;
            
        default:
            break;
    }
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

    kd[0] = 0.5;
    kd[1] = 0.5;
    kd[2] = 0.5;
    
    ks[0] = 0.5;
    ks[1] = 0.5;
    ks[2] = 0.5;
    sp = 2;
    
    ka[0] = 0.01;
    ka[1] = 0.01;
    ka[2] = 0.01;
    
    pl_num = 1;
    pl_pos[0][0] = 1;
    pl_pos[0][1] = 1;
    pl_pos[0][2] = 1;
    pl_pos[1][0] = 0;
    pl_pos[1][1] = -2;
    pl_pos[1][2] = -2;
    
    pl_color[0][0] = 1;
    pl_color[0][1] = 0;
    pl_color[0][2] = 0;
    pl_color[1][0] = 1;
    pl_color[1][1] = 1;
    pl_color[1][2] = 1;
    
    dl_num = 1;
    dl_dir[0][0] = -1;
    dl_dir[0][1] = -1;
    dl_dir[0][2] = -1;
    dl_dir[1][0] = 0;
    dl_dir[1][1] = -2;
    dl_dir[1][2] = -2;
    
    dl_color[0][0] = 0;
    dl_color[0][1] = 1;
    dl_color[0][2] = 0;
    dl_color[1][0] = 1;
    dl_color[1][1] = 1;
    dl_color[1][2] = 1;

    
  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

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








