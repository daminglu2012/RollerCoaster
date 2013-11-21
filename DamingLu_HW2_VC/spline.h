#ifndef _RC_SPLINE_H
#define _RC_SPLINE_H

#include "my_3d_vectors.h"
#include <vector>

class SplineData;

typedef std::vector<Vec3f> pointVector;
typedef pointVector::iterator pointVectorIter;

class SplineData
{
public:
	pointVector m_vPoints;

	pointVector& points() { return m_vPoints; }

	void loadSegmentFrom(char* filename);

	void loadSplineFrom(char* filename);
};


#endif
