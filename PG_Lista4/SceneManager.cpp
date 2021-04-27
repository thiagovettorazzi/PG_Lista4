#include "SceneManager.h"

static bool keys[1024];
static bool resized;
static GLuint width, height;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::initialize(GLuint w, GLuint h)
{
	width = w;
	height = h;
	
	initializeGraphics();
}

void SceneManager::initializeGraphics()
{
	glfwInit();

	window = glfwCreateWindow(width, height, "Cenario Jogo Silvio Santos", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	glfwSetWindowSizeCallback(window, resize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	addShader("../shaders/transformations.vs", "../shaders/transformations.frag");

	setupScene();

	resized = true;
}

void SceneManager::addShader(string vFilename, string fFilename)
{
	shader = new Shader (vFilename.c_str(), fFilename.c_str());
}


void SceneManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void SceneManager::resize(GLFWwindow * window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	glViewport(0, 0, width, height);
}


void SceneManager::update()
{
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void SceneManager::render()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (resized)
	{
		setupCamera2D();
		resized = false;
	}

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->update();
		objects[i]->draw();
	}
}

void SceneManager::run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		update();

		render();
		
		glfwSwapBuffers(window);
	}
}

void SceneManager::finish()
{
	glfwTerminate();
}


void SceneManager::setupScene()
{
	// Adicionando o background
	Sprite* obj = new Sprite;
	obj->setPosition(glm::vec3(1050.0f, 450.0f, 0.0));
	obj->setDimension(glm::vec3(2100.0f, 900.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj);

	// Adicionando aviões
	obj = new Sprite;
	obj->setPosition(glm::vec3(1600.0f, 700.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 90.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj);

	obj = new Sprite;
	obj->setPosition(glm::vec3(1100.0f, 400.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 90.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj);

	obj = new Sprite;
	obj->setPosition(glm::vec3(1300.0f, 200.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 90.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj);

	//Adicionando a mão
	obj = new Sprite;
	obj->setPosition(glm::vec3(180.0f, 440.0f, 0.0));
	obj->setDimension(glm::vec3(360.0f, 316.0f, 1.0f));
	obj->setShader(shader);
	objects.push_back(obj);

	unsigned int texID = loadTexture("../textures/background1.jpg");
	objects[0]->setTexture(texID);

	texID = loadTexture("../textures/aviaozinho.png");
	objects[1]->setTexture(texID);
	objects[2]->setTexture(texID);
	objects[3]->setTexture(texID);

	texID = loadTexture("../textures/mao.png");
	objects[4]->setTexture(texID);

	//Definindo a janela do mundo (ortho2D)
	ortho2D[0] = 0.0f;
	ortho2D[1] = 2100.0f;
	ortho2D[2] = 0.0f;
	ortho2D[3] = 900.0f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void SceneManager::setupCamera2D()
{
	float zNear = -1.0, zFar = 1.0;

	projection = glm::ortho(ortho2D[0], ortho2D[1], ortho2D[2], ortho2D[3], zNear, zFar);

	GLint projLoc = glGetUniformLocation(shader->ID, "projection");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

unsigned int SceneManager::loadTexture(string filename)
{
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	
	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	return texture;
}
