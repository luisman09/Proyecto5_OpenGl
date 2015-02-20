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

//Variables globales

GLfloat cutoff = 50.0f; // Cutoff del spotlight.
GLfloat exponent = 25.0f; // Exponent del spotlight.
GLfloat cAmb = 0.5f; // Componente ambiental de los modelos.
GLfloat posX = 0.0f; // Posición en X del spotlight.
GLfloat posZ = 0.0f; // Posición en Z del spotlight.

GLfloat r = 1.0; // Color Red del conejo.
GLfloat g = 1.0; // Color Green del conejo.
GLfloat b = 1.0; // Color Blue del conejo.

GLint reflex = 0; // Reflexión
GLint sReflex = 0; // Solo reflexión. Sin iluminación.

GLfloat iLuz[] = { 1.0, 1.0, 1.0, 1.0 }; // Intensidad de la luz. // SPECULAR
GLfloat cLuz[] = { 1.0, 1.0, 1.0, 1.0 }; // Color de la luz. // DIFFUSE


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

	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Cargar textura del plano
	glGenTextures(1, &texName);
	image = glmReadPPM("texAO_plano.ppm", &iwidth, &iheight);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	
	//Cargar textura de la columna
	glGenTextures(1, &texName2);
	image2 = glmReadPPM("texAO_columna.ppm", &iwidth2, &iheight2);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth2, iheight2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
		
	//Cargar textura del conejo

	glGenTextures(1, &texName3);
	image3 = glmReadPPM("texAO_bunny.ppm", &iwidth3, &iheight3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth3, iheight3, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);
		
}


void init(){

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   
   //Funcion para inicializar texturas
   init_texturas();

   glEnable(GL_COLOR_MATERIAL);
   glShadeModel(GL_SMOOTH);


}

//Funcion para cargar las texturas
void cargar_texturas(int idx){

	if( idx == 0){
			
			glBindTexture(GL_TEXTURE_2D, texName);

	}else if( idx == 1){

			glBindTexture(GL_TEXTURE_2D, texName2);

	}else if( idx == 2){

			glBindTexture(GL_TEXTURE_2D, texName3);
					
	}

		
}


void cargar_materiales(int idx) {


	// Material Piso
	if (idx == 0){	

		


	}

	// Material Columna
	if (idx == 1){

		
	}

	// Material Conejo
	if (idx == 2){

		
	}

}

void recursive_render (const aiScene *sc, const aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	aiMatrix4x4 m = nd->mTransformation;

	// update transform
	aiTransposeMatrix4(&m);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

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
		cargar_materiales(n);
		cargar_texturas(n);
		recursive_render(sc, nd->mChildren[n]);
	}

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}


void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
	case 27:             
		exit (0);
		break;

	case 81: case 113: //tecla q

		cutoff += 0.1;
		break;

	case 87: case 119: //tecla w

		cutoff -= 0.1;
		break;

	case 65: case 97: //tecla a

		exponent += 0.1;
		break;

	case 83: case 115: //tecla s

		exponent -= 0.1;
		break;

	case 90: case 122: //tecla z

		if (cAmb < 1.0) {
			cAmb += 0.1;
		}
		break;

	case 88: case 120: //tecla x

		if(cAmb > 0.0) {
			cAmb -= 0.1;
		}
		break;

	case 69: case 101: //tecla e

		posX += 0.1;
		break;

	case 68: case 100: //tecla d

		posX -= 0.1;
		break;

	case 82: case 114: //tecla r

		posZ += 0.1;
		break;

	case 70: case 102: //tecla f

		posZ -= 0.1;
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
			reflex = 1;
		} else {
			reflex = 0;		
		}
		break;

	case 86: case 118: //tecla v

		if(sReflex == 0){
			sReflex = 1;
		} else {
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


void render(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glLoadIdentity ();                       
	gluLookAt (0, 80, 250, 0.0, 15.0, 0.0, 0.0, 1.0, 0.0);

	// Luz
	glEnable(GL_LIGHTING);

	GLfloat global_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	/*
	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 200.0, 0.0, 1.0 };
	GLfloat spotlight_position[] = {posX, -1.0, posZ};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, cLuz);
	glLightfv(GL_LIGHT0, GL_SPECULAR, iLuz);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); 

	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION, spotlight_position);
	*/
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 200.0, 0.0, 1.0 };
	GLfloat spotlight_position[] = {posX, -1.0, posZ};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); 

	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION, spotlight_position);

	glEnable(GL_LIGHT0);

	GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat high_shininess[] = { 10.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess); 

	

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
	//glutTimerFunc(10,animacion,1);
	glutKeyboardFunc (Keyboard);
	


	glutMainLoop();
	return 0;

}
