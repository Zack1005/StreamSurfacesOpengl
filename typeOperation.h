#ifndef TYPE_OPERATION_H
#define TYPE_OPERATION_H

#include <cmath>
#include <cuda_runtime.h>
#include <fstream>
#include <sstream>
#include "helper_cuda.h"
#include <map>
#include <algorithm>
#include <vector>

typedef struct{
	float x, y;
} vec2f;

typedef struct{
	union{
		struct{
			union{
				vec2f xy;
				struct {
					float x, y;
				};
			};
			float z;
		};
		struct{
			float r, g, b;
		};
	};
} vec3f;

typedef struct{
	float s[3];
} vec3fa;

typedef struct{
	double s[3];
} vec3da;

typedef struct{
	union{
		vec3f xyz;
		struct{
			union{
				vec2f xy;
				struct {
					float x, y;
				};
			};
			float z;
		};
		vec3f rgb;
		struct{
			float r, g, b;
		};
	};
	union{
		float w;
		float a;
	};
} vec4f;

typedef struct{
	float s[4];
} vec4fa;

typedef struct{
	int x, y;
} vec2i;

typedef struct{
	int x, y, z;
} vec3i;

typedef struct{
	int s[3];
} vec3ia;

typedef struct{
	int x, y, z, w;
} vec4i;

typedef struct{
	int s[4];
} vec4ia;

typedef struct{
	vec4f m[3];
} vec3x4f;

typedef struct{
	union{
		vec2f pos;
		struct {
			float x, y;
		};
	};
	union{
		vec2f size;
		struct{
			float w, h;
		};
	};
} Area;

typedef struct{
	int lower, upper;
} IndexRange;

typedef struct{
	int sid, start, numPoint;
} Streamline;

typedef struct{
	int sid;
	int pid;
} StreamlineClickInfo;

typedef struct{
	int numStl, numPoint;
	Streamline *stlArray;
	vec3f *pointArray;
} StreamlinePool;

typedef struct{
	int vertex[2];
	int nbrGrid[4];
	int direction;
} EdgeInfoItem;

typedef struct{
	float val;
	int idx;
} sortElemInc;

typedef struct{
	int timeline_start, timeline_num;
	int vertex_start, vertex_num;
	int prim_start, prim_num;
} Surface;

inline __host__ __device__ Surface makeSurface(const int& timeline_start, const int& timeline_num, 
							 const int& vertex_start, const int& vertex_num,
							 const int& prim_start, const int& prim_num)
{
	Surface ret = {timeline_start, timeline_num, vertex_start, vertex_num, prim_start, prim_num};
	return ret;
}


inline __host__ __device__ sortElemInc makeSortElemInc(const float& val, const int& idx){
	sortElemInc ret = {val, idx};
	return ret;
}

inline __host__ __device__ bool operator < (const sortElemInc& a, const sortElemInc& b){
	return (a.val<b.val);
}

typedef struct{
	float val;
	int idx;
} sortElemDec;

inline __host__ __device__ sortElemDec makeSortElemDec(const float& val, const int& idx){
	sortElemDec ret = {val, idx};
	return ret;
}

inline __host__ __device__ bool operator < (const sortElemDec& a, const sortElemDec& b){
	return (a.val>b.val);
}

inline __host__ __device__ IndexRange makeIndexRange(const int& lower, const int& upper){
	IndexRange ret = {lower, upper};
	return ret;
}

inline __host__ __device__ IndexRange outerRangeBound(const IndexRange& a, const IndexRange& b){
	if (a.lower<0) {
		return b;
	} else if (b.lower<0){
		return a;
	}
	IndexRange ret;
	ret.lower = (a.lower<b.lower)?a.lower:b.lower;
	ret.upper = (a.upper>b.upper)?a.upper:b.upper;
	return ret;
}

typedef enum{
	STREAMLINE_TRACE_VELOCITY = 0,
	STREAMLINE_TRACE_NORMAL,
	STREAMLINE_TRACE_BINORMAL
} StreamlineTraceDirectionType;

typedef struct _StreamlineTraceParameter{
	int max_streamline;
	int max_point;
	int min_point;
	int max_step;
	float segment_length;
	float trace_interval;
	int trace_type;

	_StreamlineTraceParameter(){
		max_streamline = 200;
		max_point = 500;
		min_point = 50;
		max_step = 5000;
		segment_length = 0.5f;
		trace_interval = 0.1f;
		trace_type = (int)STREAMLINE_TRACE_VELOCITY;
	}
} StreamlineTraceParameter;

//conversions
inline vec3f makeVec3f(const vec3i& v){
	vec3f ret = {(float)v.x, (float)v.y, (float)v.z};
	return ret;
}

inline vec3i makeVec3i(const vec3f& v){
	vec3i ret = {(int)v.x, (int)v.y, (int)v.z};
	return ret;
}



//-----------------------------------------------------------------
//------------------    float type    -----------------------------
//-----------------------------------------------------------------
inline __host__ __device__ vec2f makeVec2f(const float &x, const float &y){
	vec2f ret = {x, y};
	return ret;
}

inline __host__ __device__ vec2f makeVec2f(const vec2i& v){
	vec2f ret = {v.x, v.y};
	return ret;
}

inline __host__ __device__ vec3f makeVec3f(const vec2f &xy, const float &z){
	vec3f ret = {xy.x, xy.y, z};
	return ret;
}

inline __host__ __device__ vec3f makeVec3f(const float &x, const float &y, const float &z){
	vec3f ret = {x, y, z};
	return ret;
}

inline __host__ __device__ vec3f makeVec3f(const float &v){
	vec3f ret = {v, v, v};
	return ret;
}

inline __host__ __device__ vec3f makeVec3f(const vec4f &v){
	vec3f ret = {v.x, v.y, v.z};
	return ret;
}

inline __host__ __device__ vec4f makeVec4f(const float &x, const float &y, const float &z, const float &w){
	vec4f ret = {x, y, z, w};
	return ret;
}

inline __host__ __device__ vec4f makeVec4f(const vec3f& xyz, const float &w){
	vec4f ret = {xyz.x, xyz.y, xyz.z, w};
	return ret;
}

inline __host__ __device__ vec4f makeVec4f(const float &v){
	vec4f ret = {v, v, v, v};
	return ret;
}



inline __host__ __device__ vec2f operator * (const float &scale, const vec2f &vec){
	vec2f ret = {scale*vec.x, scale*vec.y};
	return ret;
}

inline __host__ __device__ vec3f operator * (const float &scale, const vec3f &vec){
	vec3f ret = {scale*vec.x, scale*vec.y, scale*vec.z};
	return ret;
}

inline __host__ __device__ vec4f operator * (const float &scale, const vec4f &vec){
	vec4f ret = {scale*vec.x, scale*vec.y, scale*vec.z, scale*vec.w};
	return ret;
}

inline __host__ __device__ vec2f operator * (const vec2f &vec, const float &scale){
	vec2f ret = {scale*vec.x, scale*vec.y};
	return ret;
}

inline __host__ __device__ vec3f operator * (const vec3f &vec, const float &scale){
	vec3f ret = {scale*vec.x, scale*vec.y, scale*vec.z};
	return ret;
}

inline __host__ __device__ vec4f operator * (const vec4f &vec, const float &scale){
	vec4f ret = {scale*vec.x, scale*vec.y, scale*vec.z, scale*vec.w};
	return ret;
}

inline __host__ __device__ vec2f operator + (const vec2f &vec1, const vec2f &vec2){
	vec2f ret = {vec1.x+vec2.x, vec1.y+vec2.y};
	return ret;
}

inline __host__ __device__ vec3f operator + (const vec3f &vec1, const vec3f &vec2){
	vec3f ret = {vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z};
	return ret;
}

inline __host__ __device__ vec4f operator + (const vec4f &vec1, const vec4f &vec2){
	vec4f ret = {vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z, vec1.w+vec2.w};
	return ret;
}

inline __host__ __device__ vec2f operator - (const vec2f &vec1, const vec2f &vec2){
	vec2f ret = {vec1.x-vec2.x, vec1.y-vec2.y};
	return ret;
}

inline __host__ __device__ vec3f operator - (const vec3f &vec1, const vec3f &vec2){
	vec3f ret = {vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z};
	return ret;
}

inline __host__ __device__ vec4f operator - (const vec4f &vec1, const vec4f &vec2){
	vec4f ret = {vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z, vec1.w-vec2.w};
	return ret;
}

inline __host__ __device__ vec2f operator - (const vec2f &vec){
	vec2f ret = {-vec.x, -vec.y};
	return ret;
}

inline __host__ __device__ vec3f operator - (const vec3f &vec){
	vec3f ret = {-vec.x, -vec.y, -vec.z};
	return ret;
}

inline __host__ __device__ vec4f operator - (const vec4f &vec){
	vec4f ret = {-vec.x, -vec.y, -vec.z, -vec.w};
	return ret;
}

inline __host__ __device__ float operator * (const vec2f &vec1, const vec2f &vec2){
	return (vec1.x*vec2.x+vec1.y*vec2.y);
}

inline __host__ __device__ float operator * (const vec3f &vec1, const vec3f &vec2){
	return (vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z);
}

inline __host__ __device__ float operator * (const vec4f &vec1, const vec4f &vec2){
	return (vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z+vec1.w*vec2.w);
}


inline __host__ __device__ vec2f operator / (const vec2f &vec1, const vec2f &vec2){
	return makeVec2f(vec1.x/vec2.x, vec1.y/vec2.y);
}

inline __host__ __device__ vec3f operator / (const vec3f &vec1, const vec3f &vec2){
	return makeVec3f(vec1.x/vec2.x,vec1.y/vec2.y,vec1.z/vec2.z);
}

inline __host__ __device__ vec4f operator / (const vec4f &vec1, const vec4f &vec2){
	return makeVec4f(vec1.x/vec2.x,vec1.y/vec2.y,vec1.z/vec2.z,vec1.w/vec2.w);
}

inline __host__ __device__ vec2f operator / (const float &val, const vec2f &vec){
	return makeVec2f(val/vec.x,val/vec.y);
}

inline __host__ __device__ vec3f operator / (const float &val, const vec3f &vec){
	return makeVec3f(val/vec.x,val/vec.y,val/vec.z);
}

inline __host__ __device__ vec4f operator / (const float &val, const vec4f &vec){
	return makeVec4f(val/vec.x,val/vec.y,val/vec.z,val/vec.w);
}

inline __host__ __device__ vec2f operator / (const vec2f &vec, const float &val){
	return makeVec2f(vec.x/val,vec.y/val);
}

inline __host__ __device__ vec3f operator / (const vec3f &vec, const float &val){
	return makeVec3f(vec.x/val,vec.y/val,vec.z/val);
}

inline __host__ __device__ vec4f operator / (const vec4f &vec, const float &val){
	return makeVec4f(vec.x/val,vec.y/val,vec.z/val,vec.w/val);
}

inline __host__ __device__ bool operator < (const vec2f &vec1, const vec2f &vec2){
	if (vec1.x==vec2.x)
		return (vec1.y<vec2.y);
	return (vec1.x<vec2.x);
}

inline __host__ __device__ bool operator > (const vec2f &vec1, const vec2f &vec2){
	if (vec1.x==vec2.x)
		return (vec1.y>vec2.y);
	return (vec1.x>vec2.x);
}

inline __host__ __device__ bool operator < (const vec3f &vec1, const vec3f &vec2){
	if (vec1.x==vec2.x){
		if (vec1.y==vec2.y){
			return (vec1.z<vec2.z);
		}
		return (vec1.y<vec2.y);
	}
	return (vec1.x<vec2.x);
}

inline __host__ __device__ bool operator > (const vec3f &vec1, const vec3f &vec2){
	if (vec1.x==vec2.x){
		if (vec1.y==vec2.y){
			return (vec1.z>vec2.z);
		}
		return (vec1.y>vec2.y);
	}
	return (vec1.x>vec2.x);
}

inline __host__ __device__ void operator += (vec2f &vec1, const vec2f &vec2){
	vec1.x += vec2.x;
	vec1.y += vec2.y;
}

inline __host__ __device__ void operator += (vec3f &vec1, const vec3f &vec2){
	vec1.x += vec2.x;
	vec1.y += vec2.y;
	vec1.z += vec2.z;
}

inline __host__ __device__ void operator += (vec4f &vec1, const vec4f &vec2){
	vec1.x += vec2.x;
	vec1.y += vec2.y;
	vec1.z += vec2.z;
	vec1.w += vec2.w;
}

inline __host__ __device__ void operator -= (vec2f &vec1, const vec2f &vec2){
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
}

inline __host__ __device__ void operator -= (vec3f &vec1, const vec3f &vec2){
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
	vec1.z -= vec2.z;
}

inline __host__ __device__ void operator -= (vec4f &vec1, const vec4f &vec2){
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
	vec1.z -= vec2.z;
	vec1.w -= vec2.w;
}

inline __host__ __device__ void operator *= (vec2f &vec1, const float& val){
	vec1.x *= val;
	vec1.y *= val;
}

inline __host__ __device__ void operator *= (vec3f &vec1, const float& val){
	vec1.x *= val;
	vec1.y *= val;
	vec1.z *= val;
}

inline __host__ __device__ void operator *= (vec4f &vec1, const float& val){
	vec1.x *= val;
	vec1.y *= val;
	vec1.z *= val;
	vec1.w *= val;
}

inline __host__ __device__ void normalize(vec2f &vec){
	float length = sqrt(vec*vec);
	vec.x /= length;
	vec.y /= length;
}

inline __host__ __device__ void normalize(vec3f &vec){
	float length = sqrt(vec*vec);
	vec.x /= length;
	vec.y /= length;
	vec.z /= length;
}

inline __host__ __device__ void normalize(vec4f &vec){
	float length = sqrt(vec*vec);
	vec.x /= length;
	vec.y /= length;
	vec.z /= length;
	vec.w /= length;
}

inline __host__ __device__ float length(const vec2f &vec){
	return sqrt(vec*vec);
}

inline __host__ __device__ float length(const vec3f &vec){
	return sqrt(vec*vec);
}

inline __host__ __device__ float length(const vec4f &vec){
	return sqrt(vec*vec);
}

inline __host__ __device__ float dist(const vec2f &vec1, const vec2f &vec2){
	return length(vec1-vec2);
}

inline __host__ __device__ float dist(const vec3f &vec1, const vec3f &vec2){
	return length(vec1-vec2);
}


inline __host__ __device__ float dist(const vec4f &vec1, const vec4f &vec2){
	return length(vec1-vec2);
}

inline __host__ __device__ bool isZero(vec3f & vec){
	return ((vec.x<0.000001f && vec.x>-0.000001f) 
		&& (vec.y<0.000001f && vec.y>-0.000001f) && (vec.z<0.000001f && vec.z>-0.000001f));
}

inline __host__ __device__ vec3f cross(const vec3f &vec1, const vec3f &vec2){
	vec3f ret = {vec1.y*vec2.z-vec1.z*vec2.y, vec1.z*vec2.x-vec1.x*vec2.z, vec1.x*vec2.y-vec1.y*vec2.x};
	return ret;
}

inline __host__ __device__ float det(const vec3f &vec1, const vec3f &vec2, const vec3f &vec3){
	return (vec1.x*vec2.y*vec3.z+vec1.y*vec2.z*vec3.x+vec1.z*vec2.x*vec3.y
		-vec1.x*vec2.z*vec3.y-vec1.y*vec2.x*vec3.z-vec1.z*vec2.y*vec3.x);
}

inline __host__ __device__ bool operator == (const vec2f &vec1, const vec2f &vec2){
	return ((vec1.x==vec2.x)&&(vec1.y==vec2.y));
}

inline __host__ __device__ bool operator == (const vec3f &vec1, const vec3f &vec2){
	return ((vec1.x==vec2.x)&&(vec1.y==vec2.y)&&(vec1.z==vec2.z));
}

inline __host__ __device__ bool operator == (const vec4f &vec1, const vec4f &vec2){
	return ((vec1.x==vec2.x)&&(vec1.y==vec2.y)&&(vec1.z==vec2.z)&&(vec1.w==vec2.w));
}

inline __host__ __device__ bool operator != (const vec2f &vec1, const vec2f &vec2){
	return ((vec1.x!=vec2.x)||(vec1.y!=vec2.y));
}

inline __host__ __device__ bool operator != (const vec3f &vec1, const vec3f &vec2){
	return ((vec1.x!=vec2.x)||(vec1.y!=vec2.y)||(vec1.z!=vec2.z));
}

inline __host__ __device__ bool operator != (const vec4f &vec1, const vec4f &vec2){
	return ((vec1.x!=vec2.x)||(vec1.y!=vec2.y)||(vec1.z!=vec2.z)||(vec1.w!=vec2.w));
}

inline __host__ __device__ float clamp(float v, const float &low, const float &up){
	v = (v>low)?v:low;
	v = (v<up)?v:up;
	return v;
}

inline __host__ __device__ vec2f clamp(vec2f v, const vec2f &low, const vec2f &up){
	v.x = clamp(v.x, low.x, up.x);
	v.y = clamp(v.y, low.y, up.y);
	return v;
}

inline __host__ __device__ vec3f clamp(vec3f v, const vec3f &low, const vec3f &up){
	v.x = clamp(v.x, low.x, up.x);
	v.y = clamp(v.y, low.y, up.y);
	v.z = clamp(v.z, low.z, up.z);
	return v;
}

inline __host__ __device__ vec4f clamp(vec4f v, const vec4f &low, const vec4f &up){
	v.x = clamp(v.x, low.x, up.x);
	v.y = clamp(v.y, low.y, up.y);
	v.z = clamp(v.z, low.z, up.z);
	v.w = clamp(v.w, low.w, up.w);
	return v;
}

inline __host__ __device__ bool inBound(const float& v, const float& low, const float& up){
	return (v>=low && v<=up);
}

inline __host__ __device__ bool inBound(const vec2f& v, const vec2f& low, const vec2f& up){
	return ((v.x>=low.x)&&(v.y>=low.y)&&(v.x<=up.x)&&(v.y<=up.y));
}

inline __host__ __device__ bool inBound(const vec3f& v, const vec3f& low, const vec3f& up){
	return ((v.x>=low.x)&&(v.y>=low.y)&&(v.z>=low.z)&&(v.x<=up.x)&&(v.y<=up.y)&&(v.z<=up.z));
}

inline __host__ __device__ bool inBound(const vec4f& v, const vec4f& low, const vec4f& up){
	return ((v.x>=low.x)&&(v.y>=low.y)&&(v.z>=low.z)&&(v.w>=low.w)&&(v.x<=up.x)&&(v.y<=up.y)&&(v.z<=up.z)&&(v.w<=up.w));
}


inline __host__ __device__ Area makeArea(const float& x, const float& y, const float& w, const float& h){
	Area ret = {x, y, w, h};
	return ret;
}

inline __host__ __device__ bool isOverlapped(const Area& a, const Area& b){
	if ((a.x>b.x+b.w) || (b.x>a.x+a.w) || (a.y>b.y+b.h) || (b.y>a.y+a.h))
		return false;
	return true;
}

inline __host__ __device__ bool isOverlapped(const Area& a, const Area& b, const float& margin){
	if ((a.x-margin>b.x+b.w) || (b.x-margin>a.x+a.w) || (a.y-margin>b.y+b.h) || (b.y-margin>a.y+a.h))
		return false;
	return true;
}

inline __host__ __device__ bool isContained(const Area& a, const Area& b){
	if ((a.x>b.x) && (a.y>b.y) && (a.x+a.w<b.x+b.w) && (a.y+a.h<b.y+b.h))
		return true;
	return false;
}

inline __host__ __device__ bool isContained(const Area& a, const vec2f& p){
	if (a.x<=p.x && a.x+a.w>=p.x && a.y<=p.y && a.y+a.h>=p.y)
		return true;
	return false;
}


inline __host__ __device__ bool isContained(const vec2f& lb, const vec2f& rt, const vec2f& p){
	if (lb.x<=p.x && lb.y<=p.y && rt.x>=p.x && rt.y>=p.y)
		return true;
	return false;
}

//-----------------------------------------------------------------
//------------------     int  type    -----------------------------
//-----------------------------------------------------------------
inline __host__ __device__ vec3i makeVec3i(const int &x, const int &y, const int &z){
	vec3i ret = {x, y, z};
	return ret;
}

inline __host__ __device__ vec3i operator * (const int &scale, const vec3i &vec){
	vec3i ret = {scale*vec.x, scale*vec.y, scale*vec.z};
	return ret;
}

inline __host__ __device__ vec3i operator * (const vec3i &vec, const int &scale){
	vec3i ret = {scale*vec.x, scale*vec.y, scale*vec.z};
	return ret;
}

inline __host__ __device__ vec3i operator + (const vec3i &vec1, const vec3i &vec2){
	vec3i ret = {vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z};
	return ret;
}

inline __host__ __device__ vec3i operator - (const vec3i &vec1, const vec3i &vec2){
	vec3i ret = {vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z};
	return ret;
}

inline __host__ __device__ vec3i operator - (const vec3i &vec){
	vec3i ret = {-vec.x, -vec.y, -vec.z};
	return ret;
}

inline __host__ __device__ int operator * (const vec3i &vec1, const vec3i &vec2){
	return (vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z);
}

inline __host__ __device__ bool operator == (const vec3i &vec1, const vec3i &vec2){
	return ((vec1.x==vec2.x)&&(vec1.y==vec2.y)&&(vec1.z==vec2.z));
}

inline __host__ __device__ bool operator != (const vec3i &vec1, const vec3i &vec2){
	return ((vec1.x!=vec2.x)||(vec1.y!=vec2.y)||(vec1.z!=vec2.z));
}

inline __host__ __device__ bool operator < (const vec3i &vec1, const vec3i &vec2){
	if (vec1.x==vec2.x){
		if (vec1.y==vec2.y){
			return (vec1.z<vec2.z);
		}
		return (vec1.y<vec2.y);
	}
	return (vec1.x<vec2.x);
}

inline __host__ __device__ bool operator > (const vec3i &vec1, const vec3i &vec2){
	if (vec1.x==vec2.x){
		if (vec1.y==vec2.y){
			return (vec1.z>vec2.z);
		}
		return (vec1.y>vec2.y);
	}
	return (vec1.x>vec2.x);
}

//2i
inline __host__ __device__ vec2i makeVec2i(const int &x, const int &y){
	vec2i ret = {x, y};
	return ret;
}

inline __host__ __device__ vec2i operator * (const int &scale, const vec2i &vec){
	vec2i ret = {scale*vec.x, scale*vec.y};
	return ret;
}

inline __host__ __device__ vec2i operator * (const vec2i &vec, const int &scale){
	vec2i ret = {scale*vec.x, scale*vec.y};
	return ret;
}

inline __host__ __device__ vec2i operator + (const vec2i &vec1, const vec2i &vec2){
	vec2i ret = {vec1.x+vec2.x, vec1.y+vec2.y};
	return ret;
}

inline __host__ __device__ vec2i operator - (const vec2i &vec1, const vec2i &vec2){
	vec2i ret = {vec1.x-vec2.x, vec1.y-vec2.y};
	return ret;
}

inline __host__ __device__ vec2i operator - (const vec2i &vec){
	vec2i ret = {-vec.x, -vec.y};
	return ret;
}

inline __host__ __device__ int operator * (const vec2i &vec1, const vec2i &vec2){
	return (vec1.x*vec2.x+vec1.y*vec2.y);
}

inline __host__ __device__ bool operator == (const vec2i &vec1, const vec2i &vec2){
	return ((vec1.x==vec2.x)&&(vec1.y==vec2.y));
}

inline __host__ __device__ bool operator != (const vec2i &vec1, const vec2i &vec2){
	return ((vec1.x!=vec2.x)||(vec1.y!=vec2.y));
}

inline __host__ __device__ bool operator < (const vec2i &vec1, const vec2i &vec2){
	if (vec1.x==vec2.x)
		return (vec1.y<vec2.y);
	return (vec1.x<vec2.x);
}

inline __host__ __device__ bool operator > (const vec2i &vec1, const vec2i &vec2){
	if (vec1.x==vec2.x)
		return (vec1.y>vec2.y);
	return (vec1.x>vec2.x);
}

//4i
inline __host__ __device__ vec4i makeVec4i(const int &x, const int &y, const int &z, const int &w){
	vec4i ret = {x, y, z, w};
	return ret;
}

inline __host__ __device__ vec4i operator * (const int &scale, const vec4i &vec){
	vec4i ret = {scale*vec.x, scale*vec.y, scale*vec.z, scale*vec.w};
	return ret;
}

inline __host__ __device__ vec4i operator * (const vec4i &vec, const int &scale){
	vec4i ret = {scale*vec.x, scale*vec.y, scale*vec.z, scale*vec.w};
	return ret;
}

inline __host__ __device__ vec4i operator + (const vec4i &vec1, const vec4i &vec2){
	vec4i ret = {vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z, vec1.w+vec2.w};
	return ret;
}

inline __host__ __device__ vec4i operator - (const vec4i &vec1, const vec4i &vec2){
	vec4i ret = {vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z, vec1.w-vec2.w};
	return ret;
}

inline __host__ __device__ vec4i operator - (const vec4i &vec){
	vec4i ret = {-vec.x, -vec.y, -vec.z, -vec.w};
	return ret;
}

inline __host__ __device__ int operator * (const vec4i &vec1, const vec4i &vec2){
	return (vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z+vec1.w*vec2.w);
}

inline __host__ __device__ bool operator == (const vec4i &vec1, const vec4i &vec2){
	return ((vec1.x==vec2.x)&&(vec1.y==vec2.y)&&(vec1.z==vec2.z)&&(vec1.w==vec2.w));
}

inline __host__ __device__ bool operator != (const vec4i &vec1, const vec4i &vec2){
	return ((vec1.x!=vec2.x)||(vec1.y!=vec2.y)||(vec1.z!=vec2.z)||(vec1.w!=vec2.w));
}

inline __host__ __device__ bool operator < (const vec4i &vec1, const vec4i &vec2){
	if (vec1.x==vec2.x){
		if (vec1.y==vec2.y){
			if (vec1.z==vec2.z) {
				return (vec1.w<vec2.w);
			}
			return (vec1.z<vec2.z);
		}
		return (vec1.y<vec2.y);
	}
	return (vec1.x<vec2.x);
}

inline __host__ __device__ bool operator > (const vec4i &vec1, const vec4i &vec2){
	if (vec1.x==vec2.x){
		if (vec1.y==vec2.y){
			if (vec1.z==vec2.z) {
				return (vec1.w>vec2.w);
			}
			return (vec1.z>vec2.z);
		}
		return (vec1.y>vec2.y);
	}
	return (vec1.x>vec2.x);
}

inline __host__ __device__ void operator += (vec2i &vec1, const vec2i &vec2){
	vec1.x += vec2.x;
	vec1.y += vec2.y;
}

inline __host__ __device__ void operator += (vec3i &vec1, const vec3i &vec2){
	vec1.x += vec2.x;
	vec1.y += vec2.y;
	vec1.z += vec2.z;
}

inline __host__ __device__ void operator += (vec4i &vec1, const vec4i &vec2){
	vec1.x += vec2.x;
	vec1.y += vec2.y;
	vec1.z += vec2.z;
	vec1.w += vec2.w;
}

inline __host__ __device__ void operator -= (vec2i &vec1, const vec2i &vec2){
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
}

inline __host__ __device__ void operator -= (vec3i &vec1, const vec3i &vec2){
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
	vec1.z -= vec2.z;
}

inline __host__ __device__ void operator -= (vec4i &vec1, const vec4i &vec2){
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
	vec1.z -= vec2.z;
	vec1.w -= vec2.w;
}

inline __host__ __device__ int clamp(int v, const int &low, const int &up){
	v = (v>low)?v:low;
	v = (v<up)?v:up;
	return v;
}

inline __host__ __device__ vec2i clamp(vec2i v, const vec2i &low, const vec2i &up){
	v.x = clamp(v.x, low.x, up.x);
	v.y = clamp(v.y, low.y, up.y);
	return v;
}

inline __host__ __device__ vec3i clamp(vec3i v, const vec3i &low, const vec3i &up){
	v.x = clamp(v.x, low.x, up.x);
	v.y = clamp(v.y, low.y, up.y);
	v.z = clamp(v.z, low.z, up.z);
	return v;
}

inline __host__ __device__ vec4i clamp(vec4i v, const vec4i &low, const vec4i &up){
	v.x = clamp(v.x, low.x, up.x);
	v.y = clamp(v.y, low.y, up.y);
	v.z = clamp(v.z, low.z, up.z);
	v.w = clamp(v.w, low.w, up.w);
	return v;
}

inline __host__ __device__ bool inBound(const int& v, const int& low, const int& up){
	return (v>=low && v<=up);
}

inline __host__ __device__ bool inBound(const vec2i& v, const vec2i& low, const vec2i& up){
	return ((v.x>=low.x)&&(v.y>=low.y)&&(v.x<=up.x)&&(v.y<=up.y));
}

inline __host__ __device__ bool inBound(const vec3i& v, const vec3i& low, const vec3i& up){
	return ((v.x>=low.x)&&(v.y>=low.y)&&(v.z>=low.z)&&(v.x<=up.x)&&(v.y<=up.y)&&(v.z<=up.z));
}

inline __host__ __device__ bool inBound(const vec4i& v, const vec4i& low, const vec4i& up){
	return ((v.x>=low.x)&&(v.y>=low.y)&&(v.z>=low.z)&&(v.w>=low.w)&&(v.x<=up.x)&&(v.y<=up.y)&&(v.z<=up.z)&&(v.w<=up.w));
}

//other vector functions
#define vec2dDot(v1, v2) ((v1).x*(v2).x+(v1).y*(v2).y)
#define vec3fDot(v1, v2) ((v1).x*(v2).x+(v1).y*(v2).y+(v1).z*(v2).z)
#define vec2dLen(v)	sqrtf((v).x*(v).x+(v).y*(v).y)
#define vec3fLen(v) sqrtf((v).x*(v).x+(v).y*(v).y+(v).z*(v).z)
#define dist2d(v1, v2) sqrtf((v1-v2)*(v1-v2))
#define dist3d(v1, v2) sqrtf((v1-v2)*(v1-v2))

//other functions
#define maxOf(a, b) ((a)>(b))?(a):(b)
#define minOf(a, b) ((a)<(b))?(a):(b)

inline __host__ __device__ int iDivUp(int a, int b) {
	return (a % b != 0) ? (a / b + 1) : (a / b);
}

template <class T>
inline __host__ __device__ void computeMinMax(T* data, int num, T& minv, T& maxv){
	minv = data[0];
	maxv = data[0];
	for (int i=1; i<num; ++i){
		if (minv>data[i]) minv = data[i];
		if (maxv<data[i]) maxv = data[i];
	}
}


static __host__ __device__ bool getInvertMatrix(const float m[16], float invOut[16])
{
	float inv[16], det;
	int i;

	inv[0] = m[5]  * m[10] * m[15] - 
		m[5]  * m[11] * m[14] - 
		m[9]  * m[6]  * m[15] + 
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] - 
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] + 
		m[4]  * m[11] * m[14] + 
		m[8]  * m[6]  * m[15] - 
		m[8]  * m[7]  * m[14] - 
		m[12] * m[6]  * m[11] + 
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] - 
		m[4]  * m[11] * m[13] - 
		m[8]  * m[5] * m[15] + 
		m[8]  * m[7] * m[13] + 
		m[12] * m[5] * m[11] - 
		m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] + 
		m[4]  * m[10] * m[13] +
		m[8]  * m[5] * m[14] - 
		m[8]  * m[6] * m[13] - 
		m[12] * m[5] * m[10] + 
		m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] + 
		m[1]  * m[11] * m[14] + 
		m[9]  * m[2] * m[15] - 
		m[9]  * m[3] * m[14] - 
		m[13] * m[2] * m[11] + 
		m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] - 
		m[0]  * m[11] * m[14] - 
		m[8]  * m[2] * m[15] + 
		m[8]  * m[3] * m[14] + 
		m[12] * m[2] * m[11] - 
		m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] + 
		m[0]  * m[11] * m[13] + 
		m[8]  * m[1] * m[15] - 
		m[8]  * m[3] * m[13] - 
		m[12] * m[1] * m[11] + 
		m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] - 
		m[0]  * m[10] * m[13] - 
		m[8]  * m[1] * m[14] + 
		m[8]  * m[2] * m[13] + 
		m[12] * m[1] * m[10] - 
		m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] - 
		m[1]  * m[7] * m[14] - 
		m[5]  * m[2] * m[15] + 
		m[5]  * m[3] * m[14] + 
		m[13] * m[2] * m[7] - 
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] + 
		m[0]  * m[7] * m[14] + 
		m[4]  * m[2] * m[15] - 
		m[4]  * m[3] * m[14] - 
		m[12] * m[2] * m[7] + 
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] - 
		m[0]  * m[7] * m[13] - 
		m[4]  * m[1] * m[15] + 
		m[4]  * m[3] * m[13] + 
		m[12] * m[1] * m[7] - 
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] + 
		m[0]  * m[6] * m[13] + 
		m[4]  * m[1] * m[14] - 
		m[4]  * m[2] * m[13] - 
		m[12] * m[1] * m[6] + 
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] + 
		m[1] * m[7] * m[10] + 
		m[5] * m[2] * m[11] - 
		m[5] * m[3] * m[10] - 
		m[9] * m[2] * m[7] + 
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] - 
		m[0] * m[7] * m[10] - 
		m[4] * m[2] * m[11] + 
		m[4] * m[3] * m[10] + 
		m[8] * m[2] * m[7] - 
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] + 
		m[0] * m[7] * m[9] + 
		m[4] * m[1] * m[11] - 
		m[4] * m[3] * m[9] - 
		m[8] * m[1] * m[7] + 
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] - 
		m[0] * m[6] * m[9] - 
		m[4] * m[1] * m[10] + 
		m[4] * m[2] * m[9] + 
		m[8] * m[1] * m[6] - 
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0f / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}

template <class T>
inline __host__ __device__ void mat_mult(T** m, T* v, T* ret, const int& n){
	for (int i=0; i<n; ++i) {
		ret[i] = 0;
		for (int j=0; j<n; ++j) {
			ret[i] += m[i][j]*v[j];
		}
	}
}

template <class T>
inline __host__ __device__ T inner(T* v1, T* v2, const int& n){
	T ret= 0.0f;
	for (int i=0; i<n; ++i) {
		ret += v1[i]*v2[i];
	}
	return ret;
}

template<typename T>
inline __host__ __device__ T interpolate(const T& lower, const T& upper, float fac){
	fac = clamp(fac, 0.0f, 1.0f);
	return ((1.0f-fac)*lower+fac*upper);
}

inline __host__ __device__ float interpolate(const float& a1, const float& b1, const float& c1, const float& a2, const float b2){
	if (a1==b1) {
		return a1;
	}
	return (a2+(b2-a2)*(c1-a1)/(b1-a1));
}

inline bool onSegment(const vec2f& p, const vec2f& q, const vec2f& r){
	if (q.x <= maxOf(p.x, r.x) && q.x >= minOf(p.x, r.x) &&
		q.y <= maxOf(p.y, r.y) && q.y >= minOf(p.y, r.y))
		return true;

	return false;
}

inline int orientation(const vec2f& p, const vec2f& q, const vec2f& r){
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear

	return (val > 0)? 1: 2; // clock or counterclock wise
}

static bool segmentIntersected(const vec2f& p1, const vec2f& p2, const vec2f& q1, const vec2f& q2){

	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if (o1 != o2 && o3 != o4)
		return true;

	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false;
}

static __host__ __device__ float determinantOfColumnMatrix(const vec3f mat[3]){
	return (mat[0].x*mat[1].y*mat[2].z+mat[1].x*mat[2].y*mat[0].z+mat[2].x*mat[0].y*mat[1].z
		-mat[2].x*mat[1].y*mat[0].z-mat[1].x*mat[0].y*mat[2].z-mat[0].x*mat[2].y*mat[1].z);
}

static __host__ __device__ float determinantOfColumnMatrix(const vec3f& v1, const vec3f& v2, const vec3f& v3){
	return (v1.x*v2.y*v3.z+v2.x*v3.y*v1.z+v3.x*v1.y*v2.z
		-v3.x*v2.y*v1.z-v2.x*v1.y*v3.z-v1.x*v3.y*v2.z);
}


static __host__ __device__ void transformVector(const float m[16], const float v[4], float ret[4]){
	ret[0] = m[0]*v[0]+m[4]*v[1]+m[ 8]*v[2]+m[12]*v[3];
	ret[1] = m[1]*v[0]+m[5]*v[1]+m[ 9]*v[2]+m[12]*v[3];
	ret[2] = m[2]*v[0]+m[6]*v[1]+m[10]*v[2]+m[12]*v[3];
	ret[3] = m[3]*v[0]+m[7]*v[1]+m[11]*v[2]+m[12]*v[3];
}

template<typename T>
inline __host__ int getItemIndex(std::map<T,int>& id_map, const T& item){
	typename std::map<T,int>::iterator it = id_map.find(item);
	if (it!=id_map.end()) {
		return it->second;
	}
	int ret = id_map.size();
	id_map.insert(std::pair<T,int>(item, ret));
	return ret;
}

template <class T>
inline void allocateMatrix(T*& data, T**& mat, const int& w, const int& h){
	int wh = w*h;

	data = new T[wh];
	mat = new T*[h];

	for (int i=0; i<h; ++i) {
		mat[i] = &data[i*w];
	}
}

template <class T>
inline void allocateMatrixAccess(T*& data, T**& mat, const int& w, const int& h){
	mat = new T*[h];

	for (int i=0; i<h; ++i) {
		mat[i] = &data[i*w];
	}
}


template <class T>
inline void allocateVolume(T*& data, T**& rows, T***& vol, const int& w, const int& h, const int& d){
	int wh = w*h;

	data = new T[wh*d];
	rows = new T*[h*d];
	vol = new T**[d];

	for (int i=0; i<d; ++i) {
		vol[i] = &rows[i*h];
		for (int j=0; j<h; ++j) {
			rows[i*h+j] = &data[i*wh+j*w];
		}
	}
}

template <class T>
inline void allocateVolumeAccess(T* data, T**& rows, T***& vol, const int& w, const int& h, const int& d){
	int wh = w*h;

	rows = new T*[h*d];
	vol = new T**[d];
	for (int i=0; i<d; ++i) {
		vol[i] = &rows[i*h];
		for (int j=0; j<h; ++j) {
			rows[i*h+j] = &data[i*wh+j*w];
		}
	}
}

inline cudaArray* allocateCudaVectorField(vec4f* vec_field, const vec3i& dim){
	cudaArray* ret;
	// create 3D array
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float4>();
	checkCudaErrors(cudaMalloc3DArray(&ret, &channelDesc, make_cudaExtent(dim.x, dim.y, dim.z)));

	// copy data to 3D array
	cudaMemcpy3DParms copyParams = {0};
	copyParams.srcPtr   = make_cudaPitchedPtr(vec_field, dim.x*sizeof(float4), dim.x, dim.y);
	copyParams.dstArray = ret;
	copyParams.extent   = make_cudaExtent(dim.x, dim.y, dim.z);
	copyParams.kind     = cudaMemcpyHostToDevice;
	checkCudaErrors(cudaMemcpy3D(&copyParams));

	return ret;
}

inline cudaArray* allocateCudaVectorField(vec3f* vec_field, const vec3i& dim){
	//copy as float4
	float4* vf = new float4[dim.x*dim.y*dim.z];
	for (int i=0; i<dim.x*dim.y*dim.z; ++i) {
		vf[i] = make_float4(vec_field[i].x, vec_field[i].y, vec_field[i].z, 1.0f);
	}

	cudaArray* ret;
	// create 3D array
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float4>();
	checkCudaErrors(cudaMalloc3DArray(&ret, &channelDesc, make_cudaExtent(dim.x, dim.y, dim.z)));

	// copy data to 3D array
	cudaMemcpy3DParms copyParams = {0};
	copyParams.srcPtr   = make_cudaPitchedPtr(vf, dim.x*sizeof(float4), dim.x, dim.y);
	copyParams.dstArray = ret;
	copyParams.extent   = make_cudaExtent(dim.x, dim.y, dim.z);
	copyParams.kind     = cudaMemcpyHostToDevice;
	checkCudaErrors(cudaMemcpy3D(&copyParams));

	delete[] vf;
	return ret;
}

inline void updateCudaVectorField(cudaArray* vec_field_d, vec4f* vec_field_h, const vec3i& dim){
	// copy data to 3D array
	cudaMemcpy3DParms copyParams = {0};
	copyParams.srcPtr   = make_cudaPitchedPtr(vec_field_h, dim.x*sizeof(float4), dim.x, dim.y);
	copyParams.dstArray = vec_field_d;
	copyParams.extent   = make_cudaExtent(dim.x, dim.y, dim.z);
	copyParams.kind     = cudaMemcpyHostToDevice;
	checkCudaErrors(cudaMemcpy3D(&copyParams));
}

inline bool open_file(std::ifstream& fin, const char* fname, const bool& binary=false){
	if (binary) {
		fin.open(fname, std::ios_base::in|std::ios::binary);
	} else {
		fin.open(fname);
	}
	if(!fin.is_open()){
		printf("Fail to read file: %s.\n", fname);
		return false;
	}
	return true;
}

inline bool open_file(std::ofstream& fout, const char* fname, const bool& binary=false){
	if (binary) {
		fout.open(fname, std::ios_base::out|std::ios::binary);
	} else {
		fout.open(fname);
	}
	if(!fout.is_open()){
		printf("Fail to read file: %s.\n", fname);
		return false;
	}
	return true;
}

inline bool read_header(vec3i& ret_dim, const char* file_path){
	std::ifstream file;
	if (!open_file(file, file_path, false)) {
		return false;
	}

	file>>ret_dim.x>>ret_dim.y>>ret_dim.z;
	file.close();

	return true;
}

static void normalizeArray(float* val, const int& num, const float& cutoff_percentage=0.9f){
	float upper, lower;
	if (cutoff_percentage>=0.9999999f) {
		upper = lower = val[0];
		for (int i=1; i<num; ++i) {
			if (val[i]>upper) upper = val[i];
			if (val[i]<lower) lower = val[i];
		}
	} else {
		std::vector<float> sorted_array;
		sorted_array.assign(val, val+num);
		std::sort(sorted_array.begin(), sorted_array.end());
		int upper_idx = num*cutoff_percentage, lower_idx = num*(1-cutoff_percentage);
		upper = sorted_array[upper_idx];
		lower = sorted_array[lower_idx];
	}


	float fac = 1.0f/(upper-lower);

	for (int i=0; i<num; ++i) {
		val[i] = (val[i]-lower)*fac;
		val[i] = clamp(val[i], 0.0f, 1.0f);
	}
}

#endif