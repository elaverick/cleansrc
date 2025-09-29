/* math.c - C replacement for math.s (Quake)
   Works in 32-bit Visual Studio 2022 builds.
   Standard C only (no C++).
*/

#include "quakedef.h"

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#include <emmintrin.h> // SSE2 intrinsics

/* External vectors used by TransformVector (globals, defined elsewhere) */
extern float vright[3];
extern float vup[3];
extern float vpn[3];

/* ---------------------------------------------------------------------------
   CPU feature detection (runtime)
   -------------------------------------------------------------------------*/
static int cpu_supports_sse2(void)
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);            /* function 1 = feature info */
    return (cpuInfo[3] & (1 << 26)) != 0; /* bit 26 of EDX = SSE2 */
}

/* ---------------------------------------------------------------------------
   Invert24To16
   Matches asm behaviour from math.s:
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
   TransformVector
   Computes:
     out[0] = in dot vright
     out[1] = in dot vup
     out[2] = in dot vpn

   Runtime dispatch:
     - scalar fallback always available
     - SSE2 version used if CPU supports it
   -------------------------------------------------------------------------*/

   /* scalar fallback */
static void TransformVector_scalar(const float* in, float* out)
{
    out[0] = in[0] * vright[0] + in[1] * vright[1] + in[2] * vright[2];
    out[1] = in[0] * vup[0] + in[1] * vup[1] + in[2] * vup[2];
    out[2] = in[0] * vpn[0] + in[1] * vpn[1] + in[2] * vpn[2];
}

/* SSE2 version */
static void TransformVector_sse2(const float* in, float* out)
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

/* function pointer (set at init) */
static void (*TransformVector_impl)(const float*, float*) = TransformVector_scalar;

/* public entry point */
void TransformVector(const float* in, float* out)
{
    TransformVector_impl(in, out);
}

/* Init function to choose best implementation */
void InitMathRoutines(void)
{
    if (cpu_supports_sse2()) {
        TransformVector_impl = TransformVector_sse2;
        Con_Printf("SSE2 SIMD optimisations enabled.\n");
    }
    else {
        TransformVector_impl = TransformVector_scalar;
        Con_Printf("Using scalar fallback for TransformVector\n");
    }
}

/* ---------------------------------------------------------------------------
   BoxOnPlaneSide
   Signature (from Quake):
     int BoxOnPlaneSide(vec3_t emins, vec3_t emaxs, mplane_t *p)

   Behaviour:
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
