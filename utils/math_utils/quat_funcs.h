
/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries' separate legal notices                             |
|******************************************************************************|
| Commonly-used maths structures and functions                                 |
| Simple-as-possible. No disgusting templates.                                 |
| Structs vec3, mat4, versor. just hold arrays of floats called "v","m","q",   |
| respectively. So, for example, to get values from a mat4 do: my_mat.m        |
| A versor is the proper name for a unit quaternion.                           |
| This is C++ because it's sort-of convenient to be able to use maths operators|
\******************************************************************************/

#ifndef FPS_STYLE_ROOM_QUAT_FUNCS_H
#define FPS_STYLE_ROOM_QUAT_FUNCS_H

#include <GL/gl.h>
#include "maths_funcs.h"

#define PITCH 0
#define YAW  1
#define ROLL  2

void create_versor (float* q, float a, float x, float y, float z);
void quat_to_mat4 (float* m, float* q) ;
void normalise_quat (float* q);
void mult_quat_quat (float* result, float* r, float* s);

void calculateRotationMatrix(GLfloat angle, mat4 *matrix, int type);
#endif //FPS_STYLE_ROOM_QUAT_FUNCS_H
