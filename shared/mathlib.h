/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// mathlib.h

typedef float vec3_t[3];
typedef float vec5_t[5];

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

extern	vec3_t vec3_origin;
extern	int nanmask;

#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)
#define DEG2RAD(a) ((a) * (3.14159265358979323846f / 180.0f))
#define RAD2DEG(a) ((a) * (180.0f / 3.14159265358979323846f))

#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define Length(v) (sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]))
#define VectorCompare(a,b) ((a[0] == b[0]) & (a[1] == b[1]) & (a[2] == b[2]))

void	VectorSubtract (vec3_t a, vec3_t b, vec3_t c);
void	VectorAdd (vec3_t a, vec3_t b, vec3_t c);
void	VectorScale (vec3_t a, float b, vec3_t c);
void	VectorCopy (vec3_t a, vec3_t b);
void	VectorInverse (vec3_t a);
void	CrossProduct (vec3_t a, vec3_t b, vec3_t c);
void	VectorMA (vec3_t a, float s, vec3_t b, vec3_t c);

void	R_ConcatRotations (float in1[3][3], float in2[3][3], float out[3][3]);
void	R_ConcatTransforms (float in1[3][4], float in2[3][4], float out[3][4]);
void	FloorDivMod (double numer, double denom, int *quotient, int *rem);
int		GreatestCommonDivisor (int i1, int i2);
float	VectorNormalize(vec3_t v);
void	AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
int		BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct mplane_s *plane);
float	anglemod(float a);

#define BOX_ON_PLANE_SIDE(emins, emaxs, p)	\
	(((p)->type < 3)?						\
	(										\
		((p)->dist <= (emins)[(p)->type])?	\
			1								\
		:									\
		(									\
			((p)->dist >= (emaxs)[(p)->type])?\
				2							\
			:								\
				3							\
		)									\
	)										\
	:										\
		BoxOnPlaneSide( (emins), (emaxs), (p)))