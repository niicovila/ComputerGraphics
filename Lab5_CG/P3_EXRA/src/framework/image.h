/*** image.h  Javi Agenjo (javi.agenjo@gmail.com) 2013
	This file defines the class Image that allows to manipulate images.
	It defines all the need operators for Color and Image.
	It has a TGA loader and saver.
***/

#ifndef IMAGE_H
#define IMAGE_H

#include <string.h>
#include <stdio.h>
#include <iostream>
#include "framework.h"
#include "light.h"
#include "material.h"

//remove unsafe warnings
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

class FloatImage;

//Class Image: to store a matrix of pixels
class Image
{
	//a general struct to store all the information about a TGA file
	typedef struct sTGAInfo 
	{
		unsigned int width;
		unsigned int height;
		unsigned int bpp; //bits per pixel
		unsigned char* data; //bytes with the pixel information 
	} TGAInfo;
	struct sCelda {
		int minx;
		int maxx;
	};

public:
	unsigned int width;
	unsigned int height;
	Color* pixels;
	std::vector<sCelda> table;
	// CONSTRUCTORS 
	Image();
	Image(unsigned int width, unsigned int height);
	Image(const Image& c);
	Image& operator = (const Image& c); //assign operator 

	//destructor
	~Image();

	//get the pixel at position x,y
	Color getPixel(unsigned int x, unsigned int y) const { return pixels[ y * width + x ]; }
	Color& getPixelRef(unsigned int x, unsigned int y)	{ return pixels[ y * width + x ]; }
	Color getPixelSafe(unsigned int x, unsigned int y) const {	
		x = clamp((unsigned int)x, 0, width-1); 
		y = clamp((unsigned int)y, 0, height-1); 
		return pixels[ y * width + x ]; 
	}

	//set the pixel at position x,y with value C 
	inline void setPixel(unsigned int x, unsigned int y, const Color& c) { pixels[ y * width + x ] = c; }
	inline void setPixelSafe(unsigned int x, unsigned int y, const Color& c) const { x = clamp(x, 0, width-1); y = clamp(y, 0, height-1); pixels[ y * width + x ] = c; }

	void resize(unsigned int width, unsigned int height);
	void scale(unsigned int width, unsigned int height);
	
	void flipY(); //flip the image top-down
	void flipX(); //flip the image left-right

	//fill the image with the color C
	void fill(const Color& c) { for(unsigned int pos = 0; pos < width*height; ++pos) pixels[pos] = c; }

	//returns a new image with the area from (startx,starty) of size width,height
	Image getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height);

	//save or load images from the hard drive
	bool loadTGA(const char* filename);
	bool saveTGA(const char* filename);

	//used to easy code
	#ifndef IGNORE_LAMBDAS

	//applies an algorithm to every pixel in an image
	// you can use lambda sintax:   img.forEachPixel( [](Color c) { return c*2; });
	// or callback sintax:   img.forEachPixel( mycallback ); //the callback has to be Color mycallback(Color c) { ... }
	template <typename F>
	Image& forEachPixel( F callback )
	{
		for(unsigned int pos = 0; pos < width*height; ++pos)
			pixels[pos] = callback(pixels[pos]);
		return *this;
	}

	void paint_pixel(int x, int y, Color c);
	void DDA(int x0, int y0, int x1, int y1, Color color);
	void drawLineBresenham(int x0, int y0, int x1, int y1, Color c);
	void bresenhamCircle(int center_x, int center_y, int rad, Color c, bool fill);
	int sgn(float n);
	void init_table();
	void breshenhamTable(int x0, int y0, int x1, int y1, Color c, std::vector<sCelda>& table);
	void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c, bool fill);
	Vector3 reflect(Vector3 i, Vector3 n);
	void drawTriangleInterpolated(int x0, int y0, int x1, int y1, int x2, int y2, float z1, float z2, float z3, FloatImage* zbuffer, Vector2 tex1, Vector2 tex2, Vector2 tex3, Image* texture);
	void drawTriangleInterpolated_color(int x0, int y0, int x1, int y1, int x2, int y2, float z1, float z2, float z3, FloatImage* zbuffer, Color c0, Color c1, Color c2);
	void PhongIlluminationTexture(int x0, int y0, int x1, int y1, int x2, int y2, float z0, float z1, float z2, FloatImage* zbuffer, Material* material, Light* light, Vector3 cam_pos, Vector2 tex1, Vector2 tex2, Vector2 tex3, Image* texture, Image* texture_normal);
	void setPixel_zbuffer(int x, int y, float z, FloatImage* zbuffer);
	float getPixel_zbuffer(int x, int y, FloatImage* zbuffer);
	Color getPixel_text(int x, int y, Image* texture);
	#endif


};

//Image that stores one float per pixel instead of a Color, like a matrix, useful for a Depth Buffer
class FloatImage
{
public:
	unsigned int width;
	unsigned int height;
	float* pixels;

	// CONSTRUCTORS 
	FloatImage() { width = height = 0; pixels = NULL; }
	FloatImage(unsigned int width, unsigned int height);
	FloatImage(const FloatImage& c);
	FloatImage& operator = (const FloatImage& c); //assign operator

	//destructor
	~FloatImage();

	void fill(const float& v) { for(unsigned int pos = 0; pos < width*height; ++pos) pixels[pos] = v; }

	//get the pixel at position x,y
	float getPixel(unsigned int x, unsigned int y) const { return pixels[y * width + x]; }
	float& getPixelRef(unsigned int x, unsigned int y) { return pixels[y * width + x]; }

	//set the pixel at position x,y with value C
	inline void setPixel(unsigned int x, unsigned int y, const float& v) { pixels[y * width + x] = v; }

	void resize(unsigned int width, unsigned int height);
};



#endif