/**
 * glN64_GX - 3DMath.h
 * Copyright (C) 2003 Orkin
 * Copyright (C) 2008, 2009 sepp256 (Port to Wii/Gamecube/PS3)
 * Copyright (C) 2009 tehpola (paired single optimization for GEKKO)
 *
 * glN64 homepage: http://gln64.emulation64.com
 * Wii64 homepage: http://www.emulatemii.com
 * email address: sepp256@gmail.com
 *
**/

#ifndef _3DMATH_H
#define _3DMATH_H

#include <string.h>
#include <math.h>

inline void CopyMatrix( float m0[4][4], float m1[4][4] )
{
	memcpy( m0, m1, 16 * sizeof( float ) );
}

inline void MultMatrix( float m0[4][4], float m1[4][4] )
{
#if defined(GEKKO)
	
	float dst[4][4]/* = {{0.0f}}*/;
	
	for (int i = 0; i < 4; i++)
	{
		/*for (int k = 0; k < 4; k++)
		{
			dst[i][0] += m1[i][k]*m0[k][0];
			dst[i][1] += m1[i][k]*m0[k][1];
			dst[i][2] += m1[i][k]*m0[k][2];
			dst[i][3] += m1[i][k]*m0[k][3];
		}*/
		__asm__ volatile(
			"psq_l       2, 0(%0), 1, 0 \n"
			"psq_l       3, 0(%1), 0, 0  \n"
			"psq_l       5, 8(%1), 0, 0  \n"
			
			"ps_muls0    4, 3, 2      \n"
			"ps_muls0    6, 5, 2      \n"
			
			"psq_l       2,  4(%0), 1, 0 \n"
			"psq_l       3, 16(%1), 0, 0  \n"
			"psq_l       5, 24(%1), 0, 0  \n"
			
			"ps_madds0   4, 3, 2, 4      \n"
			"ps_madds0   6, 5, 2, 6      \n"
			
			"psq_l       2,  8(%0), 1, 0 \n"
			"psq_l       3, 32(%1), 0, 0  \n"
			"psq_l       5, 40(%1), 0, 0  \n"
			
			"ps_madds0   4, 3, 2, 4      \n"
			"ps_madds0   6, 5, 2, 6      \n"
			
			"psq_l       2, 12(%0), 1, 0 \n"
			"psq_l       3, 48(%1), 0, 0  \n"
			"psq_l       5, 56(%1), 0, 0  \n"
			
			"ps_madds0   4, 3, 2, 4      \n"
			"ps_madds0   6, 5, 2, 6      \n"
			
			"psq_st      4, 0(%2), 0, 0  \n"
			"psq_st      6, 8(%2), 0, 0  \n"
			:: "r" (m1+i), "r" (m0), "r" (dst+i)
			:  "r0", "fr2", "fr3", "fr4", "fr5", "fr6",
			   "memory");
	}
	memcpy( m0, dst, sizeof(float) * 16 );
# else // GEKKO
	int i;
	float dst[4][4];

	for (i = 0; i < 4; i++)
	{
		dst[0][i] = m0[0][i]*m1[0][0] + m0[1][i]*m1[0][1] + m0[2][i]*m1[0][2] + m0[3][i]*m1[0][3];
		dst[1][i] = m0[0][i]*m1[1][0] + m0[1][i]*m1[1][1] + m0[2][i]*m1[1][2] + m0[3][i]*m1[1][3];
		dst[2][i] = m0[0][i]*m1[2][0] + m0[1][i]*m1[2][1] + m0[2][i]*m1[2][2] + m0[3][i]*m1[2][3];
		dst[3][i] = m0[0][i]*m1[3][0] + m0[1][i]*m1[3][1] + m0[2][i]*m1[3][2] + m0[3][i]*m1[3][3];
	}
	memcpy( m0, dst, sizeof(float) * 16 );
# endif // !( X86_ASM || GEKKO )
}

inline void Transpose3x3Matrix( float mtx[4][4] )
{
	float tmp;

	tmp = mtx[0][1];
	mtx[0][1] = mtx[1][0];
	mtx[1][0] = tmp;

	tmp = mtx[0][2];
	mtx[0][2] = mtx[2][0];
	mtx[2][0] = tmp;

	tmp = mtx[1][2];
	mtx[1][2] = mtx[2][1];
	mtx[2][1] = tmp;
}

inline void TransposeMatrix( float mtx[4][4] )
{
	float tmp;

	tmp = mtx[0][1];
	mtx[0][1] = mtx[1][0];
	mtx[1][0] = tmp;

	tmp = mtx[0][2];
	mtx[0][2] = mtx[2][0];
	mtx[2][0] = tmp;

	tmp = mtx[1][2];
	mtx[1][2] = mtx[2][1];
	mtx[2][1] = tmp;

	tmp = mtx[0][3];
	mtx[0][3] = mtx[3][0];
	mtx[3][0] = tmp;

	tmp = mtx[1][3];
	mtx[1][3] = mtx[3][1];
	mtx[3][1] = tmp;

	tmp = mtx[2][3];
	mtx[2][3] = mtx[3][2];
	mtx[3][2] = tmp;

}

inline void TransformVertex( float vtx[4], float mtx[4][4] )//, float perspNorm )
{
#if defined(GEKKO)

	__asm__ volatile(
		"psq_l      2, 0(%0), 1, 0 \n" // fr2 = Vj,  1.0f
		"psq_l      3, 0(%1), 0, 0 \n" // fr3 = Mj0, Mj1
		"psq_l      5, 8(%1), 0, 0 \n" // fr5 = Mj2, Mj3
		
		"ps_muls0   4, 3, 2  \n" // fr4 = fr3 * Vj
		"ps_muls0   6, 5, 2  \n" // fr6 = fr5 * Vj
		
		"psq_l      2, 4(%0), 1, 0 \n" // fr2 = Vj,  1.0f
		"psq_l      3, 16(%1), 0, 0 \n" // fr3 = Mj0, Mj1
		"psq_l      5, 24(%1), 0, 0 \n" // fr5 = Mj2, Mj3
		
		"ps_madds0  4, 3, 2, 4  \n" // fr4 = fr3 * Vj + fr4
		"ps_madds0  6, 5, 2, 6  \n" // fr6 = fr5 * Vj + fr6
		
		"psq_l      2, 8(%0), 1, 0 \n" // fr2 = Vj,  1.0f
		"psq_l      3, 32(%1), 0, 0 \n" // fr3 = Mj0, Mj1
		"psq_l      5, 40(%1), 0, 0 \n" // fr5 = Mj2, Mj3
		
		"ps_madds0  4, 3, 2, 4  \n" // fr4 = fr3 * Vj + fr4
		"ps_madds0  6, 5, 2, 6  \n" // fr6 = fr5 * Vj + fr6
		
		"psq_l	3, 48(%1), 0, 0 \n" // fr3 = M30, M31
		"psq_l	5, 56(%1), 0, 0 \n" // fr5 = M32, M33
	
		"ps_add	4, 3, 4 \n" // fr4 = fr3 + fr4
		"ps_add 6, 5, 6 \n" // fr6 = fr5 + fr6
		
		"psq_st     4, 0(%0), 0, 0 \n" // D0, D1 = fr4
		"psq_st     6, 8(%0), 0, 0 \n" // D2, D3 = fr6
		
		:: "r" (vtx), "r" (mtx)
		:  "fr2", "fr3", "fr4", "fr5", "fr6",
		   "r0", "memory");
	
# else // GEKKO
	float x, y, z;
	x = vtx[0];
	y = vtx[1];
	z = vtx[2];

	vtx[0] = x * mtx[0][0] +
	         y * mtx[1][0] +
	         z * mtx[2][0];

	vtx[1] = x * mtx[0][1] +
	         y * mtx[1][1] +
	         z * mtx[2][1];

	vtx[2] = x * mtx[0][2] +
	         y * mtx[1][2] +
	         z * mtx[2][2];

	vtx[3] = x * mtx[0][3] +
	         y * mtx[1][3] +
	         z * mtx[2][3];

	vtx[0] += mtx[3][0];
	vtx[1] += mtx[3][1];
	vtx[2] += mtx[3][2];
	vtx[3] += mtx[3][3];
# endif // !( X86_ASM || GEKKO )
}

inline void TransformVector( float vec[3], float mtx[4][4] )
{
#if defined(GEKKO)

	__asm__ volatile(
		"psq_l      2, 0(%0), 1, 0 \n" // fr2 = Vj,  1.0f
		"psq_l      3, 0(%1), 0, 0 \n" // fr3 = Mj0, Mj1
		"psq_l      5, 8(%1), 1, 0 \n" // fr5 = Mj2, 1.0f

		"ps_muls0   4, 3, 2  \n" // fr4 = fr3 * Vj
		"ps_muls0   6, 5, 2  \n" // fr6 = fr5 * Vj

		"psq_l      2,  4(%0), 1, 0 \n" // fr2 = Vj,  1.0f
		"psq_l      3, 16(%1), 0, 0 \n" // fr3 = Mj0, Mj1
		"psq_l      5, 24(%1), 1, 0 \n" // fr5 = Mj2, 1.0f

		"ps_madds0  4, 3, 2, 4  \n" // fr4 = fr3 * Vj + fr4
		"ps_madds0  6, 5, 2, 6  \n" // fr6 = fr5 * Vj + fr6

		"psq_l      2,  8(%0), 1, 0 \n" // fr2 = Vj,  1.0f
		"psq_l      3, 32(%1), 0, 0 \n" // fr3 = Mj0, Mj1
		"psq_l      5, 40(%1), 1, 0 \n" // fr5 = Mj2, 1.0f

		"ps_madds0  4, 3, 2, 4  \n" // fr4 = fr3 * Vj + fr4
		"ps_madds0  6, 5, 2, 6  \n" // fr6 = fr5 * Vj + fr6

		"psq_st     4, 0(%0), 0, 0 \n" // D0, D1 = fr4
		"psq_st     6, 8(%0), 1, 0 \n" // D2     = fr6

		:: "r" (vec), "r" (mtx)
		:  "fr2", "fr3", "fr4", "fr5", "fr6",
		   "r0", "memory");

# else // GEKKO
	vec[0] = mtx[0][0] * vec[0]
		   + mtx[1][0] * vec[1]
		   + mtx[2][0] * vec[2];
	vec[1] = mtx[0][1] * vec[0]
		   + mtx[1][1] * vec[1]
		   + mtx[2][1] * vec[2];
	vec[2] = mtx[0][2] * vec[0]
		   + mtx[1][2] * vec[1]
		   + mtx[2][2] * vec[2];
# endif // !( X86_ASM || GEKKO )
}

inline void Normalize( float v[3] )
{
	float len;

	len = (float)(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if (len != 0.0)
	{
		len = (float)sqrt( len );
		v[0] /= (float)len;
		v[1] /= (float)len;
		v[2] /= (float)len;
	}
}

inline void Normalize2D( float v[2] )
{
	float len;

	len = (float)sqrt( v[0]*v[0] + v[1]*v[1] );
/*	if (len != 0.0)
	{*/
		v[0] /= len;
		v[1] /= len;
/*	}
	else
	{
		v[0] = 0.0;
		v[1] = 0.0;
	}*/
}


/*inline float Determinate4x4( float m[4][4] )
{
	float m_2233_3223;
	float m_2133_3123;
	float m_2132_3122;
	float m_2033_3023;
	float m_2032_3022;
	float m_2031_3021;
	float det, ret;

	__asm {
		mov		esi, dword ptr [m]
	
										//	ST(7)			ST(6)			ST(5)			ST(4)			ST(3)			ST(2)			ST(1)			ST
		fld		dword ptr [esi+20h]		//																													m20
		fld		dword ptr [esi+30h]		//																									m20				m30

		fld		dword ptr [esi+34h]		//																					m20				m30				m31
		fmul	ST, ST(2)				//																					m20				m30				m20*m31
		fld		dword ptr [esi+24h]		//																	m20				m30				m20*m31			m21
		fmul	ST, ST(2)				//																	m20				m30				m20*m31			m30*m21
		fsub							//																					m20				m30				m20*m31-m30*m21
		fstp	dword ptr [m_2031_3021] //																									m20				m30

		fld		dword ptr [esi+38h]		//																					m20				m30				m32
		fmul	ST, ST(2)				//																					m20				m30				m20*m32
		fld		dword ptr [esi+28h]		//																	m20				m30				m20*m32			m22
		fmul	ST, ST(2)				//																	m20				m30				m20*m32			m30*m22
		fsub							//																					m20				m30				m20*m32-m30*m22
		fstp	dword ptr [m_2032_3022]	//																									m20				m30

		fld		dword ptr [esi+3Ch]		//																					m20				m30				m33
		fmulp	ST(2), ST				//																									m20*m33			m30
		fld		dword ptr [esi+2Ch]		//																					m20*m33			m30				m23
		fmul							//																									m20*m33			m30*m23
		fsub							//																													m20*m33-m30*m23
		fstp	dword ptr [m_2033_3023]	//

		fld		dword ptr [esi+24h]		//																													m21
		fld		dword ptr [esi+34h]		//																									m21				m31
		fld		dword ptr [esi+38h]		//																					m21				m31				m32
		fld		dword ptr [esi+28h]		//																	m21				m31				m32				m22

		fld		ST(1)					//													m21				m31				m32				m22				m32
		fmul	ST, ST(4)				//													m21				m31				m32				m22				m21*m32
		fld		ST(1)					//									m21				m31				m32				m22				m21*m32			m22
		fmul	ST, ST(4)				//									m21				m31				m32				m22				m21*m32			m31*m22
		fsub							//													m21				m31				m32				m22				m21*m32-m31*m22
		fstp	dword ptr [m_2132_3122] //																	m21				m31				m32				m22
		
		fld		dword ptr [esi+3Ch]		//													m21				m31				m32				m22				m33
		fxch	ST(1), ST				//													m21				m31				m32				m33				m22
		fmul	ST, ST(1)				//													m21				m31				m32				m33				m22*m33
		fld		dword ptr [esi+2Ch]		//									m21				m31				m32				m33				m22*m33			m23
		fxch	ST(3), ST				//									m21				m31				m23				m33				m22*m33			m32
		fmul	ST, ST(3)				//									m21				m31				m23				m33				m22*m33			m32*m23
		fsub							//													m21				m31				m23				m33				m22*m33-m32*m23
		fstp	dword ptr [m_2233_3223] //																	m21				m31				m23				m33
		
		fmulp	ST(3), ST				//																					m21*m33			m31				m23
		fmul							//																									m21*m33			m31*m23
		fsub							//																													m21*m33-m31*m23
		tstp	dword ptr [m_2133_3123] //

		fld0							//																													0.0
		fld		dword ptr [esi+1Ch]		//																									0.0				m13
		fld		dword ptr [esi+18h]		//																					0.0				m13				m12
		fld		dword ptr [m_2233_3223] //																	0.0				m13				m12				m_2233_3223

		fld		dword ptr [esi+14h]		//													0.0				m13				m12				m_2233_3223		m11
		fmul	ST, ST(1)				//													0.0				m13				m12				m_2233_3223		m11*m_2233_3223
		fld		dword ptr [m_2133_3123]	//									0.0				m13				m12				m_2233_3223		m11*m_2233_3223 m_2133_3123
		fmul	ST, ST(3)				//									0.0				m13				m12				m_2233_3223		m11*m_2233_3223 m12*m_2133_3123
		fsub							//													0.0				m13				m12				m_2233_3223		m11*m_2233_3223-m12*m_2133_3123
		fld		dword ptr [m_2132_3122]	//									0.0				m13				m12				m_2233_3223		m11*m_2233_3223-m12*m_2133_3123 m_2132_3122
		fmul	ST, ST(4)				//									0.0				m13				m12				m_2233_3223		m11*m_2233_3223-m12*m_2133_3123 m13*m_2132_3122
		fadd							//													0.0				m13				m12				m_2233_3223		m11*m_2233_3223-m12*m_2133_3123+m13*m_2132_3122=det1
		fmul	dword ptr [esi]			//													0.0				m13				m12				m_2233_3223		det1*m00=res
		faddp	ST(4), ST				//																	res				m13				m12				m_2233_3223
// needs work from here on
		fmul	dword ptr [esi+10h]		//																	res				m13				m12				m10*m_2233_3223
		fld		dword ptr [m_2033_3023]	//													res				m13				m12				m10*m_2233_3223	m_2033_3023
		fxch	ST(2), ST				//													res				m13				m_2033_3023		m10*m_2233_3223	m12
		fmul	ST, ST(2)				//													res				m13				m_2033_3023		m10*m_2233_3223	m12*m_2033_3023
		fsub							//																	res				m13				m_2033_3023		m10*m_2233_3223-m12*m_2033_3023
		fld		dword ptr [m_2032_3022]	//													res				m13				m_2033_3023		m10*m_2233_3223-m12*m_2033_3023	m_2032_3022
		fmul	ST, ST(3)				//													res				m13				m_2033_3023		m10*m_2233_3223-m12*m_2033_3023	m13*m_2032_3022
		fadd							//																	res				m13				m_2033_3023		m10*m_2233_3223-m12*m_2033_3023+m13*m_2032_3022=det
		fmul	dword ptr [esi+04h]		//																	res				m13				m_2033_3023		det*m01
		fsubp	ST(3), ST				//																					res				m13				m_2033_3023

		fld		dword ptr [esi+10h]

		det = mr._21 * mr_3244_4234 - mr._22 * mr_3144_4134 + mr._24 * mr_3142_4132;
		res += mr._13 * det;
	}
}*/

inline float DotProduct( float v0[3], float v1[3] )
{
	float	dot;
	dot = v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2];
	return dot;
}

#endif