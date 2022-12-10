#include "application.h"
#include "utils.h"
#include "image.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "light.h"
#include "material.h"


Camera* camera = NULL;

Mesh* mesh = NULL;

Matrix44 model_matrix;

Shader* shader = NULL;

Shader* shader_phong_1 = NULL;
Shader* shader_phong_2 = NULL;
Shader* shader_phong_3 = NULL;
Shader* shader_phong_4 = NULL;
Texture* texture = NULL;
Texture* normal_text = NULL;

Light* light = new Light();

Vector3 ambient_light(0.1, 0.2, 0.3);
Material* material = new Material();

float angle = 0;
Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);
} 

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
	mesh->loadOBJ("../res/meshes/lee.obj");

	//load the texture
	texture = new Texture();
	normal_text = new Texture();
	if(!texture->load("../res/textures/lee_color_specular.tga") || !normal_text->load("../res/textures/lee_normal.tga"))
	{
		std::cout << "Texture not found" << std::endl;
		exit(1);
	}
	 
	//we load a shader
	shader = Shader::Get("../res/shaders/phong.vs","../res/shaders/phong.fs");
	shader_phong_1 = Shader::Get("../res/shaders/phong.vs", "../res/shaders/phong.fs");
	shader_phong_2 = Shader::Get("../res/shaders/phong_2.vs", "../res/shaders/phong_2.fs");
	shader_phong_3 = Shader::Get("../res/shaders/phong_3.vs", "../res/shaders/phong_3.fs");


	mode = 0;
	//load whatever you need here
	//......
	add_models();
}

//render one frame
void Application::render(void)
{
	// Clear the window and the depth buffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
		if (mode == 1 || mode == 2) {
			if (mode == 1) {
				shader = shader_phong_1;
			}
			else if (mode == 2) {
				shader = shader_phong_2;
			}
			

			//Get the viewprojection
			camera->aspect = window_width / window_height;
			Matrix44 viewprojection = camera->getViewProjectionMatrix();

			//enable the shader
			shader->enable();
			shader->setMatrix44("model", model_matrix); //upload info to the shader
			shader->setMatrix44("viewprojection", viewprojection); //upload info to the shader

			shader->setTexture("color_texture", texture, 0); //set texture in slot 0

			shader->setUniform3("camera_position", camera->eye);
			shader->setUniform3("ambient_light", ambient_light);

			shader->setUniform1("material_shininess", material->shininess);
			shader->setUniform3("material_ambient",material->ambient);
			shader->setUniform3("material_diffuse" ,material->diffuse);
			shader->setUniform3("material_specular", material->specular);

			shader->setUniform3("light_position", light->position);
			shader->setUniform3("light_diffuse", light->diffuse_color);
			shader->setUniform3("light_specular", light->specular_color);

			//render the data
			mesh->render(GL_TRIANGLES);

			//disable shader
			shader->disable();

			//swap between front buffer and back buffer
			SDL_GL_SwapWindow(this->window);
		}
		else if (mode == 3 ) {
			shader = shader_phong_3;


			// Clear the window and the depth buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			//Get the viewprojection 
			camera->aspect = window_width / window_height;
			Matrix44 viewprojection = camera->getViewProjectionMatrix();

			//enable the shader
			shader->enable();
			shader->setMatrix44("model", model_matrix); //upload info to the shader
			shader->setMatrix44("viewprojection", viewprojection); //upload info to the shader

			shader->setTexture("color_texture", texture, 0); //set texture in slot 0
			shader->setTexture("normal_texture", normal_text, 1); //set texture in slot 1
			shader->setUniform3("camera_position", camera->eye);
			shader->setUniform3("ambient_light", ambient_light);

			shader->setUniform1("material_shininess", material->shininess);
			shader->setUniform3("material_ambient", material->ambient);
			shader->setUniform3("material_diffuse", material->diffuse);
			shader->setUniform3("material_specular", material->specular);


			shader->setUniform3("light_position", light->position);
			shader->setUniform3("light_diffuse", light->diffuse_color);
			shader->setUniform3("light_specular", light->specular_color);

			//render the data
			mesh->render(GL_TRIANGLES);

			//disable shader
			shader->disable();

			//swap between front buffer and back buffer
			SDL_GL_SwapWindow(this->window);
		}
		else if (mode == 4) {
			shader = shader_phong_3;
			// Clear the window and the depth buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			//Get the viewprojection 
			camera->aspect = window_width / window_height;
			Matrix44 viewprojection = camera->getViewProjectionMatrix();
			for (int i = 0; i < models.size(); ++i) {
				//enable the shader
				shader->enable();
				shader->setMatrix44("model", models[i].model); //upload info to the shader
				shader->setMatrix44("viewprojection", viewprojection); //upload info to the shader

				shader->setTexture("color_texture", texture, 0); //set texture in slot 0
				shader->setTexture("normal_texture", normal_text, 1); //set texture in slot 1
				shader->setUniform3("camera_position", camera->eye);
				shader->setUniform3("ambient_light", ambient_light);

				shader->setUniform1("material_shininess", models[i].material->shininess);
				shader->setUniform3("material_ambient", models[i].material->ambient);
				shader->setUniform3("material_diffuse", models[i].material->diffuse);
				shader->setUniform3("material_specular", models[i].material->specular);


				shader->setUniform3("light_position", light->position);
				shader->setUniform3("light_diffuse", light->diffuse_color);
				shader->setUniform3("light_specular", light->specular_color);

				//render the data
				mesh->render(GL_TRIANGLES);

				//disable shader
				shader->disable();
			}

			//swap between front buffer and back buffer
			SDL_GL_SwapWindow(this->window);
		}
	
}

//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
	{
		if (mode == 4) {
			for (int i = 0; i < models.size(); ++i) {
				models[i].model.rotateLocal(seconds_elapsed*10, Vector3(0, 1, 0));
			}
		}
		model_matrix.rotateLocal(seconds_elapsed,Vector3(0,1,0));
	}

	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_LEFT])
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_DOWN])
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_F])
		camera->fov += 5 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_G])
		camera->fov -= 5 * seconds_elapsed;
}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode){
		case SDL_SCANCODE_R: Shader::ReloadAll(); break;
        case SDL_SCANCODE_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDL_SCANCODE_1: mode = 1; break;
		case SDL_SCANCODE_2: mode = 2; break;
		case SDL_SCANCODE_3: mode = 3; break;
		case SDL_SCANCODE_4: mode = 4; break;
	}
	if (keystate[SDL_SCANCODE_M]) {

		num_models++;
		add_models();
	}
	if (keystate[SDL_SCANCODE_N]) {
		delete_models();
	}

}

void Application::add_models() {
	if (models.size() == 0) {
		Matrix44 model_matrix;
		Material* material = new Material();
		model_matrix.setIdentity();
		model_matrix.translate(0 + std::pow(-1, num_models)*num_models * 10, 0, num_models * 10); //example of translation
		model_matrix.rotate(angle, Vector3(0, 1, 0));

		Model model = Model();
		model.material = material;
		model.model = model_matrix;

		models.push_back(model);

	}
	else {
		Matrix44 model_matrix;
		Material* material = new Material();
		model_matrix.setIdentity();
		model_matrix.translate(0 + std::pow(-1, num_models) * num_models * 10,0,  -num_models * 10);
		model_matrix.rotate(angle, Vector3(0, 1, 0));

		material->ambient = Vector3(randomValue(), randomValue(), randomValue());
		material->diffuse = Vector3(randomValue(), randomValue(), randomValue());
		material->specular = Vector3(randomValue(), randomValue(), randomValue());
		material->shininess = std::rand() % 1000;

		Model model = Model();
		model.material = material;
		model.model = model_matrix;


		models.push_back(model);

	}

}
void Application::delete_models() {
	if (models.size() > 1) {
		models.pop_back();
		num_models--;
	}
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{

	}
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
