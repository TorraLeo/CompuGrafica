/*
Animación por keyframes
La textura del skybox fue conseguida desde la página https ://opengameart.org/content/elyvisions-skyboxes?page=1
y edité en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
	*/
	//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <fstream>
#include <sstream>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;
float ciclo_xneg = 0.0f, ciclo_xneg2 = 0.0f;
float ciclo_ypos = 0.0f, ciclo_ypos2 = 0.0f;
float ciclo_yneg = 0.0f, ciclo_yneg2 = 0.0f;
float ciclo_zrot = 0.0f, ciclo_zrot2 = 0.0f;
float rotZ = 0.0f; // grados

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Skybox skybox;
Texture pisoTexture;

// MATERIALES
Material Material_brillante;
Material Material_opaco;



// MODELOS CANCHA DE JUEGO DE PELOTA
Model Pelota_M;
Model Pared_M;
Model Aro_M;

// MODELO Y TEXTURA PIRÁMIDE
Model Piramide_M;
Texture Piramide;

// MODELOS RING
Model Ring_M;
Model Chairs_M;

//TEXTURAS RING
Texture Ring;
Texture Chairs;

	//MODELOS DE TIENDA
Model Palomitas_M;
Model Soda_M;
Model Cartel_M;

	//TEXTURAS DE TIENDA
Texture Madera;
Texture Palomitas;
Texture Soda;
Texture Cartel;

// MODELOS ALTAR


// MODELOS VOLADORES DE PAPANTLA


// MODELOS LÁMPARAS
Model Lampara1_M;



// MODELOS PERSONAJES
Model STienda_M;
Model Rakan;
Model Rakan_PI;
Model Teemo_M;
Model Braum_M;
Model Gnar_M;

Model MascotaAzul_M;

//TEXTURAS PERSONAJES
Texture STienda;
Texture CuerpoRakan;
Texture CapaRakan;
Texture Teemo;
Texture Braum;
Texture Gnar;




//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearCuboUnitario()
{
	// --- Definición de un Cubo Unitario (1x1x1) ---
	// *** NORMALES INVERTIDAS (apuntan hacia adentro) ***
	GLfloat cuboVertices[] = {
		// x, y, z, u, v, nx, ny, nz
		// Cara Frontal (Normal: 0,0,-1)
		-0.5f, -0.5f, 0.5f,	 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 0
		 0.5f, -0.5f, 0.5f,	 2.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 1
		 0.5f,  0.5f, 0.5f,	 2.0f, 2.0f,  0.0f, 0.0f, -1.0f, // 2
		-0.5f,  0.5f, 0.5f,	 0.0f, 2.0f,  0.0f, 0.0f, -1.0f, // 3

		// Cara Trasera (Normal: 0,0,1)
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 4
		 0.5f, -0.5f, -0.5f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 5
		 0.5f,  0.5f, -0.5f,  2.0f, 2.0f,  0.0f, 0.0f, 1.0f, // 6
		-0.5f,  0.5f, -0.5f,  0.0f, 2.0f,  0.0f, 0.0f, 1.0f, // 7

		// Cara Superior (Normal: 0,-1,0)
		-0.5f, 0.5f,  0.5f,	 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 8
		 0.5f, 0.5f,  0.5f,	 4.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 9
		 0.5f, 0.5f, -0.5f,	 4.0f, 4.0f,  0.0f, -1.0f, 0.0f, // 10
		-0.5f, 0.5f, -0.5f,	 0.0f, 4.0f,  0.0f, -1.0f, 0.0f, // 11

		// Cara Inferior (Normal: 0,1,0)
		-0.5f, -0.5f,  0.5f,	 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 12
		 0.5f, -0.5f,  0.5f,	 1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 13
		 0.5f, -0.5f, -0.5f,	 1.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 14
		-0.5f, -0.5f, -0.5f,	 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 15

		// Cara Izquierda (Normal: 1,0,0)
		-0.5f, -0.5f, -0.5f,	 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 16
		-0.5f, -0.5f,  0.5f,	 8.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 17
		-0.5f,  0.5f,  0.5f,	 8.0f, 2.0f,  1.0f, 0.0f, 0.0f, // 18
		-0.5f,  0.5f, -0.5f,	 0.0f, 2.0f,  1.0f, 0.0f, 0.0f, // 19

		// Cara Derecha (Normal: -1,0,0)
		 0.5f, -0.5f,  0.5f,	 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 20
		 0.5f, -0.5f, -0.5f,	 8.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 21
		 0.5f,  0.5f, -0.5f,	 8.0f, 2.0f,  -1.0f, 0.0f, 0.0f, // 22
		 0.5f,  0.5f,  0.5f,	 0.0f, 2.0f,  -1.0f, 0.0f, 0.0f  // 23
	};

	// Índices ESTÁNDAR (CCW)
	unsigned int cuboIndices[] = {
		0, 1, 2,   2, 3, 0,		// Frontal
		4, 5, 6,   6, 7, 4,		// Trasera
		8, 9, 10,  10, 11, 8,	// Superior
		12, 13, 14, 14, 15, 12,	// Inferior
		16, 17, 18, 18, 19, 16,	// Izquierda
		20, 21, 22, 22, 23, 20	// Derecha
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cuboVertices, cuboIndices, 192, 36);
	meshList.push_back(cubo); // Se añadirá en meshList[5]
}

void CrearBaseTruncada()
{
	// --- Definición de una Pirámide MUY Truncada ---
	// Base de 1x1 (en y=-0.5), Top de 0.9x0.9 (en y=0.5), Altura 1.0
	// Centrado en el origen (va de -0.5 a 0.5 en Y)

	// Vértices base (y = -0.5)
	glm::vec3 p0 = glm::vec3(-0.5f, -0.5f, 0.5f);  // B0 (frontal-izq)
	glm::vec3 p1 = glm::vec3(0.5f, -0.5f, 0.5f);   // B1 (frontal-der)
	glm::vec3 p2 = glm::vec3(0.5f, -0.5f, -0.5f);  // B2 (trasera-der)
	glm::vec3 p3 = glm::vec3(-0.5f, -0.5f, -0.5f); // B3 (trasera-izq)
	// Vértices top (y = 0.5)
	glm::vec3 p4 = glm::vec3(-0.45f, 0.5f, 0.45f); // T4 (frontal-izq)
	glm::vec3 p5 = glm::vec3(0.45f, 0.5f, 0.45f);  // T5 (frontal-der)
	glm::vec3 p6 = glm::vec3(0.45f, 0.5f, -0.45f); // T6 (trasera-der)
	glm::vec3 p7 = glm::vec3(-0.45f, 0.5f, -0.45f); // T7 (trasera-izq)

	// *** CÁLCULO DE NORMALES (Apunta Hacia Adentro) ***
	// Se invierte el orden del cross product para voltear la normal
	// Normal para cara frontal (p0, p1, p5, p4)
	glm::vec3 edgeF1 = p1 - p0;
	glm::vec3 edgeF2 = p4 - p0;
	glm::vec3 normalFront = glm::normalize(glm::cross(edgeF2, edgeF1)); // Invertido

	// Normal para cara trasera (p2, p3, p7, p6)
	glm::vec3 edgeB1 = p3 - p2;
	glm::vec3 edgeB2 = p6 - p2;
	glm::vec3 normalBack = glm::normalize(glm::cross(edgeB2, edgeB1)); // Invertido

	// Normal para cara izquierda (p3, p0, p4, p7)
	glm::vec3 edgeL1 = p0 - p3;
	glm::vec3 edgeL2 = p7 - p3;
	glm::vec3 normalLeft = glm::normalize(glm::cross(edgeL2, edgeL1)); // Invertido

	// Normal para cara derecha (p1, p2, p6, p5)
	glm::vec3 edgeR1 = p2 - p1;
	glm::vec3 edgeR2 = p5 - p1;
	glm::vec3 normalRight = glm::normalize(glm::cross(edgeR2, edgeR1)); // Invertido


	// *** VÉRTICES CON NORMALES INVERTIDAS ***
	GLfloat frustumVertices[] = {
		// x, y, z, u, v, nx, ny, nz
		// Caras definidos en sentido ANTI-HORARIO (CCW) visto desde FUERA

		// Cara Superior (Normal: 0,-1,0)
		p7.x, p7.y, p7.z,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // T7 (0)
		p6.x, p6.y, p6.z,  4.0f, 0.0f,  0.0f, -1.0f, 0.0f, // T6 (1)
		p5.x, p5.y, p5.z,  4.0f, 4.0f,  0.0f, -1.0f, 0.0f, // T5 (2)
		p4.x, p4.y, p4.z,  0.0f, 4.0f,  0.0f, -1.0f, 0.0f, // T4 (3)

		// Cara Inferior (Normal: 0,1,0)
		p3.x, p3.y, p3.z,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // B3 (4)
		p0.x, p0.y, p0.z,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // B0 (5)
		p1.x, p1.y, p1.z,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f, // B1 (6)
		p2.x, p2.y, p2.z,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, // B2 (7)

		// Cara Frontal (Normal calculada e invertida)
		p0.x, p0.y, p0.z,  0.0f, 0.0f,  normalFront.x, normalFront.y, normalFront.z, // B0 (8)
		p1.x, p1.y, p1.z,  2.0f, 0.0f,  normalFront.x, normalFront.y, normalFront.z, // B1 (9)
		p5.x, p5.y, p5.z,  2.0f, 1.0f,  normalFront.x, normalFront.y, normalFront.z, // T5 (10)
		p4.x, p4.y, p4.z,  0.0f, 1.0f,  normalFront.x, normalFront.y, normalFront.z, // T4 (11)

		// Cara Trasera (Normal calculada e invertida)
		p2.x, p2.y, p2.z,  0.0f, 0.0f,  normalBack.x, normalBack.y, normalBack.z, // B2 (12)
		p3.x, p3.y, p3.z,  2.0f, 0.0f,  normalBack.x, normalBack.y, normalBack.z, // B3 (13)
		p7.x, p7.y, p7.z,  2.0f, 1.0f,  normalBack.x, normalBack.y, normalBack.z, // T7 (14)
		p6.x, p6.y, p6.z,  0.0f, 1.0f,  normalBack.x, normalBack.y, normalBack.z, // T6 (15)

		// Cara Izquierda (Normal calculada e invertida)
		p3.x, p3.y, p3.z,  0.0f, 0.0f,  normalLeft.x, normalLeft.y, normalLeft.z, // B3 (16)
		p0.x, p0.y, p0.z,  5.0f, 0.0f,  normalLeft.x, normalLeft.y, normalLeft.z, // B0 (17)
		p4.x, p4.y, p4.z,  5.0f, 0.5f,  normalLeft.x, normalLeft.y, normalLeft.z, // T4 (18)
		p7.x, p7.y, p7.z,  0.0f, 0.5f,  normalLeft.x, normalLeft.y, normalLeft.z, // T19)

		// Cara Derecha (Normal calculada e invertida)
		p1.x, p1.y, p1.z,  0.0f, 0.0f,  normalRight.x, normalRight.y, normalRight.z, // B1 (20)
		p2.x, p2.y, p2.z,  5.0f, 0.0f,  normalRight.x, normalRight.y, normalRight.z, // B2 (21)
		p6.x, p6.y, p6.z,  5.0f, 0.5f,  normalRight.x, normalRight.y, normalRight.z, // T6 (22)
		p5.x, p5.y, p5.z,  0.0f, 0.5f,  normalRight.x, normalRight.y, normalRight.z  // T5 (23)
	};

	// *** ÍNDICES ESTÁNDAR (Winding order CCW - Anti-horario) ***
	unsigned int frustumIndices[] = {
		0, 1, 2,   2, 3, 0,		// Superior
		4, 5, 6,   6, 7, 4,		// Inferior
		8, 9, 10,  10, 11, 8,	// Frontal
		12, 13, 14,  14, 15, 12,	// Trasera
		16, 17, 18,  18, 19, 16,	// Izquierda
		20, 21, 22,  22, 23, 20	// Derecha
	};

	Mesh* base = new Mesh();
	base->CreateMesh(frustumVertices, frustumIndices, 192, 36);
	meshList.push_back(base); // Se añadirá en meshList[6]
}
///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXavion = 2.0, posYavion = 5.0, posZavion = -3.0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

#define MAX_FRAMES 100 //Número de cuadros máximos
int i_max_steps = 100; //Número de pasos entre cuadros para interpolación, a mayor número , más lento será el movimiento
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float giroAvion;		//Variable para GiroAvion
	float giroAvionInc;		//Variable para IncrementoGiroAvion
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;	// Luego se establece al leer keyframes.txt
bool play = false;
int playIndex = 0;

// Carga keyframes desde keyframes.txt y asigna el FrameIndex
bool loadFramesFromFile(const std::string& filename = "keyframes.txt")
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	int idx = 0;
	while (std::getline(file, line) && idx < MAX_FRAMES) {
		// Omitir líneas vacías o comentarios
		if (line.empty()) continue;
		if (line[0] == '#') continue;

		std::istringstream iss(line);
		float x, y, g;
		// Esperamos 3 floats por línea: movAvion_x movAvion_y giroAvion
		if (!(iss >> x >> y >> g)) {
			// línea malformada -> ignorar
			continue;
		}

		KeyFrame[idx].movAvion_x = x;
		KeyFrame[idx].movAvion_y = y;
		KeyFrame[idx].giroAvion = g;
		// inicializar incrementos a 0 por seguridad
		KeyFrame[idx].movAvion_xInc = 0.0f;
		KeyFrame[idx].movAvion_yInc = 0.0f;
		KeyFrame[idx].giroAvionInc = 0.0f;

		++idx;
	}

	FrameIndex = idx;
	file.close();
	return (idx > 0);
}

void saveFrame(void) //tecla L
{
	printf("frameindex %d\n", FrameIndex);

	// Guardar los valores en el archivo de texto
	std::ofstream file("keyframes.txt", std::ios::app); // Abre en modo append para no sobrescribir
	if (file.is_open()) {
		file << movAvion_x << " "
			<< movAvion_y << " "
			<< giroAvion << std::endl;
		file.close();
		printf("Frame guardado en keyframes.txt\n");
	}
	else {
		printf("No se pudo abrir el archivo para guardar el frame.\n");
	}

	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;
	FrameIndex++;
}


void resetElements(void) //Tecla 0
{
	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolación del próximo cuadro
			{

				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//Dibujar Animación
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCuboUnitario();
	CrearBaseTruncada();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	//TEXTURAS
	pisoTexture = Texture("Textures/piso.png");
	pisoTexture.LoadTextureA();
	//TEXTURA PIRAMIDE
	Piramide = Texture("Textures/pyramid_base_color.png");
	Piramide.LoadTextureA();

	//TEXTURAS JUEGO DE PELOTA
	Piedra = Texture("Textures/Textura_Piedra_1.jpg");
	Piedra.LoadTextureA();
	ParedPiedra = Texture("Textures/Piedra.jpg");
	ParedPiedra.LoadTextureA();

	//TEXTURAS RING
	Ring = Texture("Textures/Ring.png");
	Ring.LoadTextureA();
	Chairs = Texture("Textures/black.jpg");
	Chairs.LoadTextureA();

	//TEXTURAS TIENDA
	Madera = Texture("Textures/wood.jpg");
	Madera.LoadTextureA();
	Palomitas = Texture("Textures/popcorn_basecolor.png");
	Palomitas.LoadTextureA();
	STienda = Texture("Textures/lambert2.png");
	STienda.LoadTextureA();
	Soda = Texture("Textures/Soda_Col.png");
	Soda.LoadTextureA();
	Cartel = Texture("Textures/Cartel.png");
	Cartel.LoadTextureA();

	//TEXTURAS PERSONAJES
	CuerpoRakan = Texture("Textures/Rakan_cuerpo.png");
	CuerpoRakan.LoadTextureA();
	CapaRakan = Texture("Textures/Rakan_capa.png");
	CapaRakan.LoadTextureA();
	Teemo = Texture("Textures/Teemo.png");
	Teemo.LoadTextureA();
	Braum = Texture("Textures/Braum.png");	
	Braum.LoadTextureA();	
	Gnar = Texture("Textures/Gnar.png");	
	Gnar.LoadTextureA();


	// CARGA DE MODELOS
	//MODELOS PIRAMIDE
	Piramide_M = Model();
	Piramide_M.LoadModel("Models/piramide.fbx");
	//MODELOS JUEGO DE PELOTA
	Pelota_M = Model();
	Pelota_M.LoadModel("Models/pelota.obj");
	Pared_M = Model();
	Pared_M.LoadModel("Models/stone-wall.fbx");
	Aro_M = Model();
	Aro_M.LoadModel("Models/Aro_01.obj");

	//MODELOS RING
	Ring_M = Model();
	Ring_M.LoadModel("Models/Ring.obj");
	Chairs_M = Model();
	Chairs_M.LoadModel("Models/Chairs.fbx");
	
	//MODELOS TIENDA
	Palomitas_M = Model();
	Palomitas_M.LoadModel("Models/Popcorn.obj");
	Soda_M = Model();
	Soda_M.LoadModel("Models/Soda Cup.fbx");
	Cartel_M = Model();
	Cartel_M.LoadModel("Models/cartel.obj");
	
	
	

	// CARGA SKYBOX
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sp2_rt.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_lf.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_dn.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_up.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_bk.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_ft.png");

	skybox = Skybox(skyboxFaces);

	// MATERIALES
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	// LUCES
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 0.96f, 0.71f,
		0.5f, 0.4f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.03f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//CONSTANTES
		// Definición de Medidas de la tienda
	float counterW = 8.0f;  // Ancho del mostrador
	float counterH = 3.0f;  // Altura del mostrador
	float counterD = 2.5f;  // Profundidad del mostrador
	float postSize = 0.4f;  // Grosor de los postes
	float postH = 6.0f;   // Altura de los postes (sobre el mostrador)
	float roofH = 0.5f;   // Grosor del techo
	float roofOverhang = 1.0f; // Cuánto sobresale el techo
	float valanceH = 1.0f;  // Altura del faldón rayado
	
	//Postes de Soporte
		// Se posicionan en la parte TRASERA del mostrador
	float postX_Offset = (counterW / 2.0f) - (postSize / 2.0f);
	float postY_Pos = counterH + (postH / 2.0f);
	float postZ_Pos = -(counterD / 2.0f) + (postSize / 2.0f);
	
	//Techo y Faldón
	float roofW = counterW;
	float roofD = counterD + roofOverhang;
	float roofY_Pos = counterH + postH + (roofH / 2.0f);
	float roofZ_Pos = postZ_Pos - (roofOverhang / 2.0f) + (postSize / 2.0f);
	
	
	//Props sobre el Mostrador
	float propY_Base = counterH; // Los props se sientan sobre el mostrador
	
	
	float valanceY_Pos = roofY_Pos - (roofH / 2.0f) - (valanceH / 2.0f);
	float valanceZ_Pos = roofZ_Pos + (roofD / 2.0f); // Borde frontal del techo
	
	// Letrero 
	float signW = 0.1f;
	float signH = 0.18f;
		
	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 posPelota = glm::vec3(2.0f, 0.0f, 0.0f);

	//-------------KEYFRAMES GUARDADOS---------------//

	// Intentar cargar keyframes desde keyframes.txt; si falla, se usan valores por defecto que venían en la práctica
	if (!loadFramesFromFile("keyframes.txt"))
	{
		KeyFrame[0].movAvion_x = 0.0f;
		KeyFrame[0].movAvion_y = 0.0f;
		KeyFrame[0].giroAvion = 0;

		KeyFrame[1].movAvion_x = -2.0f;
		KeyFrame[1].movAvion_y = 4.0f;
		KeyFrame[1].giroAvion = 0;

		KeyFrame[2].movAvion_x = -4.0f;
		KeyFrame[2].movAvion_y = 0.0f;
		KeyFrame[2].giroAvion = 0;

		KeyFrame[3].movAvion_x = -6.0f;
		KeyFrame[3].movAvion_y = -4.0f;
		KeyFrame[3].giroAvion = 0;

		KeyFrame[4].movAvion_x = -8.0f;
		KeyFrame[4].movAvion_y = 0.0f;
		KeyFrame[4].giroAvion = 0.0f;

		KeyFrame[5].movAvion_x = -10.0f;
		KeyFrame[5].movAvion_y = 4.0f;
		KeyFrame[5].giroAvion = 0.0f;

		FrameIndex = 6;
		printf("No se encontró keyframes.txt. Usados frames por defecto (6)\n");
	}
	else {
		printf("Cargados %d frames desde keyframes.txt\n", FrameIndex);
	}


	printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
	printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X");


	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);



	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
		}
		rotllanta += rotllantaOffset * deltaTime;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();









		// INSTANCIAS DE MODELOS



	// CANCHA DE JUEGO DE PELOTA

		// BASES 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-120.0f, 0.6f, -150.0f));
		model = glm::scale(model, glm::vec3(40.0f, 3.0f, 140.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piedra.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-220.0f, 0.6f, -150.0f));
		model = glm::scale(model, glm::vec3(40.0f, 3.0f, 140.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piedra.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh(); 

		// Prismas cuadrangulares
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-120.0f, 17.1f, -150.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ParedPiedra.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-220.0f, 17.1f, -150.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ParedPiedra.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh(); 
		
		//parte de arriba
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-120.0f, 32.5f, -150.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piedra.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh(); 
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-220.0f, 32.5f, -150.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piedra.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		
	/*	// PARED
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-135.0f, 26.5f, -150.5f));
		modelaux = model;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pared_M.RenderModel(); */
		
		// AROS
		model = modelaux;
		model = glm::translate(model, glm::vec3(-139.9f, 25.5f, -150.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-200.1f, 25.5f, -150.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 1.0f, 0.0f });
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro_M.RenderModel();
		
		// PELOTA
		model = modelaux;
		model = glm::translate(model, glm::vec3(-22.0f, -28.5f, 8.0f));
		model = glm::scale(model, glm::vec3(0.22f, 0.22f, 0.22f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pelota_M.RenderModel();

		// PELOTA 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3 ( - 181.0f, 4.0f, -172.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pelota_M.RenderModel();





		// PIRÁMIDE
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 1.0f, 0.0f });
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piramide.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Piramide_M.RenderModel();


		// RING
		//El Ring
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-180.0f, -0.5f, 180.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Ring_M.RenderModel();
		
		//Sillas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-250.0f, -0.9f, 190.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(-90.0f), { 1.0f, 0.0f, 0.0f });
		model = glm::rotate(model, glm::radians(90.0f), { 0.0f, 0.0f, 1.0f });
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Chairs_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-170.0f, -0.9f, 250.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(-90.0f), { 1.0f, 0.0f, 0.0f });
		model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 0.0f, 1.0f });
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Chairs_M.RenderModel();
		
		
		//TIENDA DE BOTANAS
		
		//  Matriz base para toda la tienda 
			glm::mat4 modelStallBase = glm::mat4(1.0);
			modelStallBase = glm::translate(modelStallBase, glm::vec3(-110.0f, -0.9f, 180.0f));
			modelStallBase = glm::scale(modelStallBase, glm::vec3(1.5f, 1.5f, 1.5f));
			modelStallBase = glm::rotate(modelStallBase, glm::radians(90.0f), { 0.0f, 1.0f, 0.0f });
			//Renderizar el Mostrador 
			model = modelStallBase; // Empezar desde la base de la tienda
			model = glm::translate(model, glm::vec3(0.0f, counterH / 2.0f, 0.0f));
			model = glm::scale(model, glm::vec3(counterW, counterH, counterD));
		
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco para textura
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		
			Madera.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[5]->RenderMesh(); // Usa el Cubo Unitario
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		
			// Poste Izquierdo
			model = modelStallBase; // Resetear a la base de la tienda
			model = glm::translate(model, glm::vec3(-postX_Offset, postY_Pos, postZ_Pos));
			model = glm::scale(model, glm::vec3(postSize, postH, postSize));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[5]->RenderMesh();
		
			// Poste Derecho
			model = modelStallBase; // Resetear a la base de la tienda
			model = glm::translate(model, glm::vec3(postX_Offset, postY_Pos, postZ_Pos));
			model = glm::scale(model, glm::vec3(postSize, postH, postSize));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[5]->RenderMesh();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		
			// Techo (Plano superior)
			model = modelStallBase;
			model = glm::translate(model, glm::vec3(0.0f, roofY_Pos, roofZ_Pos));
			model = glm::scale(model, glm::vec3(roofW, roofH, roofD));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[5]->RenderMesh();
		
			// Faldón (El que cuelga al frente)
			model = modelStallBase;
			model = glm::translate(model, glm::vec3(0.0f, valanceY_Pos, valanceZ_Pos));
			model = glm::scale(model, glm::vec3(roofW, valanceH, 0.1f)); // Muy delgado
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[5]->RenderMesh();
			// Letrero
			model = modelStallBase;
			model = glm::translate(model, glm::vec3(0.0f, propY_Base-16.9f, 1.4f));
			model = glm::scale(model, glm::vec3(signW, signH, 0.1f));
			model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 1.0f, 0.0f });
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			Cartel.UseTexture();
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Cartel_M.RenderModel();
		
		
			// Cajas de Palomitas
			model = modelStallBase;
			model = glm::translate(model, glm::vec3(-1.0f, propY_Base, 0.0f));
			model = glm::scale(model, glm::vec3(0.3, 0.3,0.3));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Palomitas.UseTexture();
			Palomitas_M.RenderModel();
		
			//Refrescos
			model = modelStallBase;
			model = glm::translate(model, glm::vec3(0.6f, propY_Base, 0.0f));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
			model = glm::rotate(model, glm::radians(-90.0f), { 1.0f, 0.0f, 0.0f });
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Soda.UseTexture();
			Soda_M.RenderModel();
		
			model = modelStallBase;
			model = glm::translate(model, glm::vec3(-2.6f, propY_Base, 0.0f));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
			model = glm::rotate(model, glm::radians(-90.0f), { 1.0f, 0.0f, 0.0f });
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Soda.UseTexture();
			Soda_M.RenderModel();


		// ALTAR
		


		// VOLADORES

		
		//Teemo para volador de Papantla
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(180.0f, 30.f, 180.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 0.0f, 1.0f });
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Teemo.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Teemo_M.RenderModel();



		// LÁMPARAS
		// Lampara 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-110.0f, -2.0f, -50.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.6f, 1.8f, 1.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara1_M.RenderModel();
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 11.3f, 0.0f));
		glm::vec3 lamparaPosition = glm::vec3(model[3]);
		pointLights[0].SetPosP(lamparaPosition); // actualizar posición de la luz puntual


		// PERSONAJES

		//Rakan
		//la pierna solo puede rotar hasta 45 grados hacia atrás o si no se nota demasiado el corte en el modelo
		//pierna izquierda x=0, y=8.75346 m, z=103.16 m
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-180.0f, -0.9f, -170.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco para que vea la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CapaRakan.UseTexture();
		CuerpoRakan.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Rakan.RenderModel();
		
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CapaRakan.UseTexture();
		CuerpoRakan.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Rakan_PI.RenderModel();

		//Braum
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-180.0f, 10.0f, 190.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		color = glm::vec3(1.0f, 1.0f, 1.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Braum.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Braum_M.RenderModel();

		//MegaGnar
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-180.0f, 10.0f, 180.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco para que se vea la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gnar.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Gnar_M.RenderModel();

		
		// MASCOTA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-120.0f, -2.0f, 20.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MascotaAzul_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}


void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;

		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			reinicioFrame++;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
		}
	}

	// Movimiento X negativo (teclas 3 / 4)
	if (keys[GLFW_KEY_3])
	{
		if (ciclo_xneg < 1)
		{
			movAvion_x -= 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo_xneg++;
			ciclo_xneg2 = 0;
			printf("\n Presiona la tecla 4 para poder habilitar la variable X- nuevamente\n");
		}
	}
	if (keys[GLFW_KEY_4])
	{
		if (ciclo_xneg2 < 1)
		{
			ciclo_xneg = 0;
			ciclo_xneg2++;
			printf("\n Ya puedes modificar la variable X- presionando la tecla 3\n");
		}
	}

	// Movimiento Y positivo (teclas 5 / 6)
	if (keys[GLFW_KEY_5])
	{
		if (ciclo_ypos < 1)
		{
			movAvion_y += 1.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo_ypos++;
			ciclo_ypos2 = 0;
			printf("\n Presiona la tecla 6 para poder habilitar la variable Y+ nuevamente\n");
		}
	}
	if (keys[GLFW_KEY_6])
	{
		if (ciclo_ypos2 < 1)
		{
			ciclo_ypos = 0;
			ciclo_ypos2++;
			printf("\n Ya puedes modificar la variable Y+ presionando la tecla 5\n");
		}
	}

	// Movimiento Y negativo (teclas 7 / 8)
	if (keys[GLFW_KEY_7])
	{
		if (ciclo_yneg < 1)
		{
			movAvion_y -= 1.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo_yneg++;
			ciclo_yneg2 = 0;
			printf("\n Presiona la tecla 8 para poder habilitar la variable Y- nuevamente\n");
		}
	}
	if (keys[GLFW_KEY_8])
	{
		if (ciclo_yneg2 < 1)
		{
			ciclo_yneg = 0;
			ciclo_yneg2++;
			printf("\n Ya puedes modificar la variable Y- presionando la tecla 7\n");
		}
	}

	// Movimiento rotación Z negativo (teclas R / T)
	if (keys[GLFW_KEY_R])
	{
		if (ciclo_zrot < 1)
		{
			rotZ += 10.0f; // incrementa 10 grados
			if (rotZ >= 360.0f) rotZ -= 360.0f;
			printf("\n rotZ (grados) es: %f\n", rotZ);
			ciclo_zrot++;
			ciclo_zrot2 = 0;
			printf("\n Presiona T para habilitar la modificación de rotacion Z nuevamente\n");
		}
	}
	if (keys[GLFW_KEY_T])
	{
		if (ciclo_zrot2 < 1)
		{
			ciclo_zrot = 0;
			ciclo_zrot2++;
			printf("\n Ya puedes modificar la rotacion en Z presionando la tecla R\n");
		}
	}

}

