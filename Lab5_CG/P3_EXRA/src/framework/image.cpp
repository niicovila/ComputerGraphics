#include "image.h"
#include "light.h"
#include "material.h"

Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

//copy constructor
Image::Image(const Image& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
}

//assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height*sizeof(Color)];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}



//change image size (the old one will remain in the top-left corner)
void Image::resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = getPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

//change image size and scale the content
void Image::scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = getPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++x)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.setPixel( x, y, getPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::flipX()
{
	for(unsigned int x = 0; x < width * 0.5; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			Color temp = getPixel(width - x - 1, y);
			setPixel( width - x - 1, y, getPixel(x,y));
			setPixel( x, y, temp );
		}
}

void Image::flipY()
{
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height * 0.5; ++y)
		{
			Color temp = getPixel(x, height - y - 1);
			setPixel( x, height - y - 1, getPixel(x,y) );
			setPixel( x, y, temp );
		}
}


//Loads an image from a TGA file
bool Image::loadTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesPerPixel;
	unsigned int imageSize;

	FILE * file = fopen(filename, "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << filename << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "TGA file seems to have errors or it is compressed, only uncompressed TGAs supported" << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	//save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	//convert to float all pixels
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			this->setPixel(x , height - y - 1, Color( tgainfo->data[pos+2], tgainfo->data[pos+1], tgainfo->data[pos]) );
		}

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::saveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	FILE *file = fopen(filename, "wb");
	if ( file == NULL )
	{
		fclose(file);
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	//tgainfo->width = header[1] * 256 + header[0];
	//tgainfo->height = header[3] * 256 + header[2];

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	//convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[(height-y-1)*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);
	return true;
}


FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width*height];
	memset(pixels, 0, width * height * sizeof(float));
}

//copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
}

//assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height * sizeof(float)];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}


//change image size (the old one will remain in the top-left corner)
void FloatImage::resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = getPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}


#ifndef IGNORE_LAMBDAS

//you can apply and algorithm for two images and store the result in the first one
//forEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void forEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

void Image::init_table() {

	table.resize(this->height);

	for (int i = 0; i < height; i++) {
		table[i].minx = 100000;
		table[i].maxx = -100000;
	}
}
void Image::breshenhamTable(int x0, int y0, int x1, int y1, Color c, std::vector<sCelda>& table) {
	int dx, dy, inc_E, inc_NE, d, x, y;

	int y_increment = 1;

	if (x0 > x1) {
		int aux = x0;
		x0 = x1;
		x1 = aux;

		aux = y0;

		y0 = y1;
		y1 = aux;
	}
	x = x0;
	y = y0;

	dx = x1 - x0;
	dy = y1 - y0;


	if (dy < 0) {
		y_increment = -1;
		dy = -dy;
	}
	//Controlamos que los valores de la tabla se actualizen correctamente para cualquier x e y
	if (y >= 0 && y < height) {
		if (x < 0) {
			table[y].minx = std::min(table[y].minx, 0);
			table[y].maxx = std::max(table[y].maxx, x);
		}
		else if (x >= width) {
			int a = width - 1;
			table[y].minx = std::min(table[y].minx, x);
			table[y].maxx = std::max(table[y].maxx, a);
		}
		else {
			table[y].minx = std::min(table[y].minx, x);
			table[y].maxx = std::max(table[y].maxx, x);
		}
	}

	if (dx > dy) {
		inc_E = 2 * dy;
		inc_NE = 2 * (dy - dx);
		d = 2 * dy - dx;

		while (x < x1) {

			if (d <= 0) {
				d += inc_E;
				x += 1;
			}
			else {
				d += inc_NE;
				y += y_increment;
				x += 1;
			}
			//setPixel(x, y, c);
		
			if (y >= 0 && y < height) {
				if (x < 0) {
					table[y].minx = std::min(table[y].minx, 0);
					table[y].maxx = std::max(table[y].maxx, x);
				}
				else if (x >= width) {
					int a = width - 1;
					table[y].minx = std::min(table[y].minx, x);
					table[y].maxx = std::max(table[y].maxx, a);
				}
				else {
					table[y].minx = std::min(table[y].minx, x);
					table[y].maxx = std::max(table[y].maxx, x);
				}
			}
			
		}
	}
	else {

		inc_E = 2 * dx;
		inc_NE = 2 * (dx - dy);
		d = 2 * dx - dy;
		while (y != y1) { // A veces aumenta y a veces disminuye, por eso ponemos != para que cuando llegue pare(no < o > ya que depende de cada caso)
			if (d <= 0) {
				d += inc_E;
				y += y_increment;
			}
			else {
				d += inc_NE;
				x += 1;
				y += y_increment;
			}
			//setPixel(x, y, c);
			if (y >= 0 && y < height) {
				if (x < 0) {
					table[y].minx = std::min(table[y].minx, 0);
					table[y].maxx = std::max(table[y].maxx, x);
				}
				else if (x >= width) {
					int a = width - 1;
					table[y].minx = std::min(table[y].minx, x);
					table[y].maxx = std::max(table[y].maxx, a);
				}
				else {
					table[y].minx = std::min(table[y].minx, x);
					table[y].maxx = std::max(table[y].maxx, x);
				}
			}
			
		}
	}
}

void Image::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c, bool fill) {
	init_table();

	if (fill) {
		breshenhamTable(x0, y0, x1, y1, c, table);
		breshenhamTable(x1, y1, x2, y2, c, table);
		breshenhamTable(x2, y2, x0, y0, c, table);
		for (int i = 0; i < height; i++) {
			if (table[i].maxx >= table[i].minx) {
				for (int j = table[i].minx; j <= table[i].maxx; j++) {
					setPixel(j, i, c);
				}
			}
		}
	}
	else {
		drawLineBresenham(x0, y0, x1, y1, c);
		drawLineBresenham(x1, y1, x2, y2, c);
		drawLineBresenham(x2, y2, x0, y0, c);
	}

}
void Image::drawTriangleInterpolated_color(int x0, int y0, int x1, int y1, int x2, int y2, float z0, float z1, float z2, FloatImage* zbuffer, Color c0, Color c1, Color c2) {

	init_table();

	breshenhamTable(x0, y0, x1, y1, Color::BLUE, table);
	breshenhamTable(x1, y1, x2, y2, Color::RED, table);
	breshenhamTable(x2, y2, x0, y0, Color::GREEN, table);


	Vector2 v0 = Vector2(x1 - x0, y1 - y0);
	Vector2 v1 = Vector2(x2 - x0, y2 - y0);


	for (int i = 0; i < height; i++) {
		if (table[i].maxx >= table[i].minx) {
			for (int j = table[i].minx; j <= table[i].maxx; j++) { 

				Vector2 v2 = Vector2(j - x0, i - y0);
				float d00 = v0.dot(v0);
				float d01 = v0.dot(v1);
				float d11 = v1.dot(v1);
				float d20 = v2.dot(v0);
				float d21 = v2.dot(v1);

				float denom = d00 * d11 - d01 * d01;
				float v = (d11 * d20 - d01 * d21) / denom;
				float w = (d00 * d21 - d01 * d20) / denom;
				float u = 1.0 - v - w;
				float z = z0 * u + z1 * v + z2 * w;
				float z_2 = getPixel_zbuffer(j, i, zbuffer);

				if (v >= 0 && v <= 1.0 && u >= 0 && u <= 1 && w >= 0 && w <= 1) {
					if (z < z_2) {
						setPixel_zbuffer(j, i, z, zbuffer);
						
						Color c = c0 * u + c1 * v + c2 * w;;

						paint_pixel(j, i, c);

					}

				}

			}
		}
	}
}

void Image::drawTriangleInterpolated(int x0, int y0, int x1, int y1, int x2, int y2, float z0, float z1, float z2, FloatImage* zbuffer, Vector2 tex1, Vector2 tex2, Vector2 tex3, Image* texture) {

	init_table();

	breshenhamTable(x0, y0, x1, y1, Color::BLUE, table);
	breshenhamTable(x1, y1, x2, y2, Color::RED, table);
	breshenhamTable(x2, y2, x0, y0, Color::GREEN, table);


	Vector2 v0 = Vector2(x1 - x0, y1 - y0);
	Vector2 v1 = Vector2(x2 - x0, y2 - y0);


	float d00 = v0.dot(v0);
	float d01 = v0.dot(v1);
	float d11 = v1.dot(v1);




	for (int i = 0; i < height; i++) {
		if (table[i].maxx >= table[i].minx) {
			for (int j = table[i].minx; j <= table[i].maxx; j++) {

				Vector2 v2 = Vector2(j - x0, i - y0);
				float d20 = v2.dot(v0);
				float d21 = v2.dot(v1);

				float denom = d00 * d11 - d01 * d01;

				float v = (d11 * d20 - d01 * d21) / denom;
				float w = (d00 * d21 - d01 * d20) / denom;
				float u = 1.0 - v - w;

				float z = z0 * u + z1 * v + z2 * w;
				float z_2 = getPixel_zbuffer(j, i, zbuffer);

				if (v >= -0.05 && v <= 1.05 && u >= -0.05 && u <= 1.05 && w >= -0.05 && w <= 1.05) {
					if (z < z_2) {
						setPixel_zbuffer(j, i, z, zbuffer);
						int texture_x = (tex1.x * u + tex2.x * v + tex3.x * w) * texture->width;
						int texture_y = (tex1.y * u + tex2.y * v + tex3.y * w) * texture->height;
						Color c = getPixel_text(texture_x, texture_y, texture);

						paint_pixel(j, i, c);

					}

				}

			}
		}
	}
}
Vector3 Image::reflect(Vector3 i, Vector3 n) {
	return (n * (2.0 * clamp(i.dot(n), 0.0, 1.0))) - i;

}

void Image::PhongIlluminationTexture(int x0, int y0, int x1, int y1, int x2, int y2, float z0, float z1, float z2, FloatImage* zbuffer, Material* material, Light* light, Vector3 cam_pos, Vector2 tex1, Vector2 tex2, Vector2 tex3, Image* texture, Image* texture_normal) {
	init_table();

	breshenhamTable(x0, y0, x1, y1, Color::BLUE, table);
	breshenhamTable(x1, y1, x2, y2, Color::RED, table);
	breshenhamTable(x2, y2, x0, y0, Color::GREEN, table);

	Vector3 v0 = Vector3(x1 - x0, y1 - y0, z1 - z0);
	Vector3 v1 = Vector3(x2 - x0, y2 - y0, z2 - z0);


	float d00 = v0.dot(v0);
	float d01 = v0.dot(v1);
	float d11 = v1.dot(v1);

	for (int i = 0; i < height; i++) {
		if (table[i].maxx >= table[i].minx) {
			for (int j = table[i].minx; j <= table[i].maxx; j++) {

				Vector3 v2 = Vector3(j - x0, i - y0, 1);
				float d20 = v2.dot(v0);
				float d21 = v2.dot(v1);

				float denom = d00 * d11 - d01 * d01;

				float v = (d11 * d20 - d01 * d21) / denom;
				float w = (d00 * d21 - d01 * d20) / denom;
				float u = 1.0 - v - w;


				float z = z0 * u + z1 * v + z2 * w;
				float z_2 = getPixel_zbuffer(j, i, zbuffer);

				if (z < z_2 && v >= -0.05 && v <= 1.05 && u >= -0.05 && u <= 1.05 && w >= -0.05 && w <= 1.05) {
					setPixel_zbuffer(j, i, z, zbuffer);

					int texture_x = (tex1.x * u + tex2.x * v + tex3.x * w) * (texture->width-1);
					int texture_y = (tex1.y * u + tex2.y * v + tex3.y * w) * (texture->height-1);

					Color c1 = getPixel_text(texture_x, texture_y, texture);

					int texture_normal_x = (tex1.x * u + tex2.x * v + tex3.x * w) * (texture_normal->width-1);
					int texture_normal_y = (tex1.y * u + tex2.y * v + tex3.y * w) * (texture_normal->height-1);

					

					Color c2 = getPixel_text(texture_normal_x, texture_normal_y, texture_normal);
				   

					float c1_r = float(c1.r / 255.0);
					float c1_g = float(c1.g / 255.0);
					float c1_b = float(c1.b / 255.0);

					float c2_r = float(c2.r / 255.0);
					float c2_g = float(c2.g / 255.0);
					float c2_b = float(c2.b / 255.0);

					
					//No multipliquem per la model ja que no mourem la mesh, mourem el punt de vista.
					Vector3 N = Vector3(c2_r, c2_g, c2_b);
					N.normalize();

					Vector3 L = light->position - Vector3(j, i, z);
					L.normalize();

					Vector3 V = cam_pos - Vector3(j, i, z);
					V.normalize();


					Vector3 R = reflect(L, N);
					R.x * -1;
					R.y * -1;
					R.z * -1;
					R.normalize();


					Vector3 ambient_light(0.1, 0.1, 0.1);


					Vector3 diffuse = Vector3(material->diffuse.x * light->diffuse_color.x * c1_r, material->diffuse.y * light->diffuse_color.y * c1_g, material->diffuse.z * light->diffuse_color.z * c1_b) * clamp(-L.dot(N), 0.0, 1.0);
					Vector3 specular = Vector3(material->specular.x * light->specular_color.x * c1_r, material->specular.y * light->specular_color.y * c1_g, material->specular.z * light->specular_color.z * c1_b) * pow(std::max((float)R.dot(V), float(0)), material->shininess);
					Vector3 ambient = Vector3(material->ambient.x * ambient_light.x * c1_r, material->ambient.y * ambient_light.y * c1_g, material->ambient.z * ambient_light.z * c1_b);

					Vector3 Ip = diffuse + specular + ambient;


					Color color = Color(Ip.x * 255, Ip.y * 255, Ip.z * 255);
					paint_pixel(j, i, color);

				}
			}
		}
	}
}

void Image::drawLineBresenham(int x0, int y0, int x1, int y1, Color c) {

	int dx, dy, inc_E, inc_NE, d, x, y;

	int y_increment = 1;

	if (x0 > x1) {
		int aux = x0;
		x0 = x1;
		x1 = aux;

		aux = y0;

		y0 = y1;
		y1 = aux;
	}
	x = x0;
	y = y0;

	dx = x1 - x0;
	dy = y1 - y0;


	if (dy < 0) {
		y_increment = -1;
		dy = -dy;
	}
	paint_pixel(x, y, c);

	if (dx > dy) {
		inc_E = 2 * dy;
		inc_NE = 2 * (dy - dx);
		d = 2 * dy - dx;

		while (x < x1) {

			if (d <= 0) {
				d += inc_E;
				x += 1;
			}
			else {
				d += inc_NE;
				y += y_increment;
				x += 1;
			}
			paint_pixel(x, y, c);
		}
	}
	else {

		inc_E = 2 * dx;
		inc_NE = 2 * (dx - dy);
		d = 2 * dx - dy;
		while (y != y1) { // A veces aumenta y a veces disminuye, por eso ponemos != para que cuando llegue pare(no < o > ya que depende de cada caso)
			if (d <= 0) {
				d += inc_E;
				y += y_increment;
			}
			else {
				d += inc_NE;
				x += 1;
				y += y_increment;
			}
			paint_pixel(x, y, c);
		}
	}
}

//Funciones para asegurarnos la entrada de valores correctos
void Image::paint_pixel(int x, int y, Color c) {
	if (x >= 0 && x < width && y>=0 && y < height) {
		setPixel(x, y, c);
	}
}

void Image::setPixel_zbuffer(int x, int y, float z, FloatImage* zbuffer) {
	if (x >= 0 && x < zbuffer->width && y>=0 && y < zbuffer->height) {
		zbuffer->setPixel(x, y, z);
	}
}

float Image::getPixel_zbuffer(int x, int y, FloatImage* zbuffer) {
	if (x >= 0 && x < zbuffer->width && y>=0 && y < zbuffer->height) {
		return zbuffer->getPixel(x, y);
	}
}

Color Image::getPixel_text(int x, int y, Image* texture) {
	if (x >= 0 && x < texture->width && y>=0 && y < texture->height) {
		return texture->getPixel(x, y);
	}
}
