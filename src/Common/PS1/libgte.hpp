#pragma once

namespace PS1::libgte
{
	struct MATRIX
	{
		short m[3][3];
		long t[3];
	};

	struct SVECTOR
	{
		short vx, vy, vz;
		short pad;
	};

	struct VECTOR
	{
		long vx, vy, vz;
		long pad;
	};

	void VectorNormal(VECTOR* v0, VECTOR* v1);
	void VectorNormalSS(SVECTOR* v0, SVECTOR* v1);
	VECTOR* ApplyMatrixLV(MATRIX* m, VECTOR* v0, VECTOR* v1);
	MATRIX* MulMatrix0(MATRIX* m0, MATRIX* m1, MATRIX* m2);
}