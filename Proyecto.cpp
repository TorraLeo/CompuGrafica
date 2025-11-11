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

// MODELOS PIRÁMIDE


// MODELOS RING


// MODELOS ALTAR


// MODELOS VOLADORES DE PAPANTLA


// MODELOS LÁMPARAS
Model Lampara1_M;



// MODELOS PERSONAJES
Model MascotaAzul_M;







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
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);


	pisoTexture = Texture("Textures/piso.png");
	pisoTexture.LoadTextureA();


	// CARGA DE MODELOS
	Pelota_M = Model();
	Pelota_M.LoadModel("Models/pelota.obj");
	Pared_M = Model();
	Pared_M.LoadModel("Models/stone-wall.fbx");
	Aro_M = Model();
	Aro_M.LoadModel("Models/Aro_01.obj");

	MascotaAzul_M = Model();
	MascotaAzul_M.LoadModel("Models/MascotaAzul.obj");


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

		// PARED
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-135.0f, 26.5f, -150.5f));
		modelaux = model;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pared_M.RenderModel();
		// ARO
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, 18.0f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aro_M.RenderModel();
		// PELOTA
		model = modelaux;
		model = glm::translate(model, glm::vec3(-22.0f, -28.5f, 8.0f));
		model = glm::scale(model, glm::vec3(0.22f, 0.22f, 0.22f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pelota_M.RenderModel();





		// PIRÁMIDE
		


		// RING
		


		// ALTAR
		


		// VOLADORES




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
