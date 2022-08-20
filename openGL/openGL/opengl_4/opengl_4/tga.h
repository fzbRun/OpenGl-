#ifndef __TEXTURE_H__    // 看看此头文件是否已经被包含
#define __TEXTURE_H__    // 如果没有，定义它

#include <stdio.h>    // 标准文件I/O头文件 
#include<iostream>
#include<vector>
#include <fstream>

#include"glad/glad.h"
#include"GLFW/glfw3.h"
#include"../../OpenGL-Mathematics-master/glm/glm.hpp"
#include"../../OpenGL-Mathematics-master/glm/gtc//matrix_transform.hpp"
#include"../../OpenGL-Mathematics-master/glm/gtc//type_ptr.hpp"

/*
typedef struct
{
	GLubyte* imageData;   // 控制整个图像的颜色值
	GLuint  bpp;    // 控制单位像素的bit数
	GLuint width;    // 整个图像的宽度
	GLuint height;    // 整个图像的高度
	GLuint texID;    // 使用glBindTexture所需的纹理ID.
	GLuint type;     // 描述存储在*ImageData中的数据(GL_RGB Or GL_RGBA)
} tgaTexture;

typedef struct
{
	GLubyte Header[12];   // 文件头决定文件类型
} TGAHeader;

typedef struct
{
	GLubyte header[6];    // 控制前6个字节
	GLuint bytesPerPixel;   // 每像素的字节数 (3 或 4)
	GLuint imageSize;    // 控制存储图像所需的内存空间
	GLuint type;    // 图像类型 GL_RGB 或 GL_RGBA
	GLuint Height;    // 图像的高度
	GLuint Width;    // 图像宽度
	GLuint Bpp;    // 每像素的比特数 (24 或 32)
} TGA;

class tga {
public:
	TGAHeader tgaheader;    // 用来存储我们的文件头
	TGA tga;      // 用来存储文件信息

	// 未压缩的TGA头
	GLubyte uTGAcompare[12] = { 0,0, 2,0,0,0,0,0,0,0,0,0 };
	// 压缩的TGA头
	GLubyte cTGAcompare[12] = { 0,0,10,0,0,0,0,0,0,0,0,0 };

	// 读取一个未压缩的文件
	bool LoadUncompressedTGA(tgaTexture*, const char*, FILE*);
	// 读取一个压缩的文件
	bool LoadCompressedTGA(tgaTexture*, const char*, FILE*);

	bool LoadTGA(tgaTexture* texture, const char* filename);
};
*/

typedef union PixelInfo
{
	std::uint32_t Colour;
	struct
	{
		std::uint8_t R, G, B, A;
	};
} *PPixelInfo;

class Tga
{
private:
	std::vector<std::uint8_t> Pixels;
	bool ImageCompressed;
	std::uint32_t width, height, size, BitsPerPixel;

public:
	Tga(const char* FilePath);
	std::vector<std::uint8_t> GetPixels() { return this->Pixels; }
	std::uint32_t GetWidth() const { return this->width; }
	std::uint32_t GetHeight() const { return this->height; }
	bool HasAlphaChannel() { return BitsPerPixel == 32; }
};
#endif