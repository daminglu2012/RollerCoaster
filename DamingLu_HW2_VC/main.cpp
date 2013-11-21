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
#include "funcs.h"

#include <cstdlib>
#include <iostream>

SplineData g_Track;

int g_iMenuId;

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;
int xMax = 200;
int yMax = 200;
int zMax = 200;

std::vector<Vec3f> all_spline_points;
std::vector<Vec3f> all_tangent_spline_points;
int allSplinePtsIter=0;

double coef_RailWidth = 0.03;

std::vector<Vec3f> all_normal_points;
std::vector<Vec3f> all_binormal_points;

std::vector<Vec3f> allPoints;

// -> move camera
// angle of rotation for the camera direction
float angle=0.0,angleY = 0.0;
// actual vector representing the camera's direction
float lx=0.0f,lz=0.0f, ly=0.0f;
// XZ position of the camera
float x=0.0f,z=1.0f, y=-150.0f;
const Vec3f startPoint(0,y,0);
// <- move camera

int recordIter = 0; // for recording

// -> skybox
GLuint deckTextureId;

GLuint frontTextureId;
GLuint rearTextureId;
GLuint leftTextureId;
GLuint rightTextureId;
GLuint ceilingTextureId;

GLuint crossbarTextureId;
// <- skybox

std::vector<Vec3f> leftRailPoints;
std::vector<Vec3f> rightRailPoints;

void InitGL ( GLvoid );
void doIdle();
void display ( void );
void keyboardfunc (unsigned char key, int x, int y) ;
void menufunc(int value);
void call_gluLookAt();
void call_debug_LookAt();
void drawLeftRail();
void drawRightRail();
//
//------------------------------------------------------------------------------------

int main ( int argc, char** argv )   
{
	if (argc!=2)
	{
		printf("USAGE: %s trackfilname\n", argv[0]);
		exit(1);
	}

	g_Track.loadSplineFrom(argv[1]);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow( "Daming Lu's HW2"); 
	
	InitGL ();
	
	glutDisplayFunc(display);
	
	g_iMenuId = glutCreateMenu(menufunc);	
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Quit",0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutIdleFunc(doIdle);
	glutKeyboardFunc(keyboardfunc);
	glutSpecialFunc(processSpecialKeys);
	glutMainLoop( );         

	return 0;
}

void InitGL ( GLvoid )     
{
	glMatrixMode( GL_PROJECTION );						
	glLoadIdentity();	
		gluPerspective( 120.0f,										
		(GLfloat)WINDOW_WIDTH/(GLfloat)WINDOW_HEIGHT, 
		0.01f, 
		2000.0f );
	glEnable(GL_DEPTH_TEST);									
	glDepthFunc(GL_LEQUAL);										
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);			
	
	loadTexture( "texture/deck.jpg", deckTextureId );	
	loadTexture( "texture/front.jpg", frontTextureId );		
	loadTexture( "texture/left.jpg", leftTextureId );			
	loadTexture( "texture/right.jpg", rightTextureId );		
	loadTexture( "texture/rear.jpg", rearTextureId );			
	loadTexture( "texture/ceiling.jpg", ceilingTextureId );	
	loadTexture( "texture/Basebrd.jpg", crossbarTextureId );	
	
	Vec3f currentpos(startPoint);
	allPoints.push_back(currentpos);

	glVertex3f(currentpos.x(),currentpos.y(),currentpos.z());
	
	for(pointVectorIter ptsiter = g_Track.points().begin(); 
		ptsiter  !=  g_Track.points().end(); ptsiter++)	{
		
		Vec3f pt(*ptsiter);
		currentpos += pt;
		allPoints.push_back(currentpos);
	}

	int ptsLength = allPoints.size();

	for(int i=0; i<ptsLength-1; i++){
		Vec3f p0,p1,p2,p3; // 1->2
		if(i == 0){ // 0
			p0 = allPoints[ptsLength-1];
			p1 = allPoints[i];
			p2 = allPoints[i+1];
			p3 = allPoints[i+2];
		}else if(i == ptsLength-2){ // n-2
			p3 = allPoints[0];
			p1 = allPoints[i];
			p2 = allPoints[i+1];
			p0 = allPoints[i-1];
		}else if(i == ptsLength-1){ // n-1
			p2 = allPoints[0];
			p3 = allPoints[1];
			p1 = allPoints[i];
			p0 = allPoints[i-1];
		}else{ // normal
			p0 = allPoints[i-1];
			p1 = allPoints[i];
			p2 = allPoints[i+1];
			p3 = allPoints[i+2];
		}
		recordAllSplinePoints(p0,p1,p2,p3); // between p1 and p2
	}
	// T N B
	calcNormalBinormal();
	calcLeftRail();
	calcRightRail();
}

void display ( void )  
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);		
	glMatrixMode(GL_PROJECTION);					
	glLoadIdentity();								
	
	gluPerspective(45.0f,(GLfloat)WINDOW_WIDTH/(GLfloat)WINDOW_HEIGHT,0.001f,1000.0f);

	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();

	//call_debug_LookAt(); // You can use this func instead to review the whole design
	call_gluLookAt();

	drawGround();
	
	drawRear();
	drawCeiling();
	drawRight();
	drawLeft();
	drawFront();
	
	drawAllCrossbars();
	
	drawLeftRail();
	drawRightRail();

	glutSwapBuffers();
	// below is for recording use only
	/*
	if((recordIter++)%3==0){
		recordScreen(picIndex++);
	}
	*/
}

void drawLeftRail(){
	glLineWidth(6);
	glBegin(GL_LINE_STRIP);
	glColor3f( 0.3, 0.3, 0.3 ); // R G B
	for(int i=0; i<leftRailPoints.size();i++){
		glVertex3f(leftRailPoints[i].x(),leftRailPoints[i].y(),leftRailPoints[i].z());
	}	
	glEnd();
}
void drawRightRail(){
	glLineWidth(6);
	glBegin(GL_LINE_STRIP);
	glColor3f( 0.3, 0.3, 0.3 ); // R G B
	for(int i=0; i<rightRailPoints.size();i++){
		glVertex3f(rightRailPoints[i].x(),rightRailPoints[i].y(),rightRailPoints[i].z());
	}	
	glEnd();
}

void call_debug_LookAt(){
	gluLookAt(	x, y+0.5, z,
				x+lx, y+0.5+ly,  z+lz,
				0.0f, 0.1f,  0.0f);
}

void keyboardfunc (unsigned char key, int x, int y) {
	if(key == 'q' || key == 'Q' || key == 27) {
		exit(0);
	}
}

void menufunc(int value)
{
	switch (value)
	{
	case 0:
		exit(0);
		break;
	}
}

void doIdle()
{
	glutPostRedisplay();
}
void call_gluLookAt(){
	gluLookAt(
		// lift the camera a bit to feel sitting on the car
		all_spline_points[allSplinePtsIter].x()+all_normal_points[allSplinePtsIter].x()/20,
		all_spline_points[allSplinePtsIter].y()+all_normal_points[allSplinePtsIter].y()/20,
		all_spline_points[allSplinePtsIter].z()+all_normal_points[allSplinePtsIter].z()/20,
		all_spline_points[allSplinePtsIter].x()+all_tangent_spline_points[allSplinePtsIter].x(),
		all_spline_points[allSplinePtsIter].y()+all_tangent_spline_points[allSplinePtsIter].y(),
		all_spline_points[allSplinePtsIter].z()+all_tangent_spline_points[allSplinePtsIter].z(),
		all_normal_points[allSplinePtsIter].x(),
		all_normal_points[allSplinePtsIter].y(),
		all_normal_points[allSplinePtsIter].z()
		);

	allSplinePtsIter++;
	if(allSplinePtsIter>=all_spline_points.size())
		allSplinePtsIter=0;
}