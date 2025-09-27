#ifndef WASP_H
#error This module depends on wasp.h
#endif

#ifndef WASP_MATH_H
#define WASP_MATH_H

/*
  CONSTANTS
*/

#define HalfPi  1.57079632679f
#define Pi      3.14159265359f
#define Tau     6.28318530718f
#define Epsilon 1.1920929E-7f

// TODO: Replace with 0x7fc00000, but verify
#define NaN     (0.0f*(0.0f/0.0f))

#define V3Forward  v3(1.0f, 0.0f, 0.0f)
#define V3Right    v3(0.0f, 1.0f, 0.0f)
#define V3Up       v3(0.0f, 0.0f, 1.0f)
#define V3Backward v3(-1.0f, 0.0f, 0.0f)
#define V3Left     v3(0.0f, -1.0f, 0.0f)
#define V3Down     v3(0.0f, 0.0f, -1.0f)

#define D2R (Pi / 180.0f)
#define R2D (180.0f / Pi)

/*
  TYPES
*/

union v2
{
    struct
    {
        f32 X, Y;
    };
    
    struct
    {
        f32 U, V;
    };
};

union v3
{
    struct
    {
        f32 X, Y, Z;
    };
    
    struct
    {
        f32 R, G, B;
    };
    
    struct
    {
        f32 Pitch, Yaw, Roll;
    };
};

union v4
{
    struct
    {
        f32 X, Y, Z, W;
    };
    
    struct
    {
        f32 R, G, B, A;
    };
    
    // __m128 Values;
};

union m4
{
    struct
    {
        f32 M00, M01, M02, M03,
        M10, M11, M12, M13,
        M20, M21, M22, M23,
        M30, M31, M32, M33;
    };
    
    // __m128 Rows[4];
};

union m2
{
    struct
    {
        f32 M00, M01,
            M10, M11;
    };
    
    // __m128 Rows[4];
};

/*
  GLOBALS
*/

/*
  FUNCTIONS
*/

internal inline f32 Floor(f32 Value);
internal inline f32 Ceil(f32 Value);
internal inline f32 Round(f32 Value);
internal inline f32 Mod(f32 Value, f32 Divisor);
internal inline f32 Sin(f32 Value);
internal inline f32 Cos(f32 Value);
internal inline f32 Tan(f32 Value);
internal inline void SinCos(f32 Value, f32 *Sin, f32 *Cos);
internal inline f32 Atan2(f32 A, f32 B);
internal inline f32 Asin(f32 Value);
internal inline f32 Acos(f32 Value);
internal inline f32 Sqrt(f32 Value);
internal inline f32 Power(f32 Value, f32 Exponent);
internal inline f32 Exponent(f32 Value);
internal inline f32 Logarithm(f32 Value);
internal inline f32 Cosh(f32 Value);
internal inline f32 Sech(f32 Value);
internal inline f32 Sigmoid(f32 Value);
internal inline f32 SigmoidDerivative(f32 Value);
internal inline f32 Tanh(f32 Value);
internal inline f32 TanhDerivative(f32 Value);
internal inline f32 ReLU(f32 Value);
internal inline f32 ReLUDerivative(f32 Value);

internal inline v2 V2(f32 X, f32 Y);
internal inline v3 V3(f32 X, f32 Y, f32 Z);
internal inline v4 V4(f32 X, f32 Y, f32 Z, f32 W);

internal inline m4 M4(f32 M00, f32 M01, f32 M02, f32 M03,
                      f32 M10, f32 M11, f32 M12, f32 M13,
                      f32 M20, f32 M21, f32 M22, f32 M23,
                      f32 M30, f32 M31, f32 M32, f32 M33);

internal inline v2 operator+(v2 A, v2 B);
internal inline v3 operator+(v3 A, v3 B);
internal inline v4 operator+(v4 A, v4 B);

internal inline v2 operator-(v2 A, v2 B);
internal inline v3 operator-(v3 A, v3 B);
internal inline v4 operator-(v4 A, v4 B);

internal inline v2 operator*(v2 A, f32 B);
internal inline v3 operator*(v3 A, f32 B);
internal inline v4 operator*(v4 A, f32 B);

internal inline v2 operator*(f32 A, v2 B);
internal inline v3 operator*(f32 A, v3 B);
internal inline v4 operator*(f32 A, v4 B);

internal inline v2 operator/(v2 A, f32 B);
internal inline v3 operator/(v3 A, f32 B);
internal inline v4 operator/(v4 A, f32 B);

internal inline v2 &operator+=(v2 &A, v2 B);
internal inline v3 &operator+=(v3 &A, v3 B);
internal inline v4 &operator+=(v4 &A, v4 B);

internal inline v2 &operator-=(v2 &A, v2 B);
internal inline v3 &operator-=(v3 &A, v3 B);
internal inline v4 &operator-=(v4 &A, v4 B);

internal inline v2 &operator*=(v2 &A, f32 B);
internal inline v3 &operator*=(v3 &A, f32 B);
internal inline v4 &operator*=(v4 &A, f32 B);

internal inline v2 &operator/=(v2 &A, f32 B);
internal inline v3 &operator/=(v3 &A, f32 B);
internal inline v4 &operator/=(v4 &A, f32 B);

internal inline m4 operator*(m4 A, m4 B);
internal inline v4 operator*(m4 A, v4 B);

internal inline m2 operator*(m2 A, m2 B);
internal inline v2 operator*(m2 A, v2 B);

internal inline m2 operator-(m2 A, m2 B);
internal inline m2 operator+(m2 A, m2 B);
internal inline m2 operator-(m2 A);

internal inline m4 Transpose(m4 A);
internal inline m2 Transpose(m2 A);

internal inline m2 Inverse(m2 A);
internal inline m2 EigenVectors(m2 A, v2 *B);
internal inline m2 M2Identity();

internal inline f32 Distance(v2 A, v2 B);
internal inline f32 Distance(v3 A, v3 B);
internal inline f32 Distance(v4 A, v4 B);

internal inline f32 Length(v2 A);
internal inline f32 Length(v3 A);
internal inline f32 Length(v4 A);

internal inline v2 Normalize(v2 A);
internal inline v3 Normalize(v3 A);
internal inline v4 Normalize(v4 A);

internal inline f32 DistanceSquare(v2 A, v2 B);
internal inline f32 DistanceSquare(v3 A, v3 B);
internal inline f32 DistanceSquare(v4 A, v4 B);

internal inline f32 LengthSquare(v2 A);
internal inline f32 LengthSquare(v3 A);
internal inline f32 LengthSquare(v4 A);

internal inline f32 Lerp(f32 A, f32 B, f32 T);
internal inline v2 Lerp(v2 A, v2 B, f32 T);
internal inline v3 Lerp(v3 A, v3 B, f32 T);
internal inline v4 Lerp(v4 A, v4 B, f32 T);

internal inline f32 Dot(v2 A, v2 B);
internal inline f32 Dot(v3 A, v3 B);
internal inline f32 Dot(v4 A, v4 B);

internal inline v2 Hadamard(v2 A, v2 B);
internal inline v3 Hadamard(v3 A, v3 B);
internal inline v4 Hadamard(v4 A, v4 B);

internal inline v3 Cross(v3 A, v3 B);

internal inline f32 Determinant(v2 A, v2 B);

/*
  IMPLEMENTATION
*/

#if Platform_LINUX || Platform_DARWIN
#include <x86intrin.h>
#elif Platform_WIN32
#include <intrin.h>
#elif Platform_WASM
// NOTE: We don't support intrinsics here yet!
#else
#error We don't know what to include for intrinsics on this platform!
#endif

//
// NOTE:
// http://gruntthepeon.free.fr/ssemath/
// https://github.com/to-miz/sse_mathfun_extension
//

// TODO: Instead paste intrinsics from https://github.com/microsoft/DirectXMath/blob/main/Inc/DirectXMathVector.inl, or make our own...

#define USE_SSE2
#include "sse_mathfun_extension.h"

internal inline f32
Floor(f32 Value)
{
    f32 Result = _mm_cvtss_f32(_mm_floor_ss(_mm_setzero_ps(), _mm_set_ss(Value)));
    return(Result);
}

internal inline f32
Ceil(f32 Value)
{
    f32 Result = _mm_cvtss_f32(_mm_ceil_ss(_mm_setzero_ps(), _mm_set_ss(Value)));
    return(Result);
}

internal inline f32
Round(f32 Value)
{
    f32 Result = _mm_cvtss_f32(_mm_round_ss(_mm_setzero_ps(), _mm_set_ss(Value), _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC));
    return(Result);
}

internal inline f32
Mod(f32 Value, f32 Divisor)
{
    f32 Result = Value - Floor(Value / Divisor) * Divisor;
    return(Result);
}

internal inline f32
Sin(f32 Value)
{
    f32 Result = _mm_cvtss_f32(sin_ps(_mm_set_ss(Value)));
    return(Result);
}

internal inline f32
Cos(f32 Value)
{
    f32 Result = _mm_cvtss_f32(cos_ps(_mm_set_ss(Value)));
    return(Result);
}

internal inline f32
Tan(f32 Value)
{
    f32 Result = _mm_cvtss_f32(tan_ps(_mm_set_ss(Value)));
    return(Result);
}

internal inline void
SinCos(f32 Value, f32 *Sin, f32 *Cos)
{
    __m128 XMM0, XMM1;
    sincos_ps(_mm_set_ss(Value), &XMM0, &XMM1);
    *Sin = _mm_cvtss_f32(XMM0);
    *Cos = _mm_cvtss_f32(XMM1);
}

internal inline f32
Atan2(f32 A, f32 B)
{
    f32 Result = _mm_cvtss_f32(atan2_ps(_mm_set_ss(A), _mm_set_ss(B)));
    return(Result);
}

internal inline f32
Asin(f32 Value)
{
    __m128 XMM0 = _mm_set_ss(Value);
    __m128 XMM1 = _mm_sqrt_ss(_mm_sub_ss(_mm_set_ss(1.0f), _mm_mul_ss(XMM0, XMM0)));
    f32 Result = _mm_cvtss_f32(atan2_ps(XMM0, XMM1));
    return(Result);
}

internal inline f32
Acos(f32 Value)
{
    __m128 XMM0 = _mm_set_ss(Value);
    __m128 XMM1 = _mm_sqrt_ss(_mm_sub_ss(_mm_set_ss(1.0f), _mm_mul_ss(XMM0, XMM0)));
    f32 Result = _mm_cvtss_f32(atan2_ps(XMM1, XMM0));
    return(Result);
}

internal inline f32
Sqrt(f32 Value)
{
    f32 Result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(Value)));
    return(Result);
}

internal inline f32
Square(f32 Value)
{  
    __m128 XMM0 = _mm_set_ss(Value);
    f32 Result = _mm_cvtss_f32(_mm_mul_ss(XMM0, XMM0));
    return(Result);
}

internal inline f32
Power(f32 Value, f32 Exponent)
{
    f32 Result = _mm_cvtss_f32(exp_ps(_mm_mul_ss(_mm_set_ss(Exponent), log_ps(_mm_set_ss(Value)))));
    return(Result);
}

internal inline f32
Exponent(f32 Value)
{
    f32 Result = _mm_cvtss_f32(exp_ps(_mm_set_ss(Value)));
    return(Result);
}

internal inline f32
Logarithm(f32 Value)
{
    f32 Result = _mm_cvtss_f32(log_ps(_mm_set_ss(Value)));
    return(Result);
}

internal inline f32
Cosh(f32 Value)
{
    __m128 XMM0 = _mm_set_ss(Value);
    __m128 XMM1 = _mm_div_ss(_mm_add_ss(exp_ps(XMM0), exp_ps(_mm_mul_ss(_mm_set_ss(-1.0f), XMM0))), _mm_set_ss(2.0f));
    f32 Result = _mm_cvtss_f32(XMM1);
    return(Result);
}

internal inline f32
Sech(f32 Value)
{
    __m128 XMM0 = _mm_set_ss(Value);
    __m128 XMM1 = _mm_div_ss(_mm_add_ss(exp_ps(XMM0), exp_ps(_mm_mul_ss(_mm_set_ss(-1.0f), XMM0))), _mm_set_ss(2.0f));
    __m128 XMM2 = _mm_rcp_ss(XMM1);
    
    f32 Result = _mm_cvtss_f32(XMM2);
    return(Result);
}

internal inline f32
Sigmoid(f32 Value)
{
    __m128 XMM0 = exp_ps(_mm_set_ss(-Value));
    __m128 XMM1 = _mm_add_ss(_mm_set_ss(1), XMM0);
    __m128 XMM2 = _mm_rcp_ss(XMM1);
    
    f32 Result = _mm_cvtss_f32(XMM2);
    return(Result);
}

internal inline f32
SigmoidDerivative(f32 Value)
{
    __m128 XMM0 = exp_ps(_mm_set_ss(-Value));
    __m128 XMM1 = _mm_add_ss(_mm_set_ss(1), XMM0);
    __m128 XMM2 = _mm_mul_ss(XMM1, XMM1);
    __m128 XMM3 = _mm_div_ss(XMM0, XMM2);
    
    f32 Result = _mm_cvtss_f32(XMM3);
    return(Result);
}

internal inline f32
Tanh(f32 Value)
{
    __m128 XMM0 = _mm_set_ss(Value);
    __m128 XMM1 = exp_ps(XMM0);
    __m128 XMM2 = exp_ps(_mm_mul_ss(_mm_set_ss(-1.0f), XMM0));
    __m128 XMM3 = _mm_div_ss(_mm_sub_ss(XMM1, XMM2), _mm_add_ss(XMM1, XMM2));
    
    f32 Result = _mm_cvtss_f32(XMM3);
    return(Result);
}

internal inline f32
TanhDerivative(f32 Value)
{
    __m128 XMM0 = _mm_set_ss(Value);
    __m128 XMM1 = exp_ps(XMM0);
    __m128 XMM2 = exp_ps(_mm_mul_ss(_mm_set_ss(-1.0f), XMM0));
    __m128 XMM3 = _mm_div_ss(_mm_sub_ss(XMM1, XMM2), _mm_add_ss(XMM1, XMM2));
    __m128 XMM4 = _mm_sub_ss(_mm_set_ss(1.0f), _mm_mul_ss(XMM3, XMM3));
    
    f32 Result = _mm_cvtss_f32(XMM4);
    return(Result);
}

internal inline f32
ReLU(f32 Value)
{
    f32 Result = Max(0.0f, Value);
    return(Result);
}

internal inline f32
ReLUDerivative(f32 Value)
{
    f32 Result = Value < 0.0f ? 0.0f : 1.0f;
    return(Result);
}

internal inline v2
V2(f32 X, f32 Y)
{
    v2 Result;
    Result.X = X;
    Result.Y = Y;
    return(Result);
}

internal inline v3
V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    return(Result);
}

internal inline v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    Result.W = W;
    return(Result);
}

internal inline m4
M4(f32 M00, f32 M01, f32 M02, f32 M03,
   f32 M10, f32 M11, f32 M12, f32 M13,
   f32 M20, f32 M21, f32 M22, f32 M23,
   f32 M30, f32 M31, f32 M32, f32 M33)
{
    m4 Result;
    
    Result.M00 = M00;
    Result.M01 = M01;
    Result.M02 = M02;
    Result.M03 = M03;
    
    Result.M10 = M10;
    Result.M11 = M11;
    Result.M12 = M12;
    Result.M13 = M13;
    
    Result.M20 = M20;
    Result.M21 = M21;
    Result.M22 = M22;
    Result.M23 = M23;
    
    Result.M30 = M30;
    Result.M31 = M31;
    Result.M32 = M32;
    Result.M33 = M33;
    
    return(Result);
}

internal inline m2
M2(f32 M00, f32 M01,
   f32 M10, f32 M11)
{
    m2 Result;
    
    Result.M00 = M00;
    Result.M01 = M01;
    Result.M10 = M10;
    Result.M11 = M11;
    
    return(Result);
}

internal inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return(Result);
}

internal inline v3
operator+(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    return(Result);
}

internal inline v4
operator+(v4 A, v4 B)
{
    v4 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    Result.W = A.W + B.W;
    return(Result);
}

internal inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return(Result);
}

internal inline v3
operator-(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    return(Result);
}

internal inline v4
operator-(v4 A, v4 B)
{
    v4 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    Result.W = A.W - B.W;
    return(Result);
}

internal inline v2
operator*(v2 A, f32 B)
{
    v2 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    return(Result);
}

internal inline v3
operator*(v3 A, f32 B)
{
    v3 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    Result.Z = A.Z * B;
    return(Result);
}

internal inline v4
operator*(v4 A, f32 B)
{
    v4 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    Result.Z = A.Z * B;
    Result.W = A.W * B;
    return(Result);
}

internal inline v2
operator*(f32 A, v2 B)
{
    v2 Result;
    Result.X = B.X * A;
    Result.Y = B.Y * A;
    return(Result);
}

internal inline v3
operator*(f32 A, v3 B)
{
    v3 Result;
    Result.X = B.X * A;
    Result.Y = B.Y * A;
    Result.Z = B.Z * A;
    return(Result);
}

internal inline v4
operator*(f32 A, v4 B)
{
    v4 Result;
    Result.X = B.X * A;
    Result.Y = B.Y * A;
    Result.Z = B.Z * A;
    Result.W = B.W * A;
    return(Result);
}

internal inline v2
operator/(v2 A, f32 B)
{
    v2 Result;
    Result.X = A.X / B;
    Result.Y = A.Y / B;
    return(Result);
}

internal inline v3
operator/(v3 A, f32 B)
{
    v3 Result;
    Result.X = A.X / B;
    Result.Y = A.Y / B;
    Result.Z = A.Z / B;
    return(Result);
}

internal inline v4
operator/(v4 A, f32 B)
{
    v4 Result;
    Result.X = A.X / B;
    Result.Y = A.Y / B;
    Result.Z = A.Z / B;
    Result.W = A.W / B;
    return(Result);
}

internal inline v2 &
operator+=(v2 &A, v2 B)
{
    A.X += B.X;
    A.Y += B.Y;
    
    return(A);
}

internal inline v3 &
operator+=(v3 &A, v3 B)
{
    A.X += B.X;
    A.Y += B.Y;
    A.Z += B.Z;
    
    return(A);
}

internal inline v4 &
operator+=(v4 &A, v4 B)
{
    A.X += B.X;
    A.Y += B.Y;
    A.Z += B.Z;
    A.W += B.W;
    
    return(A);
}

internal inline v2 &
operator-=(v2 &A, v2 B)
{
    A.X -= B.X;
    A.Y -= B.Y;
    
    return(A);
}

internal inline v3 &
operator-=(v3 &A, v3 B)
{
    A.X -= B.X;
    A.Y -= B.Y;
    A.Z -= B.Z;
    
    return(A);
}

internal inline v4 &
operator-=(v4 &A, v4 B)
{
    A.X -= B.X;
    A.Y -= B.Y;
    A.Z -= B.Z;
    A.W -= B.W;
    
    return(A);
}

internal inline v2 &
operator*=(v2 &A, f32 B)
{
    A.X *= B;
    A.Y *= B;
    
    return(A);
}

internal inline v3 &
operator*=(v3 &A, f32 B)
{
    A.X *= B;
    A.Y *= B;
    A.Z *= B;
    
    return(A);
}

internal inline v4 &
operator*=(v4 &A, f32 B)
{
    A.X *= B;
    A.Y *= B;
    A.Z *= B;
    A.W *= B;
    
    return(A);
}

internal inline v2 &
operator/=(v2 &A, f32 B)
{
    A.X /= B;
    A.Y /= B;
    
    return(A);
}

internal inline v3 &
operator/=(v3 &A, f32 B)
{
    A.X /= B;
    A.Y /= B;
    A.Z /= B;
    
    return(A);
}

internal inline v4 &
operator/=(v4 &A, f32 B)
{
    A.X /= B;
    A.Y /= B;
    A.Z /= B;
    A.W /= B;
    
    return(A);
}

internal inline m4
operator*(m4 A, m4 B)
{
    m4 Result;

    Result.M00 = A.M00 * B.M00 + A.M01 * B.M10 + A.M02 * B.M20 + A.M03 * B.M30;
    Result.M01 = A.M00 * B.M01 + A.M01 * B.M11 + A.M02 * B.M21 + A.M03 * B.M31;
    Result.M02 = A.M00 * B.M02 + A.M01 * B.M12 + A.M02 * B.M22 + A.M03 * B.M32;
    Result.M03 = A.M00 * B.M03 + A.M01 * B.M13 + A.M02 * B.M23 + A.M03 * B.M33;

    Result.M10 = A.M10 * B.M00 + A.M11 * B.M10 + A.M12 * B.M20 + A.M13 * B.M30;
    Result.M11 = A.M10 * B.M01 + A.M11 * B.M11 + A.M12 * B.M21 + A.M13 * B.M31;
    Result.M12 = A.M10 * B.M02 + A.M11 * B.M12 + A.M12 * B.M22 + A.M13 * B.M32;
    Result.M13 = A.M10 * B.M03 + A.M11 * B.M13 + A.M12 * B.M23 + A.M13 * B.M33;

    Result.M20 = A.M20 * B.M00 + A.M21 * B.M10 + A.M22 * B.M20 + A.M23 * B.M30;
    Result.M21 = A.M20 * B.M01 + A.M21 * B.M11 + A.M22 * B.M21 + A.M23 * B.M31;
    Result.M22 = A.M20 * B.M02 + A.M21 * B.M12 + A.M22 * B.M22 + A.M23 * B.M32;
    Result.M23 = A.M20 * B.M03 + A.M21 * B.M13 + A.M22 * B.M23 + A.M23 * B.M33;

    Result.M30 = A.M30 * B.M00 + A.M31 * B.M10 + A.M32 * B.M20 + A.M33 * B.M30;
    Result.M31 = A.M30 * B.M01 + A.M31 * B.M11 + A.M32 * B.M21 + A.M33 * B.M31;
    Result.M32 = A.M30 * B.M02 + A.M31 * B.M12 + A.M32 * B.M22 + A.M33 * B.M32;
    Result.M33 = A.M30 * B.M03 + A.M31 * B.M13 + A.M32 * B.M23 + A.M33 * B.M33;

    return(Result);
}

internal inline v4
operator*(m4 A, v4 B)
{
    v4 Result;

    Result.X = A.M00 * B.X + A.M01 * B.Y + A.M02 * B.Z * A.M03 * B.W;
    Result.Y = A.M10 * B.X + A.M11 * B.Y + A.M12 * B.Z * A.M13 * B.W;
    Result.Z = A.M20 * B.X + A.M21 * B.Y + A.M22 * B.Z * A.M23 * B.W;
    Result.W = A.M30 * B.X + A.M31 * B.Y + A.M32 * B.Z * A.M33 * B.W;

    return(Result);
}

internal inline m2
operator*(m2 A, m2 B)
{
    m2 Result;

    Result.M00 = A.M00 * B.M00 + A.M01 * B.M10;
    Result.M01 = A.M00 * B.M01 + A.M01 * B.M11;
    Result.M10 = A.M10 * B.M00 + A.M11 * B.M10;
    Result.M11 = A.M10 * B.M01 + A.M11 * B.M11;

    return(Result);
}

internal inline v2
operator*(m2 A, v2 B)
{
    v2 Result;

    Result.X = A.M00 * B.X + A.M01 * B.Y;
    Result.Y = A.M10 * B.X + A.M11 * B.Y;

    return(Result);
}

internal inline m4
Transpose(m4 A)
{
    m4 Result;

    Result.M00 = Result.M00;
    Result.M01 = Result.M10;
    Result.M02 = Result.M20;
    Result.M03 = Result.M30;

    Result.M10 = Result.M01;
    Result.M11 = Result.M11;
    Result.M12 = Result.M21;
    Result.M13 = Result.M31;

    Result.M20 = Result.M02;
    Result.M21 = Result.M12;
    Result.M22 = Result.M22;
    Result.M23 = Result.M32;

    Result.M30 = Result.M03;
    Result.M31 = Result.M13;
    Result.M32 = Result.M23;
    Result.M33 = Result.M33;

    return(Result);
}

internal inline m2
Transpose(m2 A)
{
    m2 Result;

    Result.M00 = Result.M00;
    Result.M01 = Result.M10;
    Result.M10 = Result.M01;
    Result.M11 = Result.M11;

    return(Result);
}

internal inline m2
Inverse(m2 A)
{
    m2 Result;

    f32 Determinant = A.M00 * A.M11 - A.M01 * A.M10;
    f32 InverseDeterminant = 1.0f / Determinant;

    Result.M00 = A.M11 * InverseDeterminant;
    Result.M01 = -A.M01 * InverseDeterminant;
    Result.M10 = -A.M10 * InverseDeterminant;
    Result.M11 = A.M00 * InverseDeterminant;

    return(Result);
}

internal inline m2
EigenVectors(m2 A, v2 *B)
{
    m2 Result = {};

    f32 Trace = A.M00 + A.M11;
    f32 Determinant = A.M00 * A.M11 - A.M01 * A.M10;

    f32 D = Sqrt(Trace * Trace - 4.0f * Determinant);

    f32 Lambda1 = 0.5f * (Trace + D);
    f32 Lambda2 = 0.5f * (Trace - D);

    if(B)
    {
        *B = V2(Lambda1, Lambda2);
    }

    m2 Matrix1 = M2(A.M00 - Lambda1, A.M01, A.M10, A.M11 - Lambda1);
    m2 Matrix2 = M2(A.M00 - Lambda2, A.M01, A.M10, A.M11 - Lambda2);

    if(Abs(Matrix1.M00) > 0.0001f || Abs(Matrix1.M01) > 0.0001f)
    {
        if(Abs(Matrix1.M00) < 0.0001f)
        {
            Result.M00 = 1.0f;
            Result.M10 = 0.0f;
        }
        else if(Abs(Matrix1.M01) < 0.0001f)
        {
            Result.M00 = 0.0f;
            Result.M10 = 1.0f;
        }
        else
        {
            Result.M00 = Matrix1.M01;
            Result.M10 = -Matrix1.M00;
        }
    }
    else if(Abs(Matrix1.M10) > 0.0001f || Abs(Matrix1.M11) > 0.0001f)
    {
        if(Abs(Matrix1.M10) < 0.0001f)
        {
            Result.M00 = 1.0f;
            Result.M10 = 0.0f;
        }
        else if(Abs(Matrix1.M11) < 0.0001f)
        {
            Result.M00 = 0.0f;
            Result.M10 = 1.0f;
        }
        else
        {
            Result.M00 = Matrix1.M11;
            Result.M10 = -Matrix1.M10;
        }
    }

    if(Abs(Matrix2.M00) > 0.0001f || Abs(Matrix2.M01) > 0.0001f)
    {
        if(Abs(Matrix2.M00) < 0.0001f)
        {
            Result.M01 = 1.0f;
            Result.M11 = 0.0f;
        }
        else if(Abs(Matrix2.M01) < 0.0001f)
        {
            Result.M01 = 0.0f;
            Result.M11 = 1.0f;
        }
        else
        {
            Result.M01 = Matrix2.M01;
            Result.M11 = -Matrix2.M00;
        }
    }
    else if(Abs(Matrix2.M10) > 0.0001f || Abs(Matrix2.M11) > 0.0001f)
    {
        if(Abs(Matrix2.M10) < 0.0001f)
        {
            Result.M01 = 1.0f;
            Result.M11 = 0.0f;
        }
        else if(Abs(Matrix2.M11) < 0.0001f)
        {
            Result.M01 = 0.0f;
            Result.M11 = 1.0f;
        }
        else
        {
            Result.M01 = Matrix2.M11;
            Result.M11 = -Matrix2.M10;
        }
    }

    return(Result);
}

internal inline m2
M2Identity()
{
    m2 Result = M2(
        1.0f, 0.0f,
        0.0f, 1.0f
    );
    return(Result);
}

internal inline m2
operator-(m2 A, m2 B)
{
    m2 Result;

    Result.M00 = A.M00 - B.M00;
    Result.M01 = A.M01 - B.M01;
    Result.M10 = A.M10 - B.M10;
    Result.M11 = A.M11 - B.M11;

    return(Result);
}

internal inline m2
operator+(m2 A, m2 B)
{
    m2 Result;

    Result.M00 = A.M00 + B.M00;
    Result.M01 = A.M01 + B.M01;
    Result.M10 = A.M10 + B.M10;
    Result.M11 = A.M11 + B.M11;

    return(Result);
}

internal inline m2
operator-(m2 A)
{
    m2 Result;

    Result.M00 = -A.M00;
    Result.M01 = -A.M01;
    Result.M10 = -A.M10;
    Result.M11 = -A.M11;

    return(Result);
}

internal inline f32
Distance(v2 A, v2 B)
{
    f32 Result = Sqrt(DistanceSquare(A, B));
    return(Result);
}

internal inline f32
Distance(v3 A, v3 B)
{
    f32 Result = Sqrt(DistanceSquare(A, B));
    return(Result);
}

internal inline f32
Distance(v4 A, v4 B)
{
    f32 Result = Sqrt(DistanceSquare(A, B));
    return(Result);
}

internal inline f32
Length(v2 A)
{
    f32 Result = Sqrt(LengthSquare(A));
    return(Result);
}

internal inline f32
Length(v3 A)
{
    f32 Result = Sqrt(LengthSquare(A));
    return(Result);
}

internal inline f32
Length(v4 A)
{
    f32 Result = Sqrt(LengthSquare(A));
    return(Result);
}

internal inline v2
Normalize(v2 A)
{
    v2 Result = A;
    
    f32 ALength = Length(A);
    if(ALength > Epsilon)
    {
        Result /= ALength;
    }
    
    return(Result);
}

internal inline v3
Normalize(v3 A)
{
    v3 Result = A;
    
    f32 ALength = Length(A);
    if(ALength > Epsilon)
    {
        Result /= ALength;
    }
    
    return(Result);
}

internal inline v4
Normalize(v4 A)
{
    v4 Result = A;
    
    f32 ALength = Length(A);
    if(ALength > Epsilon)
    {
        Result /= ALength;
    }
    
    return(Result);
}

internal inline f32
DistanceSquare(v2 A, v2 B)
{
    f32 Result = LengthSquare(A - B);
    return(Result);
}

internal inline f32
DistanceSquare(v3 A, v3 B)
{
    f32 Result = LengthSquare(A - B);
    return(Result);
}

internal inline f32
DistanceSquare(v4 A, v4 B)
{
    f32 Result = LengthSquare(A - B);
    return(Result);
}

internal inline f32
LengthSquare(v2 A)
{
    f32 Result = A.X * A.X + A.Y * A.Y;
    return(Result);
}

internal inline f32
LengthSquare(v3 A)
{
    f32 Result = A.X * A.X + A.Y * A.Y + A.Z * A.Z;
    return(Result);
}

internal inline f32
LengthSquare(v4 A)
{
    f32 Result = A.X * A.X + A.Y * A.Y + A.Z * A.Z + A.W * A.W;
    return(Result);
}

internal inline f32
Lerp(f32 A, f32 B, f32 T)
{
    f32 Result = A + (B - A) * T;
    return(Result);
}

internal inline v2
Lerp(v2 A, v2 B, f32 T)
{
    v2 Result = A + (B - A) * T;
    return(Result);
}

internal inline v3
Lerp(v3 A, v3 B, f32 T)
{
    v3 Result = A + (B - A) * T;
    return(Result);
}

internal inline v4
Lerp(v4 A, v4 B, f32 T)
{
    v4 Result = A + (B - A) * T;
    return(Result);
}

internal inline f32
Dot(v2 A, v2 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y;
    return(Result);
}

internal inline f32
Dot(v3 A, v3 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;
    return(Result);
}

internal inline f32
Dot(v4 A, v4 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
    return(Result);
}

internal inline v2
Hadamard(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X * B.X;
    Result.Y = A.Y * B.Y;
    return(Result);
}

internal inline v3
Hadamard(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X * B.X;
    Result.Y = A.Y * B.Y;
    Result.Z = A.Z * B.Z;
    return(Result);
}

internal inline v4
Hadamard(v4 A, v4 B)
{
    v4 Result;
    Result.X = A.X * B.X;
    Result.Y = A.Y * B.Y;
    Result.Z = A.Z * B.Z;
    Result.W = A.W * B.W;
    return(Result);
}

internal inline v3
Cross(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.Y * B.Z - A.Z * B.Y;
    Result.Y = A.Z * B.X - A.X * B.Z;
    Result.Z = A.X * B.Y - A.Y * B.X;
    return(Result);
}

internal inline f32
Determinant(v2 A, v2 B)
{
    f32 Result =  A.X * B.Y - A.Y * B.X;
    return(Result);
}



















// TODO

/*

#define HalfPi 1.57079632679f
#define Pi     3.14159265359f
#define Tau    6.28318530718f

#define NaN (0.0f*(0.0f/0.0f))

#define DegreesToRadiansScalar (Pi / 180.0f)
#define RadiansToDegreesScalar (180.0f / Pi)

#define v2(X, Y)       V2Make((f32)(X), (f32)(Y))
#define v3(X, Y, Z)    V3Make((f32)(X), (f32)(Y), (f32)(Z))
#define v4(X, Y, Z, W) V4Make((f32)(X), (f32)(Y), (f32)(Z), (f32)(W))

#define V3Forward  v3(1.0f, 0.0f, 0.0f)
#define V3Right    v3(0.0f, 1.0f, 0.0f)
#define V3Up       v3(0.0f, 0.0f, 1.0f)
#define V3Backward v3(-1.0f, 0.0f, 0.0f)
#define V3Left     v3(0.0f, -1.0f, 0.0f)
#define V3Down     v3(0.0f, 0.0f, -1.0f)

#define m4(M00, M01, M02, M03, \
           M10, M11, M12, M13, \
           M20, M21, M22, M23, \
           M30, M31, M32, M33) M4Make((f32)(M00), (f32)(M01), (f32)(M02), (f32)(M03), \
                                      (f32)(M10), (f32)(M11), (f32)(M12), (f32)(M13), \
                                      (f32)(M20), (f32)(M21), (f32)(M22), (f32)(M23), \
                                      (f32)(M30), (f32)(M31), (f32)(M32), (f32)(M33))

typedef union v2 v2;
typedef union v3 v3;
typedef union v4 v4;

typedef union m4 m4;

union v2
{
    struct
    {
        f32 X, Y;
    };

    struct
    {
        f32 U, V;
    };
};

union v3
{
    struct
    {
        f32 X, Y, Z;
    };

    struct
    {
        f32 R, G, B;
    };

    struct
    {
        f32 Pitch, Yaw, Roll;
    };
};

union v4
{
    struct
    {
        f32 X, Y, Z, W;
    };

    struct
    {
        f32 R, G, B, A;
    };

    __m128 Values;
};

union m4
{
    struct
    {
        f32 M00, M01, M02, M03,
            M10, M11, M12, M13,
            M20, M21, M22, M23,
            M30, M31, M32, M33;
    };

    __m128 Rows[4];
};

internal inline v3
V4QuaternionRotateVector(v4 Quaternion, v3 Vector);

internal inline v4
V4Make(f32 X, f32 Y, f32 Z, f32 W);

internal inline v2
V2Make(f32 X, f32 Y)
{
    v2 Result;
    Result.X = X;
    Result.Y = Y;
    return(Result);
}

internal inline v2
V2Add(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return(Result);
}

internal inline v2
V2Substract(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return(Result);
}

internal inline v2
V2Hadamard(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X * B.X;
    Result.Y = A.Y * B.Y;
    return(Result);
}

internal inline v2
V2Scale(v2 A, f32 B)
{
    v2 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    return(Result);
}

internal inline f32
V2Dot(v2 A, v2 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y;
    return(Result);
}

internal inline v2
V2Lerp(v2 A, v2 B, f32 T)
{
    v2 Result;
    Result.X = A.X + (B.X - A.X) * T;
    Result.Y = A.Y + (B.Y - A.Y) * T;
    return(Result);
}

internal inline f32
V2Length(v2 A)
{
    f32 Result = SquareRoot(A.X * A.X + A.Y * A.Y);
    return(Result);
}

internal inline v2
V2Unit(v2 A)
{
    v2 Result = v2(0.0f, 0.0f);

    f32 Length = V2Length(A);
    if(Length)
    {
        Result.X = A.X / Length;
        Result.Y = A.Y / Length;
    }

    return(Result);
}

internal inline f32
V2Distance(v2 A, v2 B)
{
    f32 Result;

    v2 Delta = V2Substract(A, B);
    Result = V2Length(Delta);

    return(Result);
}

internal inline v2
V2Rotate(v2 V, f32 Angle)
{
    f32 S = Sin(Angle * DegreesToRadiansScalar);
    f32 C = Cos(Angle * DegreesToRadiansScalar);

    v2 Result = v2(V.X * C - V.Y * S, V.X * S + V.Y * C);
    return(Result);
}

internal inline v3
V3Make(f32 X, f32 Y, f32 Z)
{
    v3 Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    return(Result);
}

internal inline v3
V3Add(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    return(Result);
}

internal inline v3
V3Substract(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    return(Result);
}

internal inline v3
V3Hadamard(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X * B.X;
    Result.Y = A.Y * B.Y;
    Result.Z = A.Z * B.Z;
    return(Result);
}

internal inline v3
V3Scale(v3 A, f32 B)
{
    v3 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    Result.Z = A.Z * B;
    return(Result);
}

internal inline f32
V3Dot(v3 A, v3 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;
    return(Result);
}

internal inline v3
V3Cross(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.Y * B.Z - A.Z * B.Y;
    Result.Y = A.Z * B.X - A.X * B.Z;
    Result.Z = A.X * B.Y - A.Y * B.X;
    return(Result);
}

internal inline v3
V3Lerp(v3 A, v3 B, f32 T)
{
    v3 Result;
    Result.X = A.X + (B.X - A.X) * T;
    Result.Y = A.Y + (B.Y - A.Y) * T;
    Result.Z = A.Z + (B.Z - A.Z) * T;
    return(Result);
}

internal inline f32
V3Length(v3 A)
{
    f32 Result = SquareRoot(A.X * A.X + A.Y * A.Y + A.Z * A.Z);
    return(Result);
}

internal inline v3
V3Unit(v3 A)
{
    v3 Result = v3(0.0f, 0.0f, 0.0f);

    f32 Length = V3Length(A);
    if(Length)
    {
        Result.X = A.X / Length;
        Result.Y = A.Y / Length;
        Result.Z = A.Z / Length;
    }

    return(Result);
}

internal inline f32
V3Distance(v3 A, v3 B)
{
    f32 Result;

    v3 Delta = V3Substract(A, B);
    Result = V3Length(Delta);

    return(Result);
}

internal inline v3
V3Transform(v3 A, v3 Translation, v4 Rotation, v3 Scale)
{
    v3 ScaledVector = V3Hadamard(Scale, Translation);
    v3 RotatedVector = V4QuaternionRotateVector(Rotation, ScaledVector);
    v3 TranslatedVector = V3Add(RotatedVector, Translation);

    return(TranslatedVector);
}

internal inline v3
V3TransformNoTranslation(v3 A, v3 Translation, v4 Rotation, v3 Scale)
{
    v3 ScaledVector = V3Hadamard(Scale, Translation);
    v3 RotatedVector = V4QuaternionRotateVector(Rotation, ScaledVector);

    return(RotatedVector);
}

internal inline v3
V3InverseTransform(v3 A, v3 Translation, v4 Rotation, v3 Scale)
{
    v4 InverseRotation = v4(-Rotation.X, -Rotation.Y, -Rotation.Z, Rotation.W);
    v3 InverseScale = v3(1.0f / Scale.X, 1.0f / Scale.Y, 1.0f / Scale.Z);

    v3 TranslatedVector = V3Substract(A, Translation);
    v3 RotatedVector = V4QuaternionRotateVector(InverseRotation, TranslatedVector);
    v3 ScaledVector = V3Hadamard(InverseScale, RotatedVector);

    return(ScaledVector);
}

internal inline v3
V3InverseTransformNoTranslation(v3 A, v4 Rotation, v3 Scale)
{
    v4 InverseRotation = v4(-Rotation.X, -Rotation.Y, -Rotation.Z, Rotation.W);
    v3 InverseScale = v3(1.0f / Scale.X, 1.0f / Scale.Y, 1.0f / Scale.Z);

    v3 RotatedVector = V4QuaternionRotateVector(InverseRotation, A);
    v3 ScaledVector = V3Hadamard(InverseScale, RotatedVector);

    return(ScaledVector);
}

internal inline v3
V3RotatorToVector(v3 Rotator)
{
    f32 PC, PS, YC, YS;
    SinCos(Rotator.Pitch * DegreesToRadiansScalar, &PS, &PC);
    SinCos(Rotator.Yaw   * DegreesToRadiansScalar, &YS, &YC);
    v3 Vector = v3(PC * YC, PC * YS, PS);

    return(Vector);
}

internal inline v3
V3VectorToRotator(v3 Vector)
{
    v3 Rotator;
    Rotator.Yaw = Atan2(Vector.Y, Vector.X) * RadiansToDegreesScalar;
    Rotator.Pitch = Atan2(Vector.Z, SquareRoot(Vector.X * Vector.X + Vector.Y * Vector.Y)) * RadiansToDegreesScalar;
    Rotator.Roll = 0.0f;

    return(Rotator);
}

internal inline v3
V3RotateAroundZ(v3 V, f32 Angle)
{
    f32 S = Sin(Angle * DegreesToRadiansScalar);
    f32 C = Cos(Angle * DegreesToRadiansScalar);

    v3 Result = v3(V.X * C - V.Y * S, V.X * S + V.Y * C, V.Z);
    return(Result);
}

internal inline v4
V4Make(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    Result.W = W;
    return(Result);
}

internal inline v4
V4Add(v4 A, v4 B)
{
    v4 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    Result.W = A.W + B.W;
    return(Result);
}

internal inline v4
V4Substract(v4 A, v4 B)
{
    v4 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    Result.W = A.W - B.W;
    return(Result);
}

internal inline v4
V4Hadamard(v4 A, v4 B)
{
    v4 Result;
    Result.X = A.X * B.X;
    Result.Y = A.Y * B.Y;
    Result.Z = A.Z * B.Z;
    Result.W = A.W * B.W;
    return(Result);
}

internal inline v4
V4Scale(v4 A, f32 B)
{
    v4 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    Result.Z = A.Z * B;
    Result.W = A.W * B;
    return(Result);
}

internal inline f32
V4Dot(v4 A, v4 B)
{
    // TODO: Figure out how we should load these
    __m128 XMM0 = _mm_loadu_ps(&A.X);
    __m128 XMM1 = _mm_loadu_ps(&B.X);

    f32 Result = _mm_cvtss_f32(_mm_dp_ps(XMM0, XMM1, 0xFF));
    return(Result);
}

internal inline v4
V4Lerp(v4 A, v4 B, f32 T)
{
    v4 Result;
    Result.X = A.X + (B.X - A.X) * T;
    Result.Y = A.Y + (B.Y - A.Y) * T;
    Result.Z = A.Z + (B.Z - A.Z) * T;
    Result.W = A.W + (B.W - A.W) * T;
    return(Result);
}

internal inline f32
V4Length(v4 A)
{
    f32 Result = SquareRoot(A.X * A.X + A.Y * A.Y + A.Z * A.Z + A.W * A.W);
    return(Result);
}

internal inline v4
V4Unit(v4 A)
{
    v4 Result = v4(0.0f, 0.0f, 0.0f, 0.0f);

    f32 Length = V4Length(A);
    if(Length)
    {
        Result.X = A.X / Length;
        Result.Y = A.Y / Length;
        Result.Z = A.Z / Length;
        Result.W = A.W / Length;
    }

    return(Result);
}

internal inline f32
V4Distance(v4 A, v4 B)
{
    f32 Result;

    v4 Delta = V4Substract(A, B);
    Result = V4Length(Delta);

    return(Result);
}

internal inline v4
V4MakeQuaternionFromRotation(v3 Axis, f32 Angle)
{
    v3 AxisUnit = V3Unit(Axis);

    f32 CosAngle, SinAngle;
    SinCos(Angle * 0.5f * DegreesToRadiansScalar, &SinAngle, &CosAngle);

    v4 Result = v4(SinAngle * AxisUnit.X, SinAngle * AxisUnit.Y, SinAngle * AxisUnit.Z, CosAngle);
    return(Result);
}

internal inline v3
V4QuaternionRotateVector(v4 Quaternion, v3 Vector)
{
    v3 XYZ = v3(Quaternion.X, Quaternion.Y, Quaternion.Z);
    v3 T = V3Scale(V3Cross(XYZ, Vector), 2.0f);
    v3 Result = V3Add(V3Add(Vector, V3Scale(T, Quaternion.W)), V3Cross(XYZ, T));
    return(Result);
}

internal inline m4
M4Make(f32 M00, f32 M01, f32 M02, f32 M03,
       f32 M10, f32 M11, f32 M12, f32 M13,
       f32 M20, f32 M21, f32 M22, f32 M23,
       f32 M30, f32 M31, f32 M32, f32 M33)
{
    m4 Result;

    Result.M00 = M00;
    Result.M01 = M01;
    Result.M02 = M02;
    Result.M03 = M03;

    Result.M10 = M10;
    Result.M11 = M11;
    Result.M12 = M12;
    Result.M13 = M13;

    Result.M20 = M20;
    Result.M21 = M21;
    Result.M22 = M22;
    Result.M23 = M23;

    Result.M30 = M30;
    Result.M31 = M31;
    Result.M32 = M32;
    Result.M33 = M33;

    return(Result);
}

internal inline m4
M4MakeOrthographic(f32 Left, f32 Right, f32 Top, f32 Bottom)
{
    m4 Result = m4(2.0f / (Right - Left), 0.0f,                  0.0f, (Right + Left) / (Left - Right),
                   0.0f,                  2.0f / (Top - Bottom), 0.0f, (Top + Bottom) / (Bottom - Top),
                   0.0f,                  0.0f,                  1.0f, 0.0f,
                   0.0f,                  0.0f,                  0.0f, 1.0f);
    return(Result);
}

internal inline m4
M4MakeInverseOrthographic(f32 Left, f32 Right, f32 Top, f32 Bottom)
{
    m4 Result = m4((Right - Left) / 2.0f,  0.0f,                  0.0f, (Right + Left) / 2.0f,
                   0.0f,                   (Top - Bottom) / 2.0f, 0.0f, (Top + Bottom) / 2.0f,
                   0.0f,                   0.0f,                  1.0f, 0.0f,
                   0.0f,                   0.0f,                  0.0f, 1.0f);
    return(Result);
}

internal inline m4
M4MakeUnrealToNDC(void)
{
    m4 Result = m4(0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
    return(Result);
}

internal inline m4
M4MakePerspective(f32 FOV, f32 FarPlane, f32 NearPlane, f32 WidthOverHeight)
{
    f32 T = Tan(FOV * DegreesToRadiansScalar / 2.0f);
    f32 R = 16.0f / 9.0f;
    f32 C = WidthOverHeight;
    f32 N = NearPlane;
    f32 F = FarPlane;

    m4 Result = m4(R / (C * T), 0.0f,  0.0f,        0.0f,
                   0.0f,        R / T, 0.0f,        0.0f,
                   0.0f,        0.0f,  N / (N - F), -F * N / (N - F),
                   0.0f,        0.0f,  1.0f,        0.0f);
    return(Result);
}

internal inline m4
M4MakeRotation(v3 Rotation)
{
    f32 PS, PC, YS, YC, RS, RC;
    SinCos(Rotation.Pitch * DegreesToRadiansScalar, &PS, &PC);
    SinCos(Rotation.Yaw   * DegreesToRadiansScalar, &YS, &YC);
    SinCos(Rotation.Roll  * DegreesToRadiansScalar, &RS, &RC);

    m4 Result = m4(PC * YC,                -PC * YS,                -PS,     0.0f,
                   RS * PS * YC + RC * YS, RC * YC - RS * PS * YS,  RS * PC, 0.0f,
                   RC * PS * YC - RS * YS, -YC * RS - RC * PS * YS, RC * PC, 0.0f,
                   0.0f,                   0.0f,                    0.0f,    1.0f);
    return(Result);
}

internal inline m4
M4MakeTranslation(v3 Translation)
{
    m4 Result = m4(1.0f, 0.0f, 0.0f, Translation.X,
                   0.0f, 1.0f, 0.0f, Translation.Y,
                   0.0f, 0.0f, 1.0f, Translation.Z,
                   0.0f, 0.0f, 0.0f, 1.0f);
    return(Result);
}

internal inline m4
M4MakeIdentity(void)
{
    m4 Result = m4(1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f);
    return(Result);
}

internal inline m4
M4MakeFromTransform(v3 Translation, v4 Rotation, v3 Scale)
{
    f32 X2 = Rotation.X + Rotation.X;
    f32 Y2 = Rotation.Y + Rotation.Y;
    f32 Z2 = Rotation.Z + Rotation.Z;

    f32 XX2 = Rotation.X * X2;
    f32 XY2 = Rotation.X * Y2;
    f32 XZ2 = Rotation.X * Z2;

    f32 YY2 = Rotation.Y * Y2;
    f32 YZ2 = Rotation.Y * Z2;

    f32 ZZ2 = Rotation.Z * Z2;

    f32 WX2 = Rotation.W * X2;
    f32 WY2 = Rotation.W * Y2;
    f32 WZ2 = Rotation.W * Z2;

    m4 Result = m4((1.0f - (YY2 + ZZ2)) * Scale.X, (XY2 - WZ2)          * Scale.Y, (XZ2 + WY2)          * Scale.Z, Translation.X,
                   (XY2 + WZ2)          * Scale.X, (1.0f - (XX2 + ZZ2)) * Scale.Y, (YZ2 - WX2)          * Scale.Z, Translation.Y,
                   (XZ2 - WY2)          * Scale.X, (YZ2 + WX2)          * Scale.Y, (1.0f - (XX2 + YY2)) * Scale.Z, Translation.Z,
                   0.0f,                            0.0f,                           0.0f,                           1.0f);

    return(Result);
}

internal inline m4
M4MakeInverseFromTransform(v3 Translation, v4 Rotation, v3 Scale)
{
    v4 InverseRotation = v4(-Rotation.X, -Rotation.Y, -Rotation.Z, Rotation.W);
    v3 InverseScale = v3(1.0f / Scale.X, 1.0f / Scale.Y, 1.0f / Scale.Z);
    v3 InverseTranslation = V4QuaternionRotateVector(InverseRotation, V3Hadamard(InverseScale, V3Scale(Translation, -1.0f)));

    f32 X2 = InverseRotation.X + InverseRotation.X;
    f32 Y2 = InverseRotation.Y + InverseRotation.Y;
    f32 Z2 = InverseRotation.Z + InverseRotation.Z;

    f32 XX2 = InverseRotation.X * X2;
    f32 XY2 = InverseRotation.X * Y2;
    f32 XZ2 = InverseRotation.X * Z2;

    f32 YY2 = InverseRotation.Y * Y2;
    f32 YZ2 = InverseRotation.Y * Z2;

    f32 ZZ2 = InverseRotation.Z * Z2;

    f32 WX2 = InverseRotation.W * X2;
    f32 WY2 = InverseRotation.W * Y2;
    f32 WZ2 = InverseRotation.W * Z2;

    m4 Result = m4((1.0f - (YY2 + ZZ2)) * InverseScale.X, (XY2 - WZ2)          * InverseScale.Y, (XZ2 + WY2)          * InverseScale.Z, InverseTranslation.X,
                   (XY2 + WZ2)          * InverseScale.X, (1.0f - (XX2 + ZZ2)) * InverseScale.Y, (YZ2 - WX2)          * InverseScale.Z, InverseTranslation.Y,
                   (XZ2 - WY2)          * InverseScale.X, (YZ2 + WX2)          * InverseScale.Y, (1.0f - (XX2 + YY2)) * InverseScale.Z, InverseTranslation.Z,
                   0.0f,                                  0.0f,                                  0.0f,                                  1.0f);

    return(Result);
}

internal inline m4
M4Multiply(m4 A, m4 B)
{
    // TODO: AVX2 version from XMMatrixMultiply, https://github.com/microsoft/DirectXMath/blob/main/Inc/DirectXMathMatrix.inl#L228

    __m128 XMM0 = _mm_mul_ps(_mm_set1_ps(A.M00), B.Rows[0]);
    XMM0 = _mm_fmadd_ps(_mm_set1_ps(A.M01), B.Rows[1], XMM0);
    XMM0 = _mm_fmadd_ps(_mm_set1_ps(A.M02), B.Rows[2], XMM0);
    XMM0 = _mm_fmadd_ps(_mm_set1_ps(A.M03), B.Rows[3], XMM0);

    __m128 XMM1 = _mm_mul_ps(_mm_set1_ps(A.M10), B.Rows[0]);
    XMM1 = _mm_fmadd_ps(_mm_set1_ps(A.M11), B.Rows[1], XMM1);
    XMM1 = _mm_fmadd_ps(_mm_set1_ps(A.M12), B.Rows[2], XMM1);
    XMM1 = _mm_fmadd_ps(_mm_set1_ps(A.M13), B.Rows[3], XMM1);

    __m128 XMM2 = _mm_mul_ps(_mm_set1_ps(A.M20), B.Rows[0]);
    XMM2 = _mm_fmadd_ps(_mm_set1_ps(A.M21), B.Rows[1], XMM2);
    XMM2 = _mm_fmadd_ps(_mm_set1_ps(A.M22), B.Rows[2], XMM2);
    XMM2 = _mm_fmadd_ps(_mm_set1_ps(A.M23), B.Rows[3], XMM2);

    __m128 XMM3 = _mm_mul_ps(_mm_set1_ps(A.M30), B.Rows[0]);
    XMM3 = _mm_fmadd_ps(_mm_set1_ps(A.M31), B.Rows[1], XMM3);
    XMM3 = _mm_fmadd_ps(_mm_set1_ps(A.M32), B.Rows[2], XMM3);
    XMM3 = _mm_fmadd_ps(_mm_set1_ps(A.M33), B.Rows[3], XMM3);

    m4 Result;
    Result.Rows[0] = XMM0;
    Result.Rows[1] = XMM1;
    Result.Rows[2] = XMM2;
    Result.Rows[3] = XMM3;
    return(Result);
}

internal inline v3
M4Transform(m4 Matrix, v3 Target)
{
    v4 Vector = v4(Target.X, Target.Y, Target.Z, 1.0f);

    _MM_TRANSPOSE4_PS(Matrix.Rows[0], Matrix.Rows[1], Matrix.Rows[2], Matrix.Rows[3]);

    __m128 XMM0 = _mm_mul_ps(_mm_set1_ps(Vector.X), Matrix.Rows[0]);
    XMM0 = _mm_fmadd_ps(_mm_set1_ps(Vector.Y), Matrix.Rows[1], XMM0);
    XMM0 = _mm_fmadd_ps(_mm_set1_ps(Vector.Z), Matrix.Rows[2], XMM0);
    XMM0 = _mm_fmadd_ps(_mm_set1_ps(Vector.W), Matrix.Rows[3], XMM0);

    v4 Transformed;
    Transformed.Values = XMM0;

    v3 Result = v3(Transformed.X / Transformed.W, Transformed.Y / Transformed.W, Transformed.Z / Transformed.W);
    return(Result);
}

*/

#endif // WASP_MATH_H