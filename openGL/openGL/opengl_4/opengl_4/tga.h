#ifndef __TEXTURE_H__    // ������ͷ�ļ��Ƿ��Ѿ�������
#define __TEXTURE_H__    // ���û�У�������

#include <stdio.h>    // ��׼�ļ�I/Oͷ�ļ� 
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
	GLubyte* imageData;   // ��������ͼ�����ɫֵ
	GLuint  bpp;    // ���Ƶ�λ���ص�bit��
	GLuint width;    // ����ͼ��Ŀ��
	GLuint height;    // ����ͼ��ĸ߶�
	GLuint texID;    // ʹ��glBindTexture���������ID.
	GLuint type;     // �����洢��*ImageData�е�����(GL_RGB Or GL_RGBA)
} tgaTexture;

typedef struct
{
	GLubyte Header[12];   // �ļ�ͷ�����ļ�����
} TGAHeader;

typedef struct
{
	GLubyte header[6];    // ����ǰ6���ֽ�
	GLuint bytesPerPixel;   // ÿ���ص��ֽ��� (3 �� 4)
	GLuint imageSize;    // ���ƴ洢ͼ��������ڴ�ռ�
	GLuint type;    // ͼ������ GL_RGB �� GL_RGBA
	GLuint Height;    // ͼ��ĸ߶�
	GLuint Width;    // ͼ����
	GLuint Bpp;    // ÿ���صı����� (24 �� 32)
} TGA;

class tga {
public:
	TGAHeader tgaheader;    // �����洢���ǵ��ļ�ͷ
	TGA tga;      // �����洢�ļ���Ϣ

	// δѹ����TGAͷ
	GLubyte uTGAcompare[12] = { 0,0, 2,0,0,0,0,0,0,0,0,0 };
	// ѹ����TGAͷ
	GLubyte cTGAcompare[12] = { 0,0,10,0,0,0,0,0,0,0,0,0 };

	// ��ȡһ��δѹ�����ļ�
	bool LoadUncompressedTGA(tgaTexture*, const char*, FILE*);
	// ��ȡһ��ѹ�����ļ�
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