// Cubica

#include <stdlib.h>

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// the global Assimp scene object
const aiScene* scene01 = NULL;
const aiScene* scene02 = NULL;
const aiScene* scene03 = NULL;

GLuint scene_list = 0;
aiVector3D scene_min, scene_max, scene_center;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

using namespace std;

#define DEF_floorGridScale	1.0
#define DEF_floorGridXSteps	10.0
#define DEF_floorGridZSteps	10.0

#include "glm.h"

//Variables para la carga de las texturas
static GLuint texName;
int iheight, iwidth;
unsigned char* image = NULL;

static GLuint texName2;
int iheight2, iwidth2;
unsigned char* image2 = NULL;

static GLuint texName3;
int iheight3, iwidth3;
unsigned char* image3 = NULL;

static GLuint texSkyBox[6];
int iheightSkyBox[6], iwidthSkyBox[6];
unsigned char* imageSkyBox[6];

//Variables globales
GLfloat cAmb[] = { 0.5f, 0.5f, 0.5f, 1.0f }; // Componente ambiental de los modelos.

GLfloat iLuz[] = {0.7,0.7,0.7,1.0};
GLfloat cLuz[] = {1.0,1.0,1.0,1.0};
GLfloat light_position[] = { 0.0, 200.0, 0.0, 1.0 };
GLfloat spotlight_direction[] = {0.0, -1.0, 0.0};

GLfloat cutoff = 50.0f; // Cutoff del spotlight.
GLfloat exponent = 25.0f; // Exponent del spotlight.

GLfloat r = 1.0; // Color Red del conejo.
GLfloat g = 1.0; // Color Green del conejo.
GLfloat b = 1.0; // Color Blue del conejo.

GLint reflex = 1; // Reflexión
GLint sReflex = 1; // Solo reflexión. Sin iluminación.

void changeViewport(int w, int h) {
	
	float aspectratio;

	if (h==0)
		h=1;

	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 3000.0);
	glMatrixMode (GL_MODELVIEW);

}

//Funcion para inicializar las texturas
void init_texturas(){

	//Cargar el plano
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	image = glmReadPPM("texAO_plano.ppm", &iwidth, &iheight);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	//Cargar columnas
	glGenTextures(1, &texName2);
	glBindTexture(GL_TEXTURE_2D, texName2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	image2 = glmReadPPM("texAO_columna.ppm", &iwidth2, &iheight2);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth2, iheight2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);

	//Cargar conejo
	glGenTextures(1, &texName3);
	glBindTexture(GL_TEXTURE_2D, texName3);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	image3 = glmReadPPM("texAO_bunny.ppm", &iwidth3, &iheight3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth3, iheight3, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);
	
}

void init_cubeMap(){

	glGenTextures(6,texSkyBox);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,texSkyBox[0]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,texSkyBox[1]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,texSkyBox[2]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X,texSkyBox[3]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,texSkyBox[4]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,texSkyBox[5]);

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

	int i;
	int numTexturas = 6;

	// There is always six filenames
	for (i = 0;i < numTexturas;++i) {

		if( i == 0){

			imageSkyBox[i] =  glmReadPPM("negx.ppm", &iwidthSkyBox[i], &iheightSkyBox[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,0,GL_RGB,iwidthSkyBox[i],iheightSkyBox[i],0,GL_RGB,GL_UNSIGNED_BYTE,imageSkyBox[i]);

		} else if( i == 1){
	  
			imageSkyBox[i] = glmReadPPM("negy.ppm", &iwidthSkyBox[i], &iheightSkyBox[i]); 
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,0,GL_RGB,iwidthSkyBox[i],iheightSkyBox[i],0,GL_RGB,GL_UNSIGNED_BYTE,imageSkyBox[i]);

		} else if( i == 2){
	  
			imageSkyBox[i] =  glmReadPPM("negz.ppm", &iwidthSkyBox[i], &iheightSkyBox[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,0,GL_RGB,iwidthSkyBox[i],iheightSkyBox[i],0,GL_RGB,GL_UNSIGNED_BYTE,imageSkyBox[i]);

		} else if( i == 3) {

			imageSkyBox[i] =  glmReadPPM("posx.ppm", &iwidthSkyBox[i], &iheightSkyBox[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,0,GL_RGB,iwidthSkyBox[i],iheightSkyBox[i],0,GL_RGB,GL_UNSIGNED_BYTE,imageSkyBox[i]);
	  
		} else if (i == 4){
	  
			imageSkyBox[i] =  glmReadPPM("posy.ppm", &iwidthSkyBox[i], &iheightSkyBox[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,0,GL_RGB,iwidthSkyBox[i],iheightSkyBox[i],0,GL_RGB,GL_UNSIGNED_BYTE,imageSkyBox[i]);

		} else if( i == 5){

			imageSkyBox[i] =  glmReadPPM("posz.ppm", &iwidthSkyBox[i], &iheightSkyBox[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,0,GL_RGB,iwidthSkyBox[i],iheightSkyBox[i],0,GL_RGB,GL_UNSIGNED_BYTE,imageSkyBox[i]);
	  
		}
   
		
	}

}

void init_skybox(){

	/*
	glGenTextures(6,texSkyBox2);
	glBindTexture(GL_TEXTURE_2D,texSkyBox2[0]);
	glBindTexture(GL_TEXTURE_2D,texSkyBox2[1]);
	glBindTexture(GL_TEXTURE_2D,texSkyBox2[2]);
	glBindTexture(GL_TEXTURE_2D,texSkyBox2[3]);
	glBindTexture(GL_TEXTURE_2D,texSkyBox2[4]);
	glBindTexture(GL_TEXTURE_2D,texSkyBox2[5]);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT );
	*/

}

void init(){

	glClearColor(0.0,0.0,0.0,0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
   
	glEnable(GL_COLOR_MATERIAL); 
   
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
	glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
     
	//Funcion para inicializar skybox
	init_cubeMap();
	//init_skybox();

	//Funcion para inicializar texturas
	init_texturas();

}
	
//Funcion para cargar las texturas
void cargar_texturas(int idx){

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Activar textura plano
	if( idx == 0){

		glBindTexture(GL_TEXTURE_2D, texName);
			
	//Activar textura columna
	}else if( idx == 1){

		glBindTexture(GL_TEXTURE_2D, texName2);
			
	//Activar textura conejo	
	}else if( idx == 2){

		glBindTexture(GL_TEXTURE_2D, texName3);
		   		
	}
	
}


void cargar_materiales(int idx) {

	// Material Piso
	if (idx == 0){	
	
		GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat high_shininess[] = { 80.0 };

		glMaterialfv(GL_FRONT, GL_AMBIENT, cAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	}

	// Material Columna
	if (idx == 1){

		GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat high_shininess[] = { 80.0 };

		glMaterialfv(GL_FRONT, GL_AMBIENT, cAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	}

	// Material Conejo
	if (idx == 2){

		GLfloat mat_diffuse[] = { r, g, b, 1.0 };
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat high_shininess[] = { 80.0 };

		glMaterialfv(GL_FRONT, GL_AMBIENT, cAmb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
		
	}

}

void recursive_render (const aiScene *sc, const aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	aiMatrix4x4 m = nd->mTransformation;

	// update transform
	aiTransposeMatrix4(&m);

	glPushMatrix();
	
	glMultMatrixf((float*)&m);

	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n) {
		
		const aiMesh* mesh = scene01->mMeshes[nd->mMeshes[n]];

		for (t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch(face->mNumIndices) {
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for(i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				
				if(mesh->mColors[0] != NULL)
					glColor4fv((GLfloat*)&mesh->mColors[0][index]);
				
				if(mesh->mNormals != NULL) 
					glNormal3fv(&mesh->mNormals[index].x);
				
				if (mesh->HasTextureCoords(0)) 
					glTexCoord2f(mesh->mTextureCoords[0][index].x, 1-mesh->mTextureCoords[0][index].y);
				
				glVertex3fv(&mesh->mVertices[index].x);
			}

			glEnd();
		}

	}

	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n) {
		cargar_texturas(n);
		cargar_materiales(n);
		recursive_render(sc, nd->mChildren[n]);
	}

	glPopMatrix();

}


void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
	case 27:             
		exit (0);
		break;

	case 81: case 113: //tecla q

		cutoff += 2.0;
		break;

	case 87: case 119: //tecla w

		if(cutoff > 0.0){
			cutoff -= 2.0;
		} 

		break;

	case 65: case 97: //tecla a

		exponent += 2.0;
		break;

	case 83: case 115: //tecla s

		exponent -= 2.0;
		break;

	case 90: case 122: //tecla z

		if (cAmb[0] < 1.0) {
			cAmb[0] += 0.1;
			cAmb[1] += 0.1;
			cAmb[2] += 0.1;
		}
		break;

	case 88: case 120: //tecla x

		if(cAmb[0] > 0.0) {
			cAmb[0] -= 0.1;
			cAmb[1] -= 0.1;
			cAmb[2] -= 0.1;
		}
		break;

	case 69: case 101: //tecla e

		spotlight_direction[0] += 0.1;
		break;

	case 68: case 100: //tecla d

		spotlight_direction[0] -= 0.1;
		break;

	case 82: case 114: //tecla r

		spotlight_direction[2] += 0.1;
		break;

	case 70: case 102: //tecla f

		spotlight_direction[2] -= 0.1;
		break;

	case 84: case 116: //tecla t

		if(r < 1.0){
			r += 0.1;
		}
		break;

	case 71: case 103: //tecla g

		if(r > 0.0){
			r -= 0.1;
		}
		break;

	case 89: case 121: //tecla y

		if(g < 1.0){
			g += 0.1;
		}
		break;

	case 72: case 104: //tecla h

		if(g > 0.0){
			g -= 0.1;
		}
		break;

    case 85: case 117: //tecla u

		if(b < 1.0){
			b += 0.1;
		}
		break;

	case 74: case 106: //tecla j

		if(b > 0.0){
			b -= 0.1;
		}
		break;

	case 67: case 99: //tecla c

		if(reflex == 0){
			glEnable(GL_TEXTURE_CUBE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
			reflex = 1;
		} else {
			glDisable(GL_TEXTURE_CUBE_MAP);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			reflex = 0;		
		}
		break;

	case 86: case 118: //tecla v

		if(sReflex == 0){
			glEnable(GL_LIGHTING);
			sReflex = 1;
		} else {
			glDisable(GL_LIGHTING);
			sReflex = 0;
		}
		break;

	case 66: case 98: //tecla b

		if (iLuz[0] < 1.0) {
			iLuz[0] += 0.1;
			iLuz[1] += 0.1;
			iLuz[2] += 0.1;
		}
		break;

	case 78: case 110: //tecla n

		if (iLuz[0] > 0.0) {
			iLuz[0] -= 0.1;
			iLuz[1] -= 0.1;
			iLuz[2] -= 0.1;
		}
		break;

	case 49: //tecla 1

		cLuz[0] = 1.0f;
		cLuz[1] = 1.0f;
		cLuz[2] = 1.0f;
		cLuz[3] = 1.0f;
		break;

	case 50: //tecla 2

		cLuz[0] = 0.0f;
		cLuz[1] = 1.0f;
		cLuz[2] = 1.0f;
		cLuz[3] = 1.0f;
		break;

	case 51: //tecla 3

		cLuz[0] = 0.0f;
		cLuz[1] = 1.0f;
		cLuz[2] = 0.0f;
		cLuz[3] = 1.0f;
		break;

	case 52: //tecla 4

		cLuz[0] = 1.0f;
		cLuz[1] = 0.0f;
		cLuz[2] = 1.0f;
		cLuz[3] = 1.0f;
		break;

	case 53: //tecla 5

		cLuz[0] = 0.8f;
		cLuz[1] = 0.5f;
		cLuz[2] = 0.2f;
		cLuz[3] = 1.0f;
		break;

  }

  scene_list = 0;
  glutPostRedisplay();
}

void DibujarObjetos3D() {

	glLightfv(GL_LIGHT0, GL_AMBIENT, iLuz);
   	glLightfv(GL_LIGHT0, GL_DIFFUSE, cLuz);
	glLightfv(GL_LIGHT0, GL_SPECULAR, iLuz);
   	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotlight_direction);
   	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
   	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glEnable(GL_LIGHT0);

}

void DibujarSkyBox() {

	// Center the Skybox around the given x,y,z position
	float x = 0;
	float y = 0;
	float z = 0;
	float width  = 302; // 302
	float height = 302;
	float length = 302;
	// Center the skybox
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2 + 151;
	// Draw Front side
	//glBindTexture(GL_TEXTURE_2D, texSkyBox[0]); // front
	/*glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height, z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glEnd();*/
	// Draw Back side
	//glBindTexture(GL_TEXTURE_2D, texSkyBox[1]); // back
	/*glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();*/
	// Draw Left side
	//glBindTexture(GL_TEXTURE_2D, texSkyBox[2]); // left
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z);		
	glEnd();
	// Draw Right side
	//glBindTexture(GL_TEXTURE_2D, texSkyBox[3]); // rigth
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height,	z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height,	z);
	glEnd();
	// Draw Up side
	//glBindTexture(GL_TEXTURE_2D, texSkyBox[4]); // Up
	/*glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y+height, z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glEnd();*/
	// Draw Down side
	//glBindTexture(GL_TEXTURE_2D, texSkyBox[5]); // down
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y,		z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y,		z);
	glEnd();

}


void render(){

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity ();                       
	gluLookAt (0, 80, 250, 0.0, 15.0, 0.0, 0.0, 1.0, 0.0);

	DibujarObjetos3D();

	glPushMatrix();
		glRotatef(90.0f,1.0f,0.0f,0.0f);		
		glRotatef(180.0f,0.0f,1.0f,0.0f);
		//glRotatef(180.0f,0.0f,0.0f,1.0f);
		DibujarSkyBox();
	glPopMatrix();

	//Suaviza las lineas
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );	


	glPushMatrix();
	glEnable(GL_NORMALIZE);
	if(scene_list == 0) {
	    scene_list = glGenLists(1);
	    glNewList(scene_list, GL_COMPILE);
            // now begin at the root node of the imported data and traverse
            // the scenegraph by multiplying subsequent local transforms
            // together on GL's matrix stack.		
	    recursive_render(scene01, scene01->mRootNode);
	    glEndList();
	}
	glCallList(scene_list);
	
	glPopMatrix();

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glutSwapBuffers();
}

void animacion(int value) {

	glutTimerFunc(2.0,animacion,1);
    glutPostRedisplay();
	
}

void get_bounding_box_for_node (const aiNode* nd, 
	aiVector3D* min, 
	aiVector3D* max, 
	aiMatrix4x4* trafo
){
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	
		prev = *trafo;
		aiMultiplyMatrix4(trafo,&nd->mTransformation);

		for (; n < nd->mNumMeshes; ++n) {
			const aiMesh* mesh = scene01->mMeshes[nd->mMeshes[n]];
				for (t = 0; t < mesh->mNumVertices; ++t) {

				aiVector3D tmp = mesh->mVertices[t];
				aiTransformVecByMatrix4(&tmp,trafo);

				min->x = aisgl_min(min->x,tmp.x);
				min->y = aisgl_min(min->y,tmp.y);
				min->z = aisgl_min(min->z,tmp.z);

				max->x = aisgl_max(max->x,tmp.x);
				max->y = aisgl_max(max->y,tmp.y);
				max->z = aisgl_max(max->z,tmp.z);
			}
		}

		for (n = 0; n < nd->mNumChildren; ++n) {
			get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
		}
		*trafo = prev;

}

void get_bounding_box (aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 trafo;
	
		aiIdentityMatrix4(&trafo);
	
		min->x = min->y = min->z =  1e10f;
		max->x = max->y = max->z = -1e10f;
		get_bounding_box_for_node(scene01->mRootNode,min,max,&trafo);


}

int loadasset (const char* path)
{
	// we are taking one of the postprocessing presets to avoid
	// spelling out 20+ single postprocessing flags here.
			
		scene01 = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene01) {
			get_bounding_box(&scene_min,&scene_max);
			scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
			scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
			scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
			return 0;
		}

	return 1;
}

int main (int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960,540);

	glutCreateWindow("Test Opengl");

	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
	aiAttachLogStream(&stream);

	// ... same procedure, but this stream now writes the
	// log messages to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
	aiAttachLogStream(&stream);

	// the model name can be specified on the command line. If none
	// is specified, we try to locate one of the more expressive test 
	// models from the repository (/models-nonbsd may be missing in 
	// some distributions so we need a fallback from /models!).
	//if( 0 != loadasset( argc >= 2 ? argv[1] : "dragon_vrip_res2.ply")) {
	//	if( argc != 1 || (0 != loadasset( "dragon_vrip_res2.ply") && 0 != loadasset( "dragon_vrip_res2.ply"))) { 
	//		return -1;
	//	}
	//}

	if (loadasset( "escenario.obj") != 0) {
		return -1;
	}

	init ();

	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
	return 0;

}
