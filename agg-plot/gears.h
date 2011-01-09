
#include <GL/gl.h> 

#include "defs.h"

__BEGIN_DECLS

extern void gear( GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
		  GLint teeth, GLfloat tooth_depth );

extern void gear_draw( void );

extern void gear_reshape( int width, int height );
extern void gear_init( void );

__END_DECLS
