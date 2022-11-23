/*---------------------------------------------------------*/
/* ------------ PROYECTO_FINAL_TEORIA   -----------*/
/*-----------------    2023-1   ---------------------------*/
/*------------- Alumno: Tapia Escobar José Alejandro---------------*/
/*------------- No. Cuenta: 418046832 ---------------*/
#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 70.0f, 130.0f));//0,10,90 vista de la camara principal
float MovementSpeed = 3.5f;//.1
float lastX = SCR_WIDTH / 8.0f;//2
float lastY = SCR_HEIGHT / 8.0f;//2
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

//Lighting					  4		-10
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f); //Fuente de luz posicional, direccion de donde debe de salir 
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f); //Posicion desde donde esta el sol 

// posiciones

float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 0.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
giroMonito = 0.0f;
float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
rotInc = 0.0f,
giroMonitoInc = 0.0f;

//Colores declarados 
float colorR = 2.4f,
colorG = 0.86f,
colorB = 1.5f;

float rotx = (-20.0f, 0.0f, 0.0f);
float roty = (0.0f, 40.0f, 0.0f);
float movy = 0.0f;

float rot = 0.0f;//Se agrega
bool anim = false;//Se agrega-----------------------------------------------
bool anim2 = false;//SEGUNDO ANIM
bool anim3 = false;


float t = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

void animate(void)
{
	if (anim)
	{
		if (rot < 45.0f)
			rot += 0.2f;
		else {
			anim = false;
			anim2 = true;
		}
	}

	if (anim2)
	{
		if (rot > -45.0f)
			rot -= 0.2f;
		else {
			anim2 = false;
			anim = true;
		}
	}



	rotx = 500.0f * cos(t);
	roty = 300.0f * -sin(t);

	//lightDirection.x = 300.2f * sin(t);
	//lightDirection.z = 100.2f * sin(t);

	t += 0.05f;

	

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion)
	{
		movAuto_z += 3.0f;
	}
}

void getResolution()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	
	// ------------------------------
	glfwInit();
	

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// Aqui cargamos el skybox
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// Aqui cargamos los modelos
	Model piso("resources/objects/Piso/piso.obj");
	Model tanque("resources/objects/Edificio/tanque.obj");
	Model tanque2("resources/objects/Edificio/tanque2.obj");
	Model tanque3("resources/objects/Edificio/tanque3.obj");
	Model tanque4("resources/objects/Edificio/tanque4.obj");
	Model pecera("resources/objects/Edificio/pecera.obj");
	Model colafish("resources/objects/Peses/colafish.obj");
	Model bodyfish("resources/objects/Peses/bodyfish.obj");
	Model pez2("resources/objects/Peses/pez2.obj");
	Model tortu("resources/objects/Peses/tortu.obj");


	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);//fuente de luz direccional (simula el sol) configuramos la direccion en la que tiene que ir dirigida la luz izq der arriba o abajo
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));//caras menos iluminadas
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));//caras mas iluminadas (color blanco)
		staticShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));//reflejos vidrios de color (brillo de color espejos) (casa y carro)

		//lightPosition FUENTE DE LUZ QUE CAMBIA DE COLOR  (YA FUNCIONA)
		staticShader.setVec3("pointLight[0].position", lightPosition);//foco que emite la luz en todas las direcciones
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));//aqui tenia 0.9 (Creamos las variables de color)
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setFloat("pointLight[0].constant", 1.08f);//potencia de intensidad menor
		staticShader.setFloat("pointLight[0].linear", 0.009f);//potencia de intensidad mayor
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);//Aumento de distancia de los rallos de luz 

		//lightPosition QUE ENCIENDE Y APAGA CON TECLA (YA FUNCIONA)
		staticShader.setVec3("pointLight[1].position", glm::vec3(0.5f, 0.5f, 0.5f));//Aqui tenia 180 y 40
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.5f, 0.5f, 0.5f));//aqui .8 y .8
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));//aqui .9 en Y
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		//lightPosition simulacion de SOL movimiento de parabola 
		staticShader.setVec3("pointLight[2].position", glm::vec3(0.3f, 0.3f, 0.3f));
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.3f, 0.3f, 0.3f));
		staticShader.setFloat("pointLight[2].constant", 1.0f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.00032f);

		staticShader.setVec3("spotLight.position", glm::vec3(0.0f, 0.0f, 10.0f));
		staticShader.setVec3("spotLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight.constant", 1.0f);
		staticShader.setFloat("spotLight.linear", 0.0009f);
		staticShader.setFloat("spotLight.quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.2f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		//animacionPersonaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------

		model = glm::translate(glm::mat4(1.0f), glm::vec3(40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		//ninja.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Proyecto
		
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);
		//------------------------------------------------------------

		//-----------------PROPUESTA_PISO----------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		//----------------PROPUESTA_TANQUE--------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-220.0f, -3.0f, 200.0f));
		model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f,15.0f,30.0f));
		staticShader.setMat4("model", model);
		tanque.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-200.0f, 14.0f, 250.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		staticShader.setMat4("model", model);
		colafish.Draw(staticShader);//falta hacer que se mueva la cola del pez 

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-200.0f, 14.0f, 250.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		staticShader.setMat4("model", model);
		bodyfish.Draw(staticShader);//falta hacer que se mueva la cola del pez 


		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-210.0f, 14.0f, 170.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		staticShader.setMat4("model", model);
		pez2.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-230.0f, 14.0f, 210.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		staticShader.setMat4("model", model);
		tortu.Draw(staticShader);

		//------------------------------------------------------------------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-180.0f, 25.0f, -230.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 15.0f, 20.0f));
		staticShader.setMat4("model", model);
		tanque2.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-10.0f, 25.0f, -190.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 15.0f, 20.0f));
		staticShader.setMat4("model", model);
		tanque3.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 15.0f, 20.0f));
		staticShader.setMat4("model", model);
		tanque4.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(130.0f, 10.0f, 240.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		staticShader.setMat4("model", model);
		pecera.Draw(staticShader);

		//---------------PROPUESTA_PEZ-----------------------------

		
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;

	//TECLAS PARA QUE CAMBIE DE COLOR
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		colorB++;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		colorG++;
	//TECLAS PARA QUE ENCIENDA DE UN SOLO COLOR Y SE APAGUE 
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		colorR += 5.0f;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		colorR = 0.0f;
	//TECLAS DE MOVIMIENTO DONDE SOLO ENCIENDE Y APAGA UN SOLO COLOR 
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)//Aqui movemos la luz en el eje Y
		movy++;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		movy--;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}