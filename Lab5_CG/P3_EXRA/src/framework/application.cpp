#include "application.h"
#include "utils.h"
#include "image.h"
#include "mesh.h"
#include "light.h"
#include "material.h"

Light* light = new Light();
Material* material = new Material();

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

	zbuffer.resize(w, h);
	zbuffer.fill(100000);
	framebuffer.resize(w, h);
}

//Here we have already GL working, so we can create meshes and textures
//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,10,20),Vector3(0,10,0),Vector3(0,1,0)); //define eye,center,up
	camera->perspective(60, window_width / (float)window_height, 0.1, 10000); //define fov,aspect,near,far
	light = new Light();
	material = new Material();
	//load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ("lee.obj") )
		std::cout << "FILE Lee.obj NOT FOUND" << std::endl;

	//load the texture
	texture = new Image();
	texture->loadTGA("color.tga");

	texture_normal = new Image();
	texture_normal->loadTGA("lee_normal.tga");

}

//this function fills the triangle by computing the bounding box of the triangle in screen space and using the barycentric interpolation
//to check which pixels are inside the triangle. It is slow for big triangles, but faster for small triangles 
//void Application::fillTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector2& uv0, const Vector2& uv1, const Vector2& uv2)
//{
	//compute triangle bounding box in screen space

	//clamp to screen area

	//loop all pixels inside bounding box
	// for()

		//we must compute the barycentrinc interpolation coefficients, weights of pixel P(x,y)

		//check if pixel is inside or outside the triangle

		//here add your code to test occlusions based on the Z of the vertices and the pixel (TASK 5)

		//here add your code to compute the color of the pixel (barycentric interpolation) (TASK 4)

		//draw the pixels in the colorbuffer x,y 
		//framebuffer.setPixel(x, y, COMPUTED COLOR);
//}

//render one frame
void Application::render(Image& framebuffer)
{
	framebuffer.fill(Color(40, 45, 60)); //clear
	zbuffer.fill(100000);
	//for every point of the mesh (to draw triangles take three points each time and connect the points between them (1,2,3,   4,5,6,   ...)
	for (int i = 0; i < mesh->vertices.size()-3; i+=3)
	{
		Vector3 vertex_1 = (mesh->vertices[i]); //extract vertex from mesh
		Vector3 vertex_2 = (mesh->vertices[i+1]);
		Vector3 vertex_3 = (mesh->vertices[i + 2]);

		Vector2 texcoord_1 = mesh->uvs[i]; //texture coordinate of the vertex (they are normalized, from 0,0 to 1,1)
		Vector2 texcoord_2 = mesh->uvs[i+1];
		Vector2 texcoord_3 = mesh->uvs[i+2];


		Vector3 norm1 = camera->projectVector(vertex_1);
		Vector3 norm2 = camera->projectVector(vertex_2);
		Vector3 norm3 = camera->projectVector(vertex_3);
		
		Vector2 tex1 = Vector2(texcoord_1.x, texcoord_1.y);
		Vector2 tex2 = Vector2(texcoord_2.x, texcoord_2.y);
		Vector2 tex3 = Vector2(texcoord_3.x, texcoord_3.y);

		//project every point in the mesh to normalized coordinates using the viewprojection_matrix inside camera
		//Vector3 normalized_point = camera->projectVector( vertex );

		//convert from normalized (-1 to +1) to framebuffer coordinates (0,W)
		Vector3 res1 = Vector3(framebuffer.width / 2 * norm1.x + framebuffer.width / 2, framebuffer.height / 2 * norm1.y + framebuffer.height / 2, norm1.z);
		Vector3 res2 = Vector3(framebuffer.width / 2 * norm2.x + framebuffer.width / 2, framebuffer.height / 2 * norm2.y + framebuffer.height / 2, norm2.z);
		Vector3 res3 = Vector3(framebuffer.width / 2 * norm3.x + framebuffer.width / 2,  framebuffer.height / 2 * norm3.y + framebuffer.height / 2, norm3.z );

		
		
		
		//paint points in framebuffer (using your drawTriangle function or the fillTriangle function)
		//framebuffer.drawTriangleInterpolated(res1.x, res1.y, res2.x, res2.y, res3.x, res3.y, Color::RED, Color::BLUE, Color::GREEN);

		if (mode == 1) {
			framebuffer.drawTriangle(res1.x, res1.y, res2.x, res2.y, res3.x, res3.y, Color::WHITE, false);
		}
		if (mode == 2) {
			framebuffer.drawTriangleInterpolated_color(res1.x, res1.y, res2.x, res2.y, res3.x, res3.y, res1.z, res2.z, res2.z, &zbuffer, Color::RED, Color::BLUE, Color::GREEN);
		}
		if (mode == 3) {
			framebuffer.drawTriangleInterpolated(res1.x, res1.y, res2.x, res2.y, res3.x, res3.y, res1.z, res2.z, res3.z, &zbuffer, tex1, tex2, tex3, texture);
		}
		if (mode == 4) {
			framebuffer.PhongIlluminationTexture(res1.x, res1.y, res2.x, res2.y, res3.x, res3.y, res1.z, res2.z, res3.z, &zbuffer, material, light, camera->eye, tex1, tex2, tex3, texture, texture_normal);
		}
		//framebuffer.drawTriangleInterpolated(res1.x, res1.y, res2.x, res2.y, res3.x, res3.y, res1.z, res2.z, res3.z, &zbuffer, tex1, tex2, tex3, texture);
	}
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (keystate[SDL_SCANCODE_SPACE])
	{
		//...
	}
	
	if (keystate[SDL_SCANCODE_LEFT])
		camera->eye.x -= 5 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye.x += 5 * seconds_elapsed;

	if (keystate[SDL_SCANCODE_UP])
		camera->eye.y -= 5 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_DOWN])
		camera->eye.y += 5 * seconds_elapsed;


	if (keystate[SDL_SCANCODE_W])
		if (camera->center.y > -20){
			camera->center.y -= 5 * seconds_elapsed;
		}
		
	if (keystate[SDL_SCANCODE_S])
		if (camera->center.y < 20) {
			camera->center.y += 5 * seconds_elapsed;
		}
	if (keystate[SDL_SCANCODE_A])
		if (camera->center.x < 40) {
			camera->center.x += 5 * seconds_elapsed;
		}
	if (keystate[SDL_SCANCODE_D])
		if (camera->center.x > -40) {
			camera->center.x -= 5 * seconds_elapsed;
		}
	if (keystate[SDL_SCANCODE_F])
		camera->fov += 5 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_G])
		camera->fov -= 5 * seconds_elapsed;

	//if we modify the camera fields, then update matrices
	camera->updateViewMatrix();
	camera->updateProjectionMatrix();
	
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
		case SDL_SCANCODE_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDL_SCANCODE_1: mode = 1; break;
		case SDL_SCANCODE_2: mode = 2; break;
		case SDL_SCANCODE_3: mode = 3; break;
		case SDL_SCANCODE_4: mode = 4; break;
		
	}
}

//keyboard released event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
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
 