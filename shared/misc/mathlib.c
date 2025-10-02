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
// mathlib.c -- math primitives

#include "quakedef.h"
#include <math.h>
#include <emmintrin.h> // SSE2 intrinsics
#include <xmmintrin.h>  // SSE
#include <smmintrin.h>  // SSE4.1 (for _mm_dp_ps)

void Sys_Error (char *error, ...);

vec3_t vec3_origin = {0,0,0};
int nanmask = 255<<23;

/*-----------------------------------------------------------------*/

void ProjectPointOnPlane( vec3_t dst, const vec3_t p, const vec3_t normal )
{
	float d;
	vec3_t n;
	float inv_denom;

	inv_denom = 1.0F / DotProduct( normal, normal );

	d = DotProduct( normal, p ) * inv_denom;

	n[0] = normal[0] * inv_denom;
	n[1] = normal[1] * inv_denom;
	n[2] = normal[2] * inv_denom;

	dst[0] = p[0] - d * n[0];
	dst[1] = p[1] - d * n[1];
	dst[2] = p[2] - d * n[2];
}

/*
** assumes "src" is normalized
*/
void PerpendicularVector( vec3_t dst, const vec3_t src )
{
	int	pos;
	int i;
	float minelem = 1.0F;
	vec3_t tempvec;

	/*
	** find the smallest magnitude axially aligned vector
	*/
	for ( pos = 0, i = 0; i < 3; i++ )
	{
		if ( fabs( src[i] ) < minelem )
		{
			pos = i;
			minelem = fabs( src[i] );
		}
	}
	tempvec[0] = tempvec[1] = tempvec[2] = 0.0F;
	tempvec[pos] = 1.0F;

	/*
	** project the point onto the plane defined by src
	*/
	ProjectPointOnPlane( dst, tempvec, src );

	/*
	** normalize the result
	*/
	VectorNormalize( dst );
}

#ifdef _WIN32
#pragma optimize( "", off )
#endif

void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees )
{
	float	m[3][3];
	float	im[3][3];
	float	zrot[3][3];
	float	tmpmat[3][3];
	float	rot[3][3];
	int	i;
	vec3_t vr, vup, vf;

	vf[0] = dir[0];
	vf[1] = dir[1];
	vf[2] = dir[2];

	PerpendicularVector( vr, dir );
	CrossProduct( vr, vf, vup );

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy( im, m, sizeof( im ) );

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset( zrot, 0, sizeof( zrot ) );
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

	zrot[0][0] = cos( DEG2RAD( degrees ) );
	zrot[0][1] = sin( DEG2RAD( degrees ) );
	zrot[1][0] = -sin( DEG2RAD( degrees ) );
	zrot[1][1] = cos( DEG2RAD( degrees ) );

	R_ConcatRotations( m, zrot, tmpmat );
	R_ConcatRotations( tmpmat, im, rot );

	for ( i = 0; i < 3; i++ )
	{
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
	}
}

#ifdef _WIN32
#pragma optimize( "", on )
#endif

/*-----------------------------------------------------------------*/


float anglemod(float a)
{
	a = (360.0/65536) * ((int)(a*(65536/360.0)) & 65535);
	return a;
}

/*
==================
BOPS_Error

Split out like this for ASM to call.
==================
*/
void BOPS_Error (void)
{
	Sys_Error ("BoxOnPlaneSide:  Bad signbits");
}


#if	!id386

/*
==================
BoxOnPlaneSide

Returns 1, 2, or 1 + 2
==================
*/
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, mplane_t *p)
{
	float	dist1, dist2;
	int		sides;
	
// general case
	switch (p->signbits)
	{
	case 0:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 1:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 2:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 3:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 4:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 5:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 6:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	case 7:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;		// shut up compiler
		BOPS_Error ();
		break;
	}

	sides = 0;
	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	return sides;
}

#endif


void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[YAW] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	forward[0] = cp*cy;
	forward[1] = cp*sy;
	forward[2] = -sp;
	right[0] = (-1*sr*sp*cy+-1*cr*-sy);
	right[1] = (-1*sr*sp*sy+-1*cr*cy);
	right[2] = -1*sr*cp;
	up[0] = (cr*sp*cy+-sr*-sy);
	up[1] = (cr*sp*sy+-sr*cy);
	up[2] = cr*cp;
}

double sqrt(double x);

float VectorNormalize (vec3_t v)
{
	__m128 vec = _mm_set_ps(0.0f, v[2], v[1], v[0]); // pad with 0
	__m128 squared = _mm_mul_ps(vec, vec);

	// Sum components (horizontal add manually)
	__m128 shuf1 = _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(2, 1, 0, 3));
	__m128 sums = _mm_add_ps(squared, shuf1);
	__m128 shuf2 = _mm_shuffle_ps(sums, sums, _MM_SHUFFLE(1, 0, 3, 2));
	__m128 length_sq = _mm_add_ps(sums, shuf2);

	// sqrt(length_sq)
	float length = _mm_cvtss_f32(_mm_sqrt_ss(length_sq));

	if (length > 0.0f) {
		__m128 inv_len = _mm_rsqrt_ps(length_sq); // fast approximation
		vec = _mm_mul_ps(vec, inv_len);
		_mm_store_ss(&v[0], vec);
		_mm_store_ss(&v[1], _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 1, 1, 1)));
		_mm_store_ss(&v[2], _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 2, 2, 2)));
	}

	return length;
}

void VectorSubtract (vec3_t a, vec3_t b, vec3_t c)
{
	c[0]	= a[0] - b[0];
	c[1]	= a[1] - b[1];
	c[2]	= a[2] - b[2];
}

void VectorAdd (vec3_t a, vec3_t b, vec3_t c)
{
	c[0]	= a[0] + b[0];
	c[1]	= a[1] + b[1];
	c[2]	= a[2] + b[2];
}

void VectorScale (vec3_t a, float b, vec3_t c)
{
	c[0]	= a[0] * b;
	c[1]	= a[1] * b;
	c[2]	= a[2] * b;
}

void VectorCopy (vec3_t a, vec3_t b)
{
	b[0]	= a[0];
	b[1]	= a[1];
	b[2]	= a[2];
}

void VectorInverse (vec3_t a)
{
	a[0]	= -a[0];
	a[1]	= -a[1];
	a[2]	= -a[2];
}

void CrossProduct (vec3_t a, vec3_t b, vec3_t c)
{
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
}

void VectorMA (vec3_t a, float s, vec3_t b, vec3_t c)
{
	c[0]	= a[0] + (s * b[0]);
	c[1]	= a[1] + (s * b[1]);
	c[2]	= a[2] + (s * b[2]);
}

/*
================
R_ConcatRotations
================
*/
void R_ConcatRotations (float in1[3][3], float in2[3][3], float out[3][3])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
}


/*
================
R_ConcatTransforms
================
*/
void R_ConcatTransforms (float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}


/*
===================
FloorDivMod

Returns mathematically correct (floor-based) quotient and remainder for
numer and denom, both of which should contain no fractional part. The
quotient must fit in 32 bits.
====================
*/

void FloorDivMod (double numer, double denom, int *quotient,
		int *rem)
{
	int		q, r;
	double	x;

	if (denom <= 0.0)
		Sys_Error ("FloorDivMod: bad denominator %d\n", denom);

	if (numer >= 0.0)
	{

		x = floor(numer / denom);
		q = (int)x;
		r = (int)floor(numer - (x * denom));
	}
	else
	{
	//
	// perform operations with positive values, and fix mod to make floor-based
	//
		x = floor(-numer / denom);
		q = -(int)x;
		r = (int)floor(-numer - (x * denom));
		if (r != 0)
		{
			q--;
			r = (int)denom - r;
		}
	}

	*quotient = q;
	*rem = r;
}


/*
===================
GreatestCommonDivisor
====================
*/
int GreatestCommonDivisor (int i1, int i2)
{
	if (i1 > i2)
	{
		if (i2 == 0)
			return (i1);
		return GreatestCommonDivisor (i2, i1 % i2);
	}
	else
	{
		if (i1 == 0)
			return (i2);
		return GreatestCommonDivisor (i1, i2 % i1);
	}
}

/* ---------------------------------------------------------------------------
   Invert24To16
   Matches asm behaviour:
	 - if val <= 256 return 0xFFFFFFFF
	 - else return floor(256 / val)
   -------------------------------------------------------------------------*/
unsigned int Invert24To16(unsigned int val)
{
	if (val <= 0x100u) {
		return 0xFFFFFFFFu;
	}
	return (unsigned int)(0x100u / val);
}

/* ---------------------------------------------------------------------------
   TransformVector (SSE2 always)
   Computes:
	 out[0] = in dot vright
	 out[1] = in dot vup
	 out[2] = in dot vpn
   -------------------------------------------------------------------------*/
void TransformVector(const float* in, float* out)
{
	__m128 vin = _mm_set_ps(0.0f, in[2], in[1], in[0]);

	__m128 vr = _mm_set_ps(0.0f, vright[2], vright[1], vright[0]);
	__m128 vu = _mm_set_ps(0.0f, vup[2], vup[1], vup[0]);
	__m128 vp = _mm_set_ps(0.0f, vpn[2], vpn[1], vpn[0]);

	__m128 mul, sh, add;

	mul = _mm_mul_ps(vin, vr);
	sh = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(3, 0, 2, 1));
	add = _mm_add_ps(mul, sh);
	sh = _mm_shuffle_ps(add, add, _MM_SHUFFLE(3, 0, 0, 2));
	out[0] = _mm_cvtss_f32(_mm_add_ps(add, sh));

	mul = _mm_mul_ps(vin, vu);
	sh = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(3, 0, 2, 1));
	add = _mm_add_ps(mul, sh);
	sh = _mm_shuffle_ps(add, add, _MM_SHUFFLE(3, 0, 0, 2));
	out[1] = _mm_cvtss_f32(_mm_add_ps(add, sh));

	mul = _mm_mul_ps(vin, vp);
	sh = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(3, 0, 2, 1));
	add = _mm_add_ps(mul, sh);
	sh = _mm_shuffle_ps(add, add, _MM_SHUFFLE(3, 0, 0, 2));
	out[2] = _mm_cvtss_f32(_mm_add_ps(add, sh));
}

/* ---------------------------------------------------------------------------
   BoxOnPlaneSide
   Quake behaviour:
	 Returns 1 if box is in front of plane,
			 2 if box is behind plane,
			 3 if box straddles.
   -------------------------------------------------------------------------*/
int BoxOnPlaneSide(vec3_t emins, vec3_t emaxs, mplane_t* p)
{
	const float* n = p->normal;

	float dist1 = (n[0] >= 0.0f ? n[0] * emaxs[0] : n[0] * emins[0])
		+ (n[1] >= 0.0f ? n[1] * emaxs[1] : n[1] * emins[1])
		+ (n[2] >= 0.0f ? n[2] * emaxs[2] : n[2] * emins[2]);

	float dist2 = (n[0] >= 0.0f ? n[0] * emins[0] : n[0] * emaxs[0])
		+ (n[1] >= 0.0f ? n[1] * emins[1] : n[1] * emaxs[1])
		+ (n[2] >= 0.0f ? n[2] * emins[2] : n[2] * emaxs[2]);

	int sides = 0;
	if (dist1 >= p->dist) sides = 1;
	if (dist2 < p->dist) sides |= 2;

	return sides;
}

int SV_HullPointContents(hull_t* hull, int num, const float p[3])
{
	while (num >= 0)
	{
		dclipnode_t* node = &hull->clipnodes[num];
		mplane_t* plane = &hull->planes[node->planenum];

		float d;

		// Axis-aligned plane optimization
		if (plane->type < 3) {
			d = p[plane->type] - plane->dist;
		}
		else {
			// Use SSE4.1 dot product: dot(normal, p) - dist
			__m128 normal = _mm_setr_ps(plane->normal[0], plane->normal[1], plane->normal[2], 0.0f);
			__m128 point = _mm_setr_ps(p[0], p[1], p[2], 0.0f);
			__m128 dot = _mm_dp_ps(normal, point, 0x71);  // 0111 0001 = xyz, result in .x
			d = _mm_cvtss_f32(dot) - plane->dist;
		}

		// Choose child node based on plane test
		num = (d < 0.0f) ? node->children[1] : node->children[0];
	}

	return num;  // Final leaf node content index
}
