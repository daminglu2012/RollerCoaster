#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "spline.h"

std::vector<int> pointsPerSplineSeg;

int nSegments;

void SplineData::loadSegmentFrom(char* filename)
{	
	FILE* fileSplineSegment = fopen(filename, "r");

	if (fileSplineSegment == NULL) 
	{
		printf ("can't open file %s\n", filename);
		exit(1);
	}

	int iLength;

	fscanf(fileSplineSegment, "%d", &iLength);

	pointsPerSplineSeg.push_back(iLength);

	Vec3f pt;

	while (fscanf(fileSplineSegment, "%f %f %f", 
				&pt.x(), &pt.y(),&pt.z()) != EOF) 
	{
		m_vPoints.push_back(pt);
	}
	fclose(fileSplineSegment);
}

void SplineData::loadSplineFrom(char* filename)
{	
	FILE* fileSpline = fopen(filename, "r");

	if (fileSpline == NULL) 
	{
		printf ("can't open file %s\n", filename);
		exit(1);
	}

	fscanf(fileSpline, "%d", &nSegments);

	for (int j = 0; j < nSegments; j++) 
	{
		char segmentfilename[1024];

		fscanf(fileSpline, "%s", segmentfilename);
		loadSegmentFrom(segmentfilename);
	}
	fclose(fileSpline);
}