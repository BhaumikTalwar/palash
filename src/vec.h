/*
 *  This Small Math module is heavily inspired by HandMadeMath to the point its like a 
 *  small subset of the original HandMadeMath lib (ignoring the simd stuff tho)
 *  Not focusing on SimD for now as its omething i want to learn before implemnting and do it myself
 * 
 *  Though one thing different from Handmade math is taht we have a row major storage and column vectore convention
 *  as opposed to the column major storage in HandmadeMath
 *    - Storage: row-major
 *    - Vectors: column vectors
 *    - Coordinate system: right-handed
 *    - Angles: radians
 *    - Transform order: M = T * R * S
 *    - Multiplication: v' = M * v
 *  ===========================================================================================
 *
 *  Ref HandMadeMath repo :- https://github.com/HandmadeMath/HandmadeMath/tree/master#
 *      - License :- MIT
 * */


#ifndef VEC_H
#define VEC_H

#include <math.h>
#include <stdbool.h>

#include "compiler.h"
#include "types.h"

#ifdef VEC_IMPLEMENTATION
    #define VEC_DEF
#else
    #define VEC_DEF static FORCE_INLINE
#endif

#define PI 3.14159265358979323846
#define PI32 3.14159265359f
#define DEG180 180.0
#define DEG18032 180.0f
#define TURNHALF 0.5
#define TURNHALF32 0.5f
#define RadToDeg ((float)(DEG180/PI))
#define RadToTurn ((float)(TURNHALF/PI))
#define DegToRad ((float)(PI/DEG180))
#define DegToTurn ((float)(TURNHALF/DEG180))
#define TurnToRad ((float)(PI/TURNHALF))
#define TurnToDeg ((float)(DEG180/TURNHALF))

#ifdef STRICT_MAX_MIN
    #define MAX(a, b) MAX_ST((a),(b))
    #define MIN(a, b) MIN_ST((a),(b))
#else
    #define MAX(a,b)  ((a > b) ? a : b)
    #define MIN(a,b)  ((a < b) ? a : b)
#endif

#define ABS(a)    ((a) > 0 ? (a) : -(a))
#define MOD(a, m) (((a) % (m)) >= 0 ? ((a) % (m)) : (((a) % (m)) + (m)))
#define SQUARE(x) ((x) * (x))

#define F32_EPSILON 1e-6f
#define NORMALIZE_EPSILON 1e-6f

typedef union {
    struct {
        f32 x, y;
    };

    struct {
        f32 u, v;
    };

    struct {
        f32 left, right;
    };

    struct {
        f32 width, height;
    };

    f32 data[2];
} Vec2;
STATIC_ASSERT((sizeof(Vec2) == 8), "Size of Vec3 must be 8 bytes");

typedef union {
    struct {
        f32 x, y, z;
    };

    struct {
        f32 r, g, b;
    };

    struct {
        f32 u, v, w;
    };
    
    struct {
        Vec2 xy;
        f32 _Ignored0;
    };

    struct {
        f32 _Ignored1;
        Vec2 yz;
    };

    struct {
        Vec2 uv;
        f32 _Ignored2;
    };

    struct {
        f32 _Ignored3;
        Vec2 vw;
    };

    f32 data[3];
} Vec3;
STATIC_ASSERT((sizeof(Vec3) == 12), "Size of Vec3 must be 12 bytes");

typedef union {
    struct {
        union {
            Vec3 xyz;
            struct {
                f32 x, y, z;
            };
        };

        f32 w;
    };

    struct {
        union {
            Vec3 rgb;
            struct {
                f32 r, g, b;
            };
        };

        f32 a;
    };

    struct {
        Vec2 xy;
        f32 _Ignored0;
        f32 _Ignored1;
    };

    struct {
        f32 _Ignored2;
        Vec2 yz;
        f32 _Ignored3;
    };

    struct {
        f32 _Ignored4;
        f32 _Ignored5;
        Vec2 zw;
    };

    f32 data[4];
} Vec4;
STATIC_ASSERT((sizeof(Vec4) == 16), "Size of Vec4 must be 16 bytes");

typedef union {
    f32 elems[2][2];
    Vec2 rows[2];
} Mat2;
STATIC_ASSERT((sizeof(Mat2) == 16), "Size of Mat2 must be 16 bytes");

typedef union {
    f32 elems[3][3];
    Vec3 rows[3];
} Mat3;
STATIC_ASSERT((sizeof(Mat3) == 36), "Size of Mat3 must be 36 bytes");

typedef union {
    f32 elems[4][4];
    Vec4 rows[4];
}Mat4;
STATIC_ASSERT((sizeof(Mat4) == 64), "Size of Mat4 must be 64 bytes");

#define vec2(_x,_y)         (Vec2){.x = (_x), .y = (_y)}
#define vec3(_x,_y,_z)      (Vec3){.x = (_x), .y = (_y), .z = (_z)}
#define vec4(_x,_y,_z,_w)   (Vec4){.x = (_x), .y = (_y), .z = (_z), .w = (_w)}
#define vec4V(_v3, _w)      (Vec4){.xyz = (_v3), .w = (_w)}
#define mat2(void)          (Mat2){0}
#define mat3(void)          (Mat3){0}
#define mat4(void)          (Mat4){0}

VEC_DEF bool NearlyEqualF32(f32 left, f32 right) {
    f32 diff = fabsf(left - right);
    if (diff <= F32_EPSILON) {
        return true;
    }

    f32 largest = fmaxf(fabsf(left), fabsf(right));
    return diff <= largest * F32_EPSILON;
}

VEC_DEF bool NearlyEqualV2(Vec2 left, Vec2 right) {
    return NearlyEqualF32(left.x, right.x) &&
           NearlyEqualF32(left.y, right.y);
}

VEC_DEF bool NearlyEqualV3(Vec3 left, Vec3 right) {
    return NearlyEqualF32(left.x, right.x) &&
           NearlyEqualF32(left.y, right.y) &&
           NearlyEqualF32(left.z, right.z
        );
}

VEC_DEF bool NearlyEqualV4(Vec4 left, Vec4 right) {
    return NearlyEqualF32(left.x, right.x) &&
           NearlyEqualF32(left.y, right.y) &&
           NearlyEqualF32(left.z, right.z &&
           NearlyEqualF32(left.w, right.w)
        );
}

VEC_DEF bool EqV2(Vec2 left, Vec2 right) {
    return (left.x == right.x && left.y == right.y);
}

VEC_DEF Vec2 AddV2(Vec2 left, Vec2 right) {
    return vec2((left.x + right.x), (left.y + right.y));
}

VEC_DEF Vec2 SubV2(Vec2 left, Vec2 right) {
    return vec2((left.x - right.x), (left.y - right.y));
}

VEC_DEF Vec2 MulV2(Vec2 left, Vec2 right) {
    return vec2((left.x * right.x), (left.y * right.y));
}

VEC_DEF Vec2 DivV2(Vec2 left, Vec2 right) {
    if ((right.x * right.y) == 0) {
        LOG("Division By Zero returning left vec");
        return left;
    }

    return vec2((left.x / right.x), (left.y / right.y));
}

VEC_DEF Vec2 MulV2F(Vec2 left, f32 right) {
    return vec2((left.x * right), (left.y * right));
}

VEC_DEF Vec2 DivV2F(Vec2 left, f32 right) {
    if (right == 0) {
        LOG("Division By Zero returning left vec");
        return left;
    }

    return vec2((left.x / right), (left.y / right));
}

VEC_DEF f32 DotV2(Vec2 left, Vec2  right) {
    return (left.x * right.x) + (left.y * right.y);
}

VEC_DEF f32 CrossV2(Vec2 left, Vec2  right) {
    return (left.x * right.y) - (left.y * right.x);
}

VEC_DEF f32 LenSqV2(Vec2 vec) {
    return (DotV2(vec,vec));
}

VEC_DEF f32 LenV2(Vec2 vec) {
    return sqrtf(LenSqV2(vec));
}

VEC_DEF Vec2 LerpV2(Vec2 a, f32 tm, Vec2 b) {
    return AddV2(MulV2F(a, 1.0f - tm), MulV2F(b, tm));
}

VEC_DEF Vec2 NormV2(Vec2 vec) {
    f32 len = LenV2(vec);
    if (len ==0) {
        LOG("Division By Zero returning left vec");
        return vec;
    }

    return MulV2F(vec, 1.0f/len);
}

VEC_DEF bool TryNormalizeV2(Vec2 v,Vec2 *out) {
    f32 len = LenV2(v);
    if (len < NORMALIZE_EPSILON) {
        return false;
    }

    *out = MulV2F(v, 1.0f/len);
    return true;
}

VEC_DEF bool EqV3(Vec3 left, Vec3 right) {
    return (left.x == right.x && left.y == right.y && left.z == right.z);
}

VEC_DEF Vec3 AddV3(Vec3 left, Vec3 right) {
    return vec3((left.x + right.x), (left.y + right.y), (left.z + right.z));
}

VEC_DEF Vec3 SubV3(Vec3 left, Vec3 right) {
    return vec3((left.x - right.x), (left.y - right.y), (left.z - right.z));
}

VEC_DEF Vec3 MulV3(Vec3 left, Vec3 right) {
    return vec3((left.x * right.x), (left.y * right.y), (left.z * right.z));
}

VEC_DEF Vec3 DivV3(Vec3 left, Vec3 right) {
    if ((right.x * right.y * right.z) == 0) {
        LOG("Division By Zero returning left vec");
        return left;
    }

    return vec3((left.x / right.x), (left.y / right.y), (left.z / right.z));
}

VEC_DEF Vec3 MulV3F(Vec3 left, f32 right) {
    return vec3((left.x * right), (left.y * right), (left.z * right));
}

VEC_DEF Vec3 DivV3F(Vec3 left, f32 right) {
    if (right == 0) {
        LOG("Division By Zero returning left vec");
        return left;
    }

    return vec3((left.x / right), (left.y / right), (left.z / right));
}

VEC_DEF f32 DotV3(Vec3 left, Vec3  right) {
    return (left.x * right.x) + (left.y * right.y) + (left.z * right.z);
}

VEC_DEF Vec3 CrossV3(Vec3 left, Vec3  right) {
    return vec3(
		(left.y * right.z) - (left.z * right.y),
		(left.z * right.x) - (left.x * right.z),
		(left.x * right.y) - (left.y * right.x)
    );
}

VEC_DEF f32 LenSqV3(Vec3 vec) {
    return (DotV3(vec,vec));
}

VEC_DEF f32 LenV3(Vec3 vec) {
    return sqrtf(LenSqV3(vec));
}

VEC_DEF Vec3 LerpV3(Vec3 a, f32 tm, Vec3 b) {
    return AddV3(MulV3F(a, 1.0f - tm), MulV3F(b, tm));
}

VEC_DEF Vec3 NormV3(Vec3 vec) {
    f32 len = LenV3(vec);
    if (len ==0) {
        LOG("Division By Zero returning left vec");
        return vec;
    }

    return MulV3F(vec, 1.0f/len);
}

VEC_DEF bool TryNormalizeV3(Vec3 v,Vec3 *out) {
    f32 len = LenV3(v);
    if (len < NORMALIZE_EPSILON) {
        return false;
    }

    *out = MulV3F(v, 1.0f/len);
    return true;
}

VEC_DEF bool EqV4(Vec4 left, Vec4 right) {
    return (
        left.x == right.x 
        && left.y == right.y 
        && left.z == right.z
        && left.w == right.w
    );
}

VEC_DEF Vec4 AddV4(Vec4 left, Vec4 right) {
    return vec4(
        (left.x + right.x), 
        (left.y + right.y), 
        (left.z + right.z),
        (left.w + right.w)
    );
}

VEC_DEF Vec4 SubV4(Vec4 left, Vec4 right) {
    return vec4(
        (left.x - right.x), 
        (left.y - right.y), 
        (left.z - right.z),
        (left.w - right.w)
    );
}

VEC_DEF Vec4 MulV4(Vec4 left, Vec4 right) {
    return vec4(
        (left.x * right.x), 
        (left.y * right.y), 
        (left.z * right.z),
        (left.w * right.w)
    );
}

VEC_DEF Vec4 DivV4(Vec4 left, Vec4 right) {
    if ((right.x * right.y * right.z * right.w) == 0) {
        LOG("Division By Zero returning left vec");
        return left;
    }

    return vec4(
        (left.x / right.x), 
        (left.y / right.y), 
        (left.z / right.z),
        (left.w / right.w)
    );
}

VEC_DEF Vec4 MulV4F(Vec4 left, f32 right) {
    return vec4(
        (left.x * right), 
        (left.y * right), 
        (left.z * right),
        (left.w * right)
    );
}

VEC_DEF Vec4 DivV4F(Vec4 left, f32 right) {
    if (right == 0) {
        LOG("Division By Zero returning left vec");
        return left;
    }

    return vec4(
        (left.x / right), 
        (left.y / right), 
        (left.z / right),
        (left.w / right)
    );
}

VEC_DEF f32 DotV4(Vec4 left, Vec4  right) {
    return 
        (left.x * right.x) 
        + (left.y * right.y) 
        + (left.z * right.z)
        + (left.w * right.w);
}

VEC_DEF f32 LenSqV4(Vec4 vec) {
    return (DotV4(vec,vec));
}

VEC_DEF f32 LenV4(Vec4 vec) {
    return sqrtf(LenSqV4(vec));
}

VEC_DEF Vec4 LerpV4(Vec4 a, f32 tm, Vec4 b) {
    return AddV4(MulV4F(a, 1.0f - tm), MulV4F(b, tm));
}

VEC_DEF Vec4 NormV4(Vec4 vec) {
    f32 len = LenV4(vec);
    if (len ==0) {
        LOG("Division By Zero returning left vec");
        return vec;
    }

    return MulV4F(vec, 1.0f/len);
}

VEC_DEF bool TryNormalizeV4(Vec4 v,Vec4 *out) {
    f32 len = LenV4(v);
    if (len < NORMALIZE_EPSILON) {
        return false;
    }

    *out = MulV4F(v, 1.0f/len);
    return true;
}

VEC_DEF Vec2 Mat2Column(Mat2 mat, usize col) {
    return vec2(
        mat.elems[0][col],
        mat.elems[1][col]
    );
}

VEC_DEF Mat2 Mat2Diagonal(f32 diag) {
    Mat2 out = mat2();

    out.elems[0][0] = diag;
    out.elems[1][1] = diag;
    return out;
};

VEC_DEF Mat2 Mat2Transpose(Mat2 mat) {
    Mat2 out = mat;
    
    out.elems[0][1] = mat.elems[1][0];
    out.elems[1][0] = mat.elems[0][1];
    return out;
}

VEC_DEF f32 Mat2Determinat(Mat2 mat) {
    return (mat.elems[0][0] * mat.elems[1][1]) 
            - (mat.elems[0][1] * mat.elems[1][0]);
}

VEC_DEF Mat2 Mat2InvGen(Mat2 mat) {
    f32 det = Mat2Determinat(mat);
    if (det == 0.0f) {
        LOG("Cant divde by zero, returning mat"); 
        return mat;
    }

    f32 invDet = 1.0f / det;
    Mat2 out = {0};
    out.elems[0][0] = invDet * +mat.elems[1][1];
    out.elems[1][1] = invDet * +mat.elems[0][0];
    out.elems[0][1] = invDet * -mat.elems[0][1];
    out.elems[1][0] = invDet * -mat.elems[1][0];

    return out;
}

VEC_DEF Mat2 AddMat2(Mat2 left, Mat2 right) {
    Mat2 out = {0};

    out.elems[0][0] = left.elems[0][0] + right.elems[0][0];
    out.elems[0][1] = left.elems[0][1] + right.elems[0][1];
    out.elems[1][0] = left.elems[1][0] + right.elems[1][0];
    out.elems[1][1] = left.elems[1][1] + right.elems[1][1];
    return out;
}

VEC_DEF Mat2 SubMat2(Mat2 left, Mat2 right) {
    Mat2 out = {0};

    out.elems[0][0] = left.elems[0][0] - right.elems[0][0];
    out.elems[0][1] = left.elems[0][1] - right.elems[0][1];
    out.elems[1][0] = left.elems[1][0] - right.elems[1][0];
    out.elems[1][1] = left.elems[1][1] - right.elems[1][1];
    return out;
}

VEC_DEF Vec2 MulMat2Vec2(Mat2 left, Vec2 right) {
    Vec2 out;

    out.x = left.rows[0].x * right.x +
            left.rows[0].y * right.y;

    out.y = left.rows[1].x * right.x +
            left.rows[1].y * right.y;

    return out;
}

VEC_DEF Mat2 MulMat2(Mat2 left, Mat2 right) {
    Vec2 col0 = Mat2Column(right, 0);
    Vec2 col1 = Mat2Column(right, 1);

    Mat2 out;
    out.elems[0][0] = DotV2(left.rows[0], col0);
    out.elems[0][1] = DotV2(left.rows[0], col1);

    out.elems[1][0] = DotV2(left.rows[1], col0);
    out.elems[1][1] = DotV2(left.rows[1], col1);

    return out;
}

VEC_DEF Mat2 MulMat2F(Mat2 left, f32 right) {
    Mat2 out = {0};

    out.elems[0][0] = left.elems[0][0] * right; 
    out.elems[0][1] = left.elems[0][1] * right; 
    out.elems[1][0] = left.elems[1][0] * right; 
    out.elems[1][1] = left.elems[1][1] * right; 
    return out;
}

VEC_DEF Mat2 DivMat2F(Mat2 left, f32 right) {
    if (right == 0.0f) {
        LOG("Cant Divide By zero");
        return left;
    }

    right = 1.0f / right;
    return MulMat2F(left,right);
}

VEC_DEF Vec3 Mat3Column(Mat3 mat, usize col) {
    return vec3(
        mat.elems[0][col],
        mat.elems[1][col],
        mat.elems[2][col]
    );
}

VEC_DEF Mat3 Mat3Diagonal(f32 diag) {
    Mat3 out = mat3();

    out.elems[0][0] = diag;
    out.elems[1][1] = diag;
    out.elems[2][2] = diag;
    return out;
};

VEC_DEF Mat3 Mat3Transpose(Mat3 mat) {
    Mat3 out = mat;
    
    out.elems[0][1] = mat.elems[1][0];
    out.elems[0][2] = mat.elems[2][0];
    out.elems[1][0] = mat.elems[0][1];
    out.elems[1][2] = mat.elems[2][1];
    out.elems[2][1] = mat.elems[1][2];
    out.elems[2][0] = mat.elems[0][2];
    return out;
}

VEC_DEF f32 Mat3Determinat(Mat3 mat) {
    f32 a = mat.elems[0][0];
    f32 b = mat.elems[0][1];
    f32 c = mat.elems[0][2];

    f32 d = mat.elems[1][0];
    f32 e = mat.elems[1][1];
    f32 f = mat.elems[1][2];

    f32 g = mat.elems[2][0];
    f32 h = mat.elems[2][1];
    f32 i = mat.elems[2][2];

    return
        a * (e * i - f * h) -
        b * (d * i - f * g) +
        c * (d * h - e * g);
}

VEC_DEF Mat3 Mat3InvGen(Mat3 m) {
    f32 a = m.elems[0][0];
    f32 b = m.elems[0][1];
    f32 c = m.elems[0][2];

    f32 d = m.elems[1][0];
    f32 e = m.elems[1][1];
    f32 f = m.elems[1][2];

    f32 g = m.elems[2][0];
    f32 h = m.elems[2][1];
    f32 i = m.elems[2][2];

    f32 det =
          a * (e * i - f * h)
        - b * (d * i - f * g)
        + c * (d * h - e * g);

    if (fabsf(det) < F32_EPSILON) {
        LOG("Matrix is singular, returning Identity Matrix");
        return Mat3Diagonal(1.0f);
    }

    f32 invDet = 1.0f / det;

    Mat3 out;

    out.elems[0][0] = +(e*i - f*h) * invDet;
    out.elems[0][1] = -(b*i - c*h) * invDet;
    out.elems[0][2] = +(b*f - c*e) * invDet;

    out.elems[1][0] = -(d*i - f*g) * invDet;
    out.elems[1][1] = +(a*i - c*g) * invDet;
    out.elems[1][2] = -(a*f - c*d) * invDet;

    out.elems[2][0] = +(d*h - e*g) * invDet;
    out.elems[2][1] = -(a*h - b*g) * invDet;
    out.elems[2][2] = +(a*e - b*d) * invDet;

    return out;
}

VEC_DEF Mat3 AddMat3(Mat3 left, Mat3 right) {
    Mat3 out = {0};

    out.elems[0][0] = left.elems[0][0] + right.elems[0][0];
    out.elems[0][1] = left.elems[0][1] + right.elems[0][1];
    out.elems[0][2] = left.elems[0][2] + right.elems[0][2];
    out.elems[1][0] = left.elems[1][0] + right.elems[1][0];
    out.elems[1][1] = left.elems[1][1] + right.elems[1][1];
    out.elems[1][2] = left.elems[1][2] + right.elems[1][2];
    out.elems[2][0] = left.elems[2][0] + right.elems[2][0];
    out.elems[2][1] = left.elems[2][1] + right.elems[2][1];
    out.elems[2][2] = left.elems[2][2] + right.elems[2][2];
    return out;
}

VEC_DEF Mat3 SubMat3(Mat3 left, Mat3 right) {
    Mat3 out = {0};

    out.elems[0][0] = left.elems[0][0] - right.elems[0][0];
    out.elems[0][1] = left.elems[0][1] - right.elems[0][1];
    out.elems[0][2] = left.elems[0][2] - right.elems[0][2];
    out.elems[1][0] = left.elems[1][0] - right.elems[1][0];
    out.elems[1][1] = left.elems[1][1] - right.elems[1][1];
    out.elems[1][2] = left.elems[1][2] - right.elems[1][2];
    out.elems[2][0] = left.elems[2][0] - right.elems[2][0];
    out.elems[2][1] = left.elems[2][1] - right.elems[2][1];
    out.elems[2][2] = left.elems[2][2] - right.elems[2][2];
    return out;
}

VEC_DEF Vec3 MulMat3Vec3(Mat3 left, Vec3 right) {
    return vec3(
        DotV3(left.rows[0], right),
        DotV3(left.rows[1], right),
        DotV3(left.rows[2], right)
    );
}

VEC_DEF Mat3 MulMat3(Mat3 left, Mat3 right) {
    Mat3 out = {0};

    out.rows[0] = vec3(
        DotV3(left.rows[0], Mat3Column(right, 0)),
        DotV3(left.rows[0], Mat3Column(right, 1)),
        DotV3(left.rows[0], Mat3Column(right, 2))
    );

    out.rows[1] = vec3(
        DotV3(left.rows[1], Mat3Column(right, 0)),
        DotV3(left.rows[1], Mat3Column(right, 1)),
        DotV3(left.rows[1], Mat3Column(right, 2))
    );

    out.rows[2] = vec3(
        DotV3(left.rows[2], Mat3Column(right, 0)),
        DotV3(left.rows[2], Mat3Column(right, 1)),
        DotV3(left.rows[2], Mat3Column(right, 2))
    );

    return out;
}

VEC_DEF Mat3 MulMat3F(Mat3 left, f32 right) {
    Mat3 out = {0};

    out.elems[0][0] = left.elems[0][0] * right;
    out.elems[0][1] = left.elems[0][1] * right;
    out.elems[0][2] = left.elems[0][2] * right;
    out.elems[1][0] = left.elems[1][0] * right;
    out.elems[1][1] = left.elems[1][1] * right;
    out.elems[1][2] = left.elems[1][2] * right;
    out.elems[2][0] = left.elems[2][0] * right;
    out.elems[2][1] = left.elems[2][1] * right;
    out.elems[2][2] = left.elems[2][2] * right;
    return out;
}

VEC_DEF Mat3 DivMat3F(Mat3 left, f32 right) {
    if (right == 0.0f) {
        LOG("Cant Divide By zero");
        return left;
    }

    right = 1.0f / right;
    return MulMat3F(left,right);
}


VEC_DEF Vec4 Mat4Column(Mat4 mat, usize col) {
    return vec4(
        mat.elems[0][col],
        mat.elems[1][col],
        mat.elems[2][col],
        mat.elems[3][col]
    );
}

VEC_DEF Mat4 Mat4Diagonal(f32 diag) {
    Mat4 out = mat4();

    out.elems[0][0] = diag;
    out.elems[1][1] = diag;
    out.elems[2][2] = diag;
    out.elems[3][3] = diag;
    return out;
};

VEC_DEF Mat4 Mat4Transpose(Mat4 mat) {
    Mat4 out = mat;
    
    out.elems[0][0] = mat.elems[0][0];
    out.elems[0][1] = mat.elems[1][0];
    out.elems[0][2] = mat.elems[2][0];
    out.elems[0][3] = mat.elems[3][0];
    out.elems[1][0] = mat.elems[0][1];
    out.elems[1][1] = mat.elems[1][1];
    out.elems[1][2] = mat.elems[2][1];
    out.elems[1][3] = mat.elems[3][1];
    out.elems[2][0] = mat.elems[0][2];
    out.elems[2][1] = mat.elems[1][2];
    out.elems[2][2] = mat.elems[2][2];
    out.elems[2][3] = mat.elems[3][2];
    out.elems[3][0] = mat.elems[0][3];
    out.elems[3][1] = mat.elems[1][3];
    out.elems[3][2] = mat.elems[2][3];
    out.elems[3][3] = mat.elems[3][3];
    return out;
}

VEC_DEF f32 Mat4Determinant(Mat4 mat) {
    Vec4 c0 = Mat4Column(mat, 0);
    Vec4 c1 = Mat4Column(mat, 1);
    Vec4 c2 = Mat4Column(mat, 2);
    Vec4 c3 = Mat4Column(mat, 3);

    Vec3 C01 = CrossV3(c0.xyz, c1.xyz);
    Vec3 C23 = CrossV3(c2.xyz, c3.xyz);

    Vec3 B10 =
        SubV3(
            MulV3F(c0.xyz, c1.w),
            MulV3F(c1.xyz, c0.w)
        );

    Vec3 B32 =
        SubV3(
            MulV3F(c2.xyz, c3.w),
            MulV3F(c3.xyz, c2.w)
        );

    return DotV3(C01, B32) +
           DotV3(C23, B10);
}

VEC_DEF Mat4 Mat4InvGen(Mat4 mat) {
    Vec4 c0 = Mat4Column(mat, 0);
    Vec4 c1 = Mat4Column(mat, 1);
    Vec4 c2 = Mat4Column(mat, 2);
    Vec4 c3 = Mat4Column(mat, 3);

    Vec3 C01 = CrossV3(c0.xyz, c1.xyz);
    Vec3 C23 = CrossV3(c2.xyz, c3.xyz);

    Vec3 B10 =
        SubV3(
            MulV3F(c0.xyz, c1.w),
            MulV3F(c1.xyz, c0.w)
        );

    Vec3 B32 =
        SubV3(
            MulV3F(c2.xyz, c3.w),
            MulV3F(c3.xyz, c2.w)
        );

    f32 det = DotV3(C01, B32) +
           DotV3(C23, B10);

    if (fabsf(det) < F32_EPSILON) {
        LOG("Det is Zero, returning identity");
        return Mat4Diagonal(1.0f); 
    }

    f32 invDet = 1.0f / det;
    C01 = MulV3F(C01, invDet);
    C23 = MulV3F(C23, invDet);
    B10 = MulV3F(B10, invDet);
    B32 = MulV3F(B32, invDet);

    Vec3 t0 = AddV3(
        CrossV3(c1.xyz, B32),
        MulV3F(C23, c1.w)
    );

    Vec3 t1 = SubV3(
        CrossV3(B32, c0.xyz),
        MulV3F(C23, c0.w)
    );

    Vec3 t2 = AddV3(
        CrossV3(c3.xyz, B10),
        MulV3F(C01, c3.w)
    );

    Vec3 t3 = SubV3(
        CrossV3(B10, c2.xyz),
        MulV3F(C01, c2.w)
    );

    Mat4 out = {0};
    out.rows[0] = vec4(t0.x, t0.y, t0.z,
                       -DotV3(c1.xyz, C23));

    out.rows[1] = vec4(t1.x, t1.y, t1.z,
                       +DotV3(c0.xyz, C23));

    out.rows[2] = vec4(t2.x, t2.y, t2.z,
                       -DotV3(c3.xyz, C01));

    out.rows[3] = vec4(t3.x, t3.y, t3.z,
                       +DotV3(c2.xyz, C01));

    return out;
}

VEC_DEF Mat4 AddMat4(Mat4 left, Mat4 right) {
    Mat4 out = {0};

    out.rows[0] = AddV4(left.rows[0], right.rows[0]);
    out.rows[1] = AddV4(left.rows[1], right.rows[1]);
    out.rows[2] = AddV4(left.rows[2], right.rows[2]);
    out.rows[3] = AddV4(left.rows[3], right.rows[3]);
    return out;
}

VEC_DEF Mat4 SubMat4(Mat4 left, Mat4 right) {
    Mat4 out = {0};

    out.rows[0] = SubV4(left.rows[0], right.rows[0]);
    out.rows[1] = SubV4(left.rows[1], right.rows[1]);
    out.rows[2] = SubV4(left.rows[2], right.rows[2]);
    out.rows[3] = SubV4(left.rows[3], right.rows[3]);
    return out;
}

VEC_DEF Vec4 MulMat4Vec4(Mat4 left, Vec4 right) {
    return vec4(
        DotV4(left.rows[0], right),
        DotV4(left.rows[1], right),
        DotV4(left.rows[2], right),
        DotV4(left.rows[3], right)
    );
}

VEC_DEF Mat4 MulMat4(Mat4 left, Mat4 right) {
    Vec4 c0 = Mat4Column(right, 0);
    Vec4 c1 = Mat4Column(right, 1);
    Vec4 c2 = Mat4Column(right, 2);
    Vec4 c3 = Mat4Column(right, 3);

    Mat4 out;
    out.rows[0] = vec4(
        DotV4(left.rows[0], c0),
        DotV4(left.rows[0], c1),
        DotV4(left.rows[0], c2),
        DotV4(left.rows[0], c3)
    );

    out.rows[1] = vec4(
        DotV4(left.rows[1], c0),
        DotV4(left.rows[1], c1),
        DotV4(left.rows[1], c2),
        DotV4(left.rows[1], c3)
    );

    out.rows[2] = vec4(
        DotV4(left.rows[2], c0),
        DotV4(left.rows[2], c1),
        DotV4(left.rows[2], c2),
        DotV4(left.rows[2], c3)
    );

    out.rows[3] = vec4(
        DotV4(left.rows[3], c0),
        DotV4(left.rows[3], c1),
        DotV4(left.rows[3], c2),
        DotV4(left.rows[3], c3)
    );

    return out;
}

VEC_DEF Mat4 MulMat4F(Mat4 left, f32 right) {
    Mat4 out = {0};

    out.elems[0][0] = left.elems[0][0] * right;
    out.elems[0][1] = left.elems[0][1] * right;
    out.elems[0][2] = left.elems[0][2] * right;
    out.elems[0][3] = left.elems[0][3] * right;
    out.elems[1][0] = left.elems[1][0] * right;
    out.elems[1][1] = left.elems[1][1] * right;
    out.elems[1][2] = left.elems[1][2] * right;
    out.elems[1][3] = left.elems[1][3] * right;
    out.elems[2][0] = left.elems[2][0] * right;
    out.elems[2][1] = left.elems[2][1] * right;
    out.elems[2][2] = left.elems[2][2] * right;
    out.elems[2][3] = left.elems[2][3] * right;
    out.elems[3][0] = left.elems[3][0] * right;
    out.elems[3][1] = left.elems[3][1] * right;
    out.elems[3][2] = left.elems[3][2] * right;
    out.elems[3][3] = left.elems[3][3] * right;
    return out;
}

VEC_DEF Mat4 DivMat4F(Mat4 left, f32 right) {
    if (right == 0.0f) {
        LOG("Cant Divide By zero");
        return left;
    }

    right = 1.0f / right;
    return MulMat4F(left,right);
}

VEC_DEF Mat4 Translate(Vec3 tr) {
    Mat4 out = Mat4Diagonal(1.0f);
    
    out.elems[0][3] = tr.x;
    out.elems[1][3] = tr.y;
    out.elems[2][3] = tr.z;
    return out;
}

VEC_DEF Mat4 InvTranslate(Mat4 trmat) {
    Mat4 out = trmat;

    out.elems[0][3] =   -trmat.elems[0][3];
    out.elems[1][3] =   -trmat.elems[1][3]; 
    out.elems[2][3] =   -trmat.elems[2][3]; 

    return out;
}

VEC_DEF Mat4 Scale(Vec3 tr) {
    Mat4 out = Mat4Diagonal(1.0f);

    out.elems[0][0] = tr.x;
    out.elems[1][1] = tr.y;
    out.elems[2][2] = tr.z;

    return out;
}

VEC_DEF Mat4 InvScale(Mat4 trmat) {
    f32 d0 = trmat.elems[0][0]; 
    f32 d1 = trmat.elems[1][1]; 
    f32 d2 = trmat.elems[2][2]; 

    if (d0 == 0 || d1 == 0 || d2 == 0) {
        LOG("Cant divide by zero Scale: returning Original Scale");
        return trmat;
    }

    Mat4 out = trmat;
    out.elems[0][0] = 1.0f / d0;
    out.elems[1][1] = 1.0f / d1;
    out.elems[2][2] = 1.0f / d2;

    return out;
}

VEC_DEF Mat4 RotateX(f32 angle) {
    f32 s = sinf(angle);
    f32 c = cosf(angle);

    Mat4 out = Mat4Diagonal(1.0f);

    out.elems[1][1] = c;
    out.elems[1][2] = -s;

    out.elems[2][1] = s;
    out.elems[2][2] = c;

    return out;
}

VEC_DEF Mat4 RotateY(f32 angle) {
    f32 s = sinf(angle);
    f32 c = cosf(angle);

    Mat4 out = Mat4Diagonal(1.0f);

    out.elems[0][0] = c;
    out.elems[0][2] = s;

    out.elems[2][0] = -s;
    out.elems[2][2] = c;

    return out;
}

VEC_DEF Mat4 RotateZ(f32 angle){
    f32 s = sinf(angle);
    f32 c = cosf(angle);

    Mat4 out = Mat4Diagonal(1.0f);

    out.elems[0][0] = c;
    out.elems[0][1] = -s;

    out.elems[1][0] = s;
    out.elems[1][1] = c;

    return out;
}

VEC_DEF Mat4 RotateRH(f32 angle, Vec3 axis) {
    axis = NormV3(axis);

    f32 s = sinf(angle);
    f32 c = cosf(angle);
    f32 t = 1.0f - c;

    f32 x = axis.x;
    f32 y = axis.y;
    f32 z = axis.z;

    Mat4 out = Mat4Diagonal(1.0f);

    out.elems[0][0] = t*x*x + c;
    out.elems[0][1] = t*x*y + s*z;
    out.elems[0][2] = t*x*z - s*y;

    out.elems[1][0] = t*y*x - s*z;
    out.elems[1][1] = t*y*y + c;
    out.elems[1][2] = t*y*z + s*x;

    out.elems[2][0] = t*z*x + s*y;
    out.elems[2][1] = t*z*y - s*x;
    out.elems[2][2] = t*z*z + c;

    return out;
}

VEC_DEF Mat4 RotateLH(f32 angle, Vec3 axis) {
    return RotateRH(-angle, axis);
}

VEC_DEF Mat4 InvRotate(Mat4 rot) {
    return Mat4Transpose(rot);
}

VEC_DEF Mat4 RotateXYZ(Vec3 a) {
    return MulMat4(
            RotateZ(a.z), 
            MulMat4(
                RotateY(a.y), RotateX(a.x)
            )
        );
}

VEC_DEF Mat4 RotateZYX(Vec3 a) {
    return MulMat4(
            RotateX(a.x), 
            MulMat4(
                RotateY(a.y), RotateZ(a.z)
            )
        );
}

VEC_DEF Mat4 LookAt(Vec3 f, Vec3 s, Vec3 u,Vec3 eye) {
    Mat4 out = Mat4Diagonal(1.0f);

    out.elems[0][0] =  s.x;
    out.elems[0][1] =  s.y; 
    out.elems[0][2] =  s.z;

    out.elems[1][0] =  u.x; 
    out.elems[1][1] =  u.y; 
    out.elems[1][2] =  u.z; 

    out.elems[2][0] = -f.x; 
    out.elems[2][1] = -f.y; 
    out.elems[2][2] = -f.z; 

    out.elems[0][3] = -DotV3(s, eye);
    out.elems[1][3] = -DotV3(u, eye);
    out.elems[2][3] =  DotV3(f, eye);

    return out;
}

VEC_DEF Mat4 LookAtRH(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f = NormV3(SubV3(center, eye));
    Vec3 s = NormV3(CrossV3(f, up));
    Vec3 u = CrossV3(s, f);

    return LookAt(f,  s,  u,  eye);
}

VEC_DEF Mat4 LookAtLH(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f = NormV3(SubV3(eye, center));
    Vec3 s = NormV3(CrossV3(f, up));
    Vec3 u = CrossV3(s, f);

    return LookAt(f,  s,  u,  eye);
}

VEC_DEF Mat4 PerspectiveRH_ZO(
    f32 fov,
    f32 aspect,
    f32 near,
    f32 far
){
    f32 cot = 1.0f / tanf(fov * 0.5f);

    Mat4 out = {0};

    out.elems[0][0] = cot / aspect;
    out.elems[1][1] = cot;

    out.elems[2][2] = far / (near - far);
    out.elems[2][3] = (near * far) / (near - far);

    out.elems[3][2] = -1.0f;

    return out;
}

VEC_DEF Mat4 PerspectiveRH_NO(
    f32 fov,
    f32 aspect,
    f32 near,
    f32 far
){
    f32 cot = 1.0f / tanf(fov * 0.5f);

    Mat4 out = {0};

    out.elems[0][0] = cot / aspect;
    out.elems[1][1] = cot;

    out.elems[2][2] = (near + far) / (near - far);
    out.elems[2][3] = (2.0f * near * far) / (near - far);

    out.elems[3][2] = -1.0f;

    return out;
}

VEC_DEF Mat4 PerspectiveLH_ZO(
    f32 fov,
    f32 aspect,
    f32 near,
    f32 far
){
    Mat4 out = PerspectiveRH_ZO(fov, aspect, near, far);

    out.elems[2][2] = -out.elems[2][2];
    out.elems[3][2] = -out.elems[3][2];

    return out;
}

VEC_DEF Mat4 PerspectiveLH_NO(
    f32 fov,
    f32 aspect,
    f32 near,
    f32 far
){
    Mat4 out = PerspectiveRH_NO(fov, aspect, near, far);

    out.elems[2][2] = -out.elems[2][2];
    out.elems[3][2] = -out.elems[3][2];

    return out;
}

VEC_DEF Mat4 OrthographicRH_ZO(
    f32 left,
    f32 right,
    f32 bottom,
    f32 top,
    f32 near,
    f32 far
){
    Mat4 out = {0};

    out.elems[0][0] = 2.0f / (right - left);
    out.elems[1][1] = 2.0f / (top - bottom);
    out.elems[2][2] = 1.0f / (near - far);
    out.elems[3][3] = 1.0f;

    out.elems[0][3] = (left + right) / (left - right);
    out.elems[1][3] = (bottom + top) / (bottom - top);
    out.elems[2][3] = near / (near - far);

    return out;
}

VEC_DEF Mat4 OrthographicRH_NO(
    f32 left,
    f32 right,
    f32 bottom,
    f32 top,
    f32 near,
    f32 far
){
    Mat4 out = {0};

    out.elems[0][0] = 2.0f / (right - left);
    out.elems[1][1] = 2.0f / (top - bottom);
    out.elems[2][2] = 2.0f / (near - far);
    out.elems[3][3] = 1.0f;

    out.elems[0][3] = (left + right) / (left - right);
    out.elems[1][3] = (bottom + top) / (bottom - top);
    out.elems[2][3] = (near + far) / (near - far);

    return out;
}

VEC_DEF Mat4 TRS(Vec3 t, Vec3 r, Vec3 s){
    return MulMat4(
        Translate(t),
        MulMat4(
            RotateXYZ(r),
            Scale(s)
        )
    );
}


VEC_DEF Mat4 VP(Mat4 view, Mat4 proj){
    return MulMat4(proj, view);
}

VEC_DEF Mat4 MVP(Mat4 model, Mat4 view, Mat4 proj){
    return MulMat4(
        proj,
        MulMat4(view, model)
    );
}

#endif // !VEC_H
