#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>     
#include <stdlib.h>    
#ifdef __APPLE__

#include <OpenGL/gl.h>    
#include <OpenGL/glu.h>    
#include <GLUT/glut.h>   
#else
#include <GL/gl.h>     
#include <GL/glu.h>    
#include <GL/glut.h>   
#endif
#include <pic.h>
#include "spline.h"
#include <cmath>

extern GLuint deckTextureId;
extern GLuint frontTextureId;
extern GLuint rearTextureId;
extern GLuint leftTextureId;
extern GLuint rightTextureId;
extern GLuint ceilingTextureId;

extern GLuint crossbarTextureId;

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int xMax;
extern int yMax;
extern int zMax;

extern std::vector<Vec3f> all_spline_points;
extern std::vector<Vec3f> all_tangent_spline_points;
extern std::vector<Vec3f> all_normal_points;
extern std::vector<Vec3f> all_binormal_points;
extern std::vector<int> pointsPerSplineSeg;


void loadTexture (char *filename, GLuint &textureID);

extern float angle, angleY;
// actual vector representing the camera's direction
extern float lx,lz, ly;
// XZ position of the camera
extern float x,z, y;
//
double width=0.1, height=0.005, thickness=0.025; // crossbar

const int pixelWidth = 5; // 5 or 10, 50 is too slow

void drawCrossBar(Vec3f pt){
	glPushMatrix();

	glTranslatef(pt.x(),pt.y(),pt.z());

	glColor3f(0.0f, 1.0f , 0.0f);
	glutSolidCone(0.08f,0.5f,5,2);
	glPopMatrix();
}
//-----------------------------------------------------------------------------
void drawSplineSegment( Vec3f pt1, Vec3f pt2, Vec3f pt3, Vec3f pt4 )
{
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	glColor3f( 1, 0, 0 ); // R G B
	for(int i = 0; i != pixelWidth; i++) 
	{ 
		float u = (float)i / (float)(pixelWidth - 1);
		float p[4];
		p[0] =-1.0 * u * u * u + 2.0 * u * u - 1.0 * u + 0.0; 
		p[1] = 3.0 * u * u * u - 5.0 * u * u + 0.0 * u + 2.0; 
		p[2] =-3.0 * u * u * u + 4.0 * u * u + 1.0 * u + 0.0; 
		p[3] = 1.0 * u * u * u - 1.0 * u * u + 0.0 * u + 0.0;

		p[0] *= 0.5;
		p[1] *= 0.5;
		p[2] *= 0.5;
		p[3] *= 0.5;

		float x, y, z;
		x = y = z = 0.0; 
		x = p[0]*pt1.x() + p[1]*pt2.x() + p[2]*pt3.x() + p[3]*pt4.x();
		y = p[0]*pt1.y() + p[1]*pt2.y() + p[2]*pt3.y() + p[3]*pt4.y();
		z = p[0]*pt1.z() + p[1]*pt2.z() + p[2]*pt3.z() + p[3]*pt4.z();

		glVertex3f( x, y, z );
		Vec3f curPoint(x,y,z);
	} 

	glEnd();
}
//-----------------------------------------------------------------------------
void recordAllSplinePoints( Vec3f pt1, Vec3f pt2, Vec3f pt3, Vec3f pt4 )
{
	for(int i = 0; i != pixelWidth; i++) 
	{ 
		float u = (float)i / (float)(pixelWidth - 1);
		float p[4], tangent[4];
		p[0] =-1.0 * u * u * u + 2.0 * u * u - 1.0 * u + 0.0; 
		p[1] = 3.0 * u * u * u - 5.0 * u * u + 0.0 * u + 2.0; 
		p[2] =-3.0 * u * u * u + 4.0 * u * u + 1.0 * u + 0.0; 
		p[3] = 1.0 * u * u * u - 1.0 * u * u + 0.0 * u + 0.0;

		p[0] *= 0.5;
		p[1] *= 0.5;
		p[2] *= 0.5;
		p[3] *= 0.5;

		tangent[0] = -3.0 * u * u +  4 * u - 1.0 + 0.0;
		tangent[1] =  9.0 * u * u - 10 * u + 0.0 + 0.0;
		tangent[2] = -9.0 * u * u +  8 * u + 1.0 + 0.0;
		tangent[3] =  3.0 * u * u -  2 * u + 0.0 + 0.0;

		tangent[0] *= 0.5;
		tangent[1] *= 0.5;
		tangent[2] *= 0.5;
		tangent[3] *= 0.5;

		float x, y, z;
		x = y = z = 0.0; 
		x = p[0]*pt1.x() + p[1]*pt2.x() + p[2]*pt3.x() + p[3]*pt4.x();
		y = p[0]*pt1.y() + p[1]*pt2.y() + p[2]*pt3.y() + p[3]*pt4.y();
		z = p[0]*pt1.z() + p[1]*pt2.z() + p[2]*pt3.z() + p[3]*pt4.z();

		Vec3f curPoint(x,y,z);
		all_spline_points.push_back(curPoint); // !!!

		x = y = z = 0.0;
		x = tangent[0]*pt1.x()+tangent[1]*pt2.x()+tangent[2]*pt3.x()+tangent[3]*pt4.x();
		y = tangent[0]*pt1.y()+tangent[1]*pt2.y()+tangent[2]*pt3.y()+tangent[3]*pt4.y();
		z = tangent[0]*pt1.z()+tangent[1]*pt2.z()+tangent[2]*pt3.z()+tangent[3]*pt4.z();

		double norm = sqrt (x*x + y*y + z*z);
		if(norm==0) norm = 1E-9;
		x /= norm;
		y /= norm;
		z /= norm;

		Vec3f curTangentPoint(x,y,z);
		all_tangent_spline_points.push_back(curTangentPoint);
	} 

	glEnd();
}
//-----------------------------------------------------------------------------

Vec3f crossUnit(Vec3f v1, Vec3f v2){
	double vx = v1.y()*v2.z() - v1.z()*v2.y();
	double vy = v1.z()*v2.x() - v1.x()*v2.z();
	double vz = v1.x()*v2.y() - v1.y()*v2.x();
	return Vec3f(vx,vy,vz);
}

//-----------------------------------------------------------------------------
void calcNormalBinormal(){

	std::vector<int> cumuPointsPerSplineSeg;
	int segSum=0;
	cumuPointsPerSplineSeg.push_back(segSum);
	for(int i=0; i<pointsPerSplineSeg.size();i++){
		int cur = pointsPerSplineSeg[i]*pixelWidth;
		segSum+=cur;
		cumuPointsPerSplineSeg.push_back(segSum);
	}

	int iterAllSplinePoints = 0;
	int breakPointIter=0;
	while(iterAllSplinePoints < all_tangent_spline_points.size()){
		if(iterAllSplinePoints == cumuPointsPerSplineSeg[breakPointIter]){
			Vec3f arbV;
			if(breakPointIter>=11){
				Vec3f arbV1(1.0,0.0,0.0);
				arbV = arbV1;
			}else{
				Vec3f arbV2(-1.0,0.0,0.0);
				arbV = arbV2;
			}
			Vec3f N0 = crossUnit(all_tangent_spline_points[iterAllSplinePoints], arbV);
			Vec3f B0 = crossUnit(all_tangent_spline_points[iterAllSplinePoints], N0);
			
			all_normal_points.push_back(N0);
			all_binormal_points.push_back(B0);
			iterAllSplinePoints++;
			breakPointIter++;
			continue;
		}
		Vec3f N1 = crossUnit(
			all_binormal_points[iterAllSplinePoints-1], 
			all_tangent_spline_points[iterAllSplinePoints]);  

		Vec3f B1 = crossUnit(
			all_tangent_spline_points[iterAllSplinePoints],
			N1);
		
		all_normal_points.push_back(N1);
		all_binormal_points.push_back(B1);
		iterAllSplinePoints++;
	}
}

//-----------------------------------------------------------------------------
void drawFace(Vec3f v0,Vec3f v1,Vec3f v2,Vec3f v3, Vec3f center){

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);								
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glDepthMask(true);		

	glBindTexture(GL_TEXTURE_2D, crossbarTextureId);  

	glBegin(GL_QUADS);
		glTranslatef(center.x(),center.y(),center.z());

		glTexCoord2f(0.0 ,1.0);	
		glVertex3f(v0.x(),v0.y(),v0.z());
		glTexCoord2f(1.0 ,1.0);	
		glVertex3f(v1.x(),v1.y(),v1.z());
		glTexCoord2f(1.0 ,0.0);	
		glVertex3f(v2.x(),v2.y(),v2.z());
		glTexCoord2f(0.0 ,0.0);	
		glVertex3f(v3.x(),v3.y(),v3.z());
	glEnd();

	glDisable(GL_TEXTURE_2D); 
	glPopMatrix();

}

void drawSingleCrossbar(Vec3f center, Vec3f tangent, Vec3f normal, Vec3f binormal){
	center = center - normal*height;
	Vec3f posT(center + tangent*(thickness/2));
	Vec3f negT(center - tangent*(thickness/2));
	Vec3f posN(center + normal*(height/2));
	Vec3f negN(center - normal*(height/2));
	Vec3f posB(center + binormal*(width/2));
	Vec3f negB(center - binormal*(width/2));

	Vec3f v0(posT - binormal*(width/2) + normal*(height/2));
	Vec3f v1(posT + binormal*(width/2) + normal*(height/2));
	Vec3f v2(negT + binormal*(width/2) + normal*(height/2));
	Vec3f v3(negT - binormal*(width/2) + normal*(height/2));

	Vec3f v4(posT - binormal*(width/2) - normal*(height/2));
	Vec3f v5(posT + binormal*(width/2) - normal*(height/2));
	Vec3f v6(negT + binormal*(width/2) - normal*(height/2));
	Vec3f v7(negT - binormal*(width/2) - normal*(height/2));

	drawFace(v0, v1, v2, v3, center);
	drawFace(v4, v5, v6, v7, center);
	drawFace(v7, v3, v2, v6, center);
	drawFace(v4, v0, v1, v5, center);
	drawFace(v6, v2, v1, v5, center);
	drawFace(v7, v3, v0, v4, center);
}

void drawAllCrossbars(){
	for(int i=1; i<all_spline_points.size()-pixelWidth/2; i+=pixelWidth){
		drawSingleCrossbar(
			all_spline_points[i],
			all_tangent_spline_points[i],
			all_normal_points[i],
			all_binormal_points[i]
			);
	}
}

void loadTexture (char *filename, GLuint &textureID)
{
	Pic *pBitMap = pic_read(filename, NULL);

	if(pBitMap == NULL)
	{
		printf("Could not load the texture file %s\n", filename);
		exit(1);
	}

	glEnable(GL_TEXTURE_2D); 
	glGenTextures(1, &textureID); 

	glBindTexture(GL_TEXTURE_2D, textureID); 

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, pBitMap->nx, pBitMap->ny, GL_RGB, GL_UNSIGNED_BYTE, pBitMap->pix);
	glDisable(GL_TEXTURE_2D);
	pic_free(pBitMap); 
}

//-----------------------------------------------------------------------------

void processSpecialKeys(int key, int xx, int yy) {
	// this is for debug view only

	float fraction = 0.1f;

	switch (key) {
		case GLUT_KEY_LEFT :
			angle -= 0.03f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
		case GLUT_KEY_RIGHT :
			angle += 0.03f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
		case GLUT_KEY_UP :
			x += lx * fraction;
			z += lz * fraction;
			break;
		case GLUT_KEY_DOWN :
			x -= lx * fraction;
			z -= lz * fraction;
			break;
		case GLUT_KEY_PAGE_UP :
			angleY += 0.03f;
			ly = sin(angleY);
			break;
		case GLUT_KEY_PAGE_DOWN :
			angleY -= 0.03f;
			ly = sin(angleY);
			break;
	}

}

//-----------------------------------------------------------------------------

void drawGround(){

	glEnable(GL_TEXTURE_2D);								
	glDisable(GL_DEPTH_TEST);							
	glDepthMask(false);		

	glBindTexture(GL_TEXTURE_2D, deckTextureId);  
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0); 	

	glTexCoord2f(0.0 ,0.0);		
	glVertex3f(-xMax, -yMax, zMax);
	glTexCoord2f(0.0 ,1.0);
	glVertex3f(-xMax, -yMax, -zMax);
	glTexCoord2f(1.0 ,1.0);
	glVertex3f(xMax, -yMax, -zMax);
	glTexCoord2f(1.0 ,0.0);
	glVertex3f(xMax, -yMax, zMax);

	glEnd();
	glDisable(GL_TEXTURE_2D); 
}

void drawRear(){
	
	glEnable(GL_TEXTURE_2D);								
	glDisable(GL_DEPTH_TEST);								
	glDepthMask(false);		

	glBindTexture(GL_TEXTURE_2D, rearTextureId);  
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0); 	

	glTexCoord2f(0.0 ,0.0);		
	glVertex3f(-xMax, yMax, -zMax);
	glTexCoord2f(0.0 ,1.0);
	glVertex3f(-xMax, -yMax, -zMax);
	glTexCoord2f(1.0 ,1.0);
	glVertex3f(xMax, -yMax, -zMax);
	glTexCoord2f(1.0 ,0.0);
	glVertex3f(xMax, yMax, -zMax);

	glEnd();
	glDisable(GL_TEXTURE_2D); 
}

void drawCeiling(){
	
	glEnable(GL_TEXTURE_2D);								
	glDisable(GL_DEPTH_TEST);								
	glDepthMask(false);		

	glBindTexture(GL_TEXTURE_2D, ceilingTextureId);  
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0); 	

	glTexCoord2f(0.0 ,0.0);		
	glVertex3f(-xMax, yMax, zMax);
	glTexCoord2f(0.0 ,1.0);
	glVertex3f(-xMax, yMax, -zMax);
	glTexCoord2f(1.0 ,1.0);
	glVertex3f(xMax, yMax, -zMax);
	glTexCoord2f(1.0 ,0.0);
	glVertex3f(xMax, yMax, zMax);

	glEnd();
	glDisable(GL_TEXTURE_2D); 
}

void drawRight(){
	
	glEnable(GL_TEXTURE_2D);								
	glDisable(GL_DEPTH_TEST);								
	glDepthMask(false);		

	glBindTexture(GL_TEXTURE_2D, rightTextureId);  
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0); 	

	glTexCoord2f(0.0 ,0.0);		
	glVertex3f(xMax, yMax, -zMax);
	glTexCoord2f(0.0 ,1.0);
	glVertex3f(xMax, -yMax, -zMax);
	glTexCoord2f(1.0 ,1.0);
	glVertex3f(xMax, -yMax, zMax);
	glTexCoord2f(1.0 ,0.0);
	glVertex3f(xMax, yMax, zMax);

	glEnd();
	glDisable(GL_TEXTURE_2D); 
}

void drawLeft(){
	
	glEnable(GL_TEXTURE_2D);								
	glDisable(GL_DEPTH_TEST);							
	glDepthMask(false);		

	glBindTexture(GL_TEXTURE_2D, leftTextureId);  
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0); 	

	glTexCoord2f(0.0 ,0.0);		
	glVertex3f(-xMax, yMax, zMax);
	glTexCoord2f(0.0 ,1.0);
	glVertex3f(-xMax, -yMax, zMax);
	glTexCoord2f(1.0 ,1.0);
	glVertex3f(-xMax, -yMax, -zMax);
	glTexCoord2f(1.0 ,0.0);
	glVertex3f(-xMax, yMax, -zMax);

	glEnd();
	glDisable(GL_TEXTURE_2D); 
}

void drawFront(){
	
	glEnable(GL_TEXTURE_2D);								
	glDisable(GL_DEPTH_TEST);								
	glDepthMask(false);		

	glBindTexture(GL_TEXTURE_2D, frontTextureId);  
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0); 	

	glTexCoord2f(0.0 ,0.0);		
	glVertex3f(xMax, yMax, zMax);
	glTexCoord2f(0.0 ,1.0);
	glVertex3f(xMax, -yMax, zMax);
	glTexCoord2f(1.0 ,1.0);
	glVertex3f(-xMax, -yMax, zMax);
	glTexCoord2f(1.0 ,0.0);
	glVertex3f(-xMax, yMax, zMax);

	glEnd();
	glDisable(GL_TEXTURE_2D); 
}


//------------------------------------------------------------------
extern double coef_RailWidth;
extern std::vector<Vec3f> leftRailPoints;
extern std::vector<Vec3f> rightRailPoints;

void calcLeftRail(){
	for(int i=0; i<all_spline_points.size();i++){
		double x = all_spline_points[i].x();
		double y = all_spline_points[i].y();
		double z = all_spline_points[i].z();
		double leftX = all_binormal_points[i].x();
		double leftY = all_binormal_points[i].y();
		double leftZ = all_binormal_points[i].z();
		Vec3f cur(x-leftX*coef_RailWidth,y-leftY*coef_RailWidth,z-leftZ*coef_RailWidth);
		leftRailPoints.push_back(cur);
	}	
	Vec3f cur(
		all_spline_points[0].x()-all_binormal_points[0].x()*coef_RailWidth,
		all_spline_points[0].y()-all_binormal_points[0].y()*coef_RailWidth,
		all_spline_points[0].z()-all_binormal_points[0].z()*coef_RailWidth);
	leftRailPoints.push_back(cur);
}

void calcRightRail(){
	for(int i=0; i<all_spline_points.size();i++){
		double x = all_spline_points[i].x();
		double y = all_spline_points[i].y();
		double z = all_spline_points[i].z();
		double leftX = all_binormal_points[i].x();
		double leftY = all_binormal_points[i].y();
		double leftZ = all_binormal_points[i].z();
		Vec3f cur(x+leftX*coef_RailWidth,y+leftY*coef_RailWidth,z+leftZ*coef_RailWidth);
		rightRailPoints.push_back(cur);
	}	
	Vec3f cur(
		all_spline_points[0].x()+all_binormal_points[0].x()*coef_RailWidth, 
		all_spline_points[0].y()+all_binormal_points[0].y()*coef_RailWidth, 
		all_spline_points[0].z()+all_binormal_points[0].z()*coef_RailWidth );
	rightRailPoints.push_back(cur);	
}

///  for recording

int picIndex=0;
char picnum[8]={'0','0','0','.','j','p','g','\0'};

void saveScreenshot (char *filename)
{
  int i, j;
  Pic *in = NULL;

  if (filename == NULL)
    return;

  /* Allocate a picture buffer */
  in = pic_alloc(640, 480, 3, NULL);

  printf("File to save to: %s\n", filename);

  for (i=479; i>=0; i--) {
    glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 &in->pix[i*in->nx*in->bpp]);
  }

  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);
}

void updatePicNum(int index){
	for(int i=0; i<3; i++){
		picnum[2-i]=index%10+'0';
		index/=10;
		if(index==0)break;
	}
}

void recordScreen(int index){
	updatePicNum(index);
	saveScreenshot(picnum);
}
// <- recording