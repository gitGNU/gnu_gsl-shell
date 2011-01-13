#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/gl.h> 
#include "func-plot.h" 

#ifndef M_PI
#  define M_PI 3.14159265
#endif

double
myf (GLfloat x, GLfloat y)
{
  return 0.3*exp(-2*(x*x+y*y)) * (cos(2*M_PI * x) + cos(2*M_PI * y));
}

void
set_normal (GLfloat *v1, GLfloat *v2)
{
  double x = v1[1]*v2[2] - v1[2]*v2[1];
  double y = v1[2]*v2[0] - v1[0]*v2[2];
  double z = v1[0]*v2[1] - v1[1]*v2[0];

  double n = sqrt(x*x+y*y+z*z);

  glNormal3f (x/n, y/n, z/n);
}

void plot2d (GLfloat *x, GLfloat *y, GLfloat *z, int nx, int ny)
{
  int i, j;
  
  for (j = 0; j < ny-1; j++)
    {
      glBegin( GL_QUAD_STRIP );

      glVertex3f( x[0], y[j],   z[j]   );
      glVertex3f( x[0], y[j+1], z[j+1] );

      for (i = 0; i < nx-1; i++)
	{
	  GLfloat v1[3], v2[3];

	  v1[0] = x[i+1] - x[i];
	  v1[1] = 0.0;
	  v1[2] = z[(i+1)*ny + j] - z[i*ny + j];

	  v2[0] = x[i+1] - x[i];
	  v2[1] = y[j+1] - y[j];
	  v2[2] = z[(i+1)*ny + j+1] - z[i*ny + j];

	  set_normal (v1, v2);

	  glVertex3f( x[i+1], y[j],   z[(i+1)*ny + j    ] );
	  glVertex3f( x[i+1], y[j+1], z[(i+1)*ny + (j+1)] );
	}

      glEnd();
    }
}


static GLfloat view_rotx=-70.0, view_roty= 0.0, view_rotz=20.0;
static GLfloat angle = 0.0;

static GLint fsurf_id;


void plot2d_draw( void )
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glPushMatrix();
   glRotatef( view_rotx, 1.0, 0.0, 0.0 );
   glRotatef( view_roty, 0.0, 1.0, 0.0 );
   glRotatef( view_rotz, 0.0, 0.0, 1.0 );

   glPushMatrix();
   //   glTranslatef( -3.0, -2.0, 0.0 );
   //   glRotatef( angle, 0.0, 0.0, 1.0 );
   glCallList(fsurf_id);
   glPopMatrix();

   glPopMatrix();
}


/* new window size or exposure */
void plot2d_reshape( int width, int height )
{
   GLfloat  h = (GLfloat) height / (GLfloat) width;
   GLfloat ff = 0.6;

   glViewport(0, 0, (GLint)width, (GLint)height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum( -ff, ff, -ff * h, ff * h, 15.0, 45.0 );
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, -40.0 );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


void plot2d_init( void )
{
   static GLfloat pos[4] = {5.0, 5.0, 5.0, 0.5 };
   static GLfloat brown[4] = {0.6, 0.3, 0.3, 1.0 };
   static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0 };
   static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0 };

   GLfloat x[32], y[32], z[32*32];
   GLfloat xmin = -1.0, xmax = 1.0;
   GLfloat ymin = -1.0, ymax = 1.0;

   int i, j, nx = 32, ny = 32;
   for (j = 0; j < ny; j++)
     {
       y[j] = ymin + (ymax - ymin) * j / (ny-1);

       for (i = 0; i < nx; i++)
	 {
	   if (j == 0)
	     x[i] = xmin + (xmax - xmin) * i / (nx-1);
	   
	   z[i*ny+j] = myf(x[i], y[j]);
	 }
     }

   glLightfv( GL_LIGHT0, GL_POSITION, pos );
   //   glEnable( GL_CULL_FACE );
   glEnable( GL_LIGHTING );
   glEnable( GL_LIGHT0 );
   glEnable( GL_DEPTH_TEST );

   /* make the gears */
   fsurf_id = glGenLists(1);
   glNewList(fsurf_id, GL_COMPILE);
   glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue );
   //   glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, green );
   plot2d(x, y, z, nx, ny);
   glEndList();

   glEnable( GL_NORMALIZE );
}
