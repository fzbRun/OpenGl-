#include "tga.h"    // 包含我们刚刚建立的头文件

/*bool tga::LoadUncompressedTGA(tgaTexture* texture, const char* filename, FILE* fTGA)
{
	std::cout << "uncompress" << std::endl;
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
	{
		std::cout << "0" << std::endl;
		return false;    // 返回 False
	}

	texture->width = tga.header[1] * 256 + tga.header[0]; // 计算高度
	texture->height = tga.header[3] * 256 + tga.header[2]; // 计算宽度
	texture->bpp = tga.header[4];   // 计算BPP
	tga.Width = texture->width;    // 拷贝Width到本地结构中去
	tga.Height = texture->height;   // 拷贝Height到本地结构中去
	tga.Bpp = texture->bpp;    // 拷贝Bpp到本地结构中去

	if ((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp != 32)))
	{
		std::cout << "1" << std::endl;
		return false;    // 返回 False
	}

	if (texture->bpp == 24)    // 是24 bit图像吗？
	{
		texture->type = GL_RGB;   //如果是，设置类型为GL_RGB
	}
	else      // 如果不是24bit,则必是32bit
	{
		texture->type = GL_RGBA;  //这样设置类型为GL_RGBA
	}

	tga.bytesPerPixel = (tga.Bpp / 8);  // 计算BPP
	// 计算存储图像所需的内存
	tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);

	// 分配内存
	texture->imageData = (GLubyte*)malloc(tga.imageSize);
	if (texture->imageData == NULL)   // 确认已经分配成功
	{
		std::cout << "2" << std::endl;
		return false;    // 确认已经分配成功
	}

	// 尝试读取所有图像数据
	if (fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)
	{
		std::cout << "3" << std::endl;
		return false;    // 如果不能，返回false
	}

	for (GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		// 第一字节 XOR第三字节XOR 第一字节 XOR 第三字节
		texture->imageData[cswap] ^= texture->imageData[cswap + 2] ^=
		texture->imageData[cswap] ^= texture->imageData[cswap + 2];
	}

	fclose(fTGA);     // 关闭文件
	return true;     // 返回成功
}

bool tga::LoadCompressedTGA(tgaTexture* texture, const char* filename, FILE* fTGA)
{
	std::cout << "compress" << std::endl;
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
	{
		std::cout << "4" << std::endl;
		//...Error code here...
	}
	texture->width = tga.header[1] * 256 + tga.header[0];
	texture->height = tga.header[3] * 256 + tga.header[2];
	texture->bpp = tga.header[4];
	tga.Width = texture->width;
	tga.Height = texture->height;
	tga.Bpp = texture->bpp;
	if ((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp != 32)))
	{
		std::cout << "5" << std::endl;
		//...Error code here...
	}

	if (texture->bpp == 24)    // 是24 bit图像吗？
	{
		texture->type = GL_RGB;   //如果是，设置类型为GL_RGB
		std::cout << "rgb" << std::endl;
	}
	else      // 如果不是24bit,则必是32bit
	{
		texture->type = GL_RGBA;  //这样设置类型为GL_RGBA
		std::cout << "rgba" << std::endl;
	}

	tga.bytesPerPixel = (tga.Bpp / 8);
	tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);

	texture->imageData = (GLubyte*)malloc(tga.imageSize);
	if (texture->imageData == NULL)   // 如果不能分配内存
	{
		std::cout << "6" << std::endl;
		//...Error code here...
		return false;    // 返回 False
	}

	GLuint pixelcount = tga.Height * tga.Width; // 图像中的像素数
	GLuint currentpixel = 0;  // 当前正在读取的像素
	GLuint currentbyte = 0;   // 当前正在向图像中写入的像素
   // 一个像素的存储空间
	GLubyte* colorbuffer = (GLubyte*)malloc(tga.bytesPerPixel);

	do      // 开始循环
	{
		GLubyte chunkheader = 0;    // 存储Id块值的变量
		if (fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0) // 尝试读取块的头
		{
			std::cout << "7" << std::endl;
			//...Error code...
			return false;    // If It Fails, Return False
		}
		if (chunkheader < 128)    // 如果是RAW块
		{
			chunkheader++;    // 变量值加1以获取RAW像素的总数
			// 开始像素读取循环
			for (short counter = 0; counter < chunkheader; counter++)
			{
				// 尝试读取一个像素
				if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
				{
					std::cout << "8" << std::endl;
					//...Error code...
					return false;   // 如果失败，返回false
				}

				texture->imageData[currentbyte] = colorbuffer[2];  // 写“R”字节
				texture->imageData[currentbyte + 1] = colorbuffer[1]; //写“G”字节
				texture->imageData[currentbyte + 2] = colorbuffer[0]; // 写“B”字节
				if (tga.bytesPerPixel == 4)     // 如果是32位图像...
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3]; // 写“A”字节
				}
				// 依据每像素的字节数增加字节计数器
				currentbyte += tga.bytesPerPixel;
				currentpixel++;     // 像素计数器加1
			}
		}
		else      // 如果是RLE头
		{
			chunkheader -= 127;   //  减去127获得ID Bit的Rid
			// 读取下一个像素
			if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
			{
				std::cout << "9" << std::endl;
				//...Error code...
				return false;    // 如果失败，返回false
			}
			// 开始循环
			for (short counter = 0; counter < chunkheader; counter++)
			{
				// 拷贝“R”字节
				texture->imageData[currentbyte] = colorbuffer[2];
				// 拷贝“G”字节
				texture->imageData[currentbyte + 1] = colorbuffer[1];
				// 拷贝“B”字节
				texture->imageData[currentbyte + 2] = colorbuffer[0];
				if (tga.bytesPerPixel == 4)  // 如果是32位图像
				{
					// 拷贝“A”字节
					texture->imageData[currentbyte + 3] = colorbuffer[3];
				}
				currentbyte += tga.bytesPerPixel; // 增加字节计数器
				currentpixel++;   // 增加字节计数器
			}
		}
	} while (currentpixel < pixelcount); // 是否有更多的像素要读取？开始循环直到最后
	fclose(fTGA);   // 关闭文件
	return true;   // 返回成功
}


// 读取一个TGA文件!
bool tga::LoadTGA(tgaTexture* texture, const char* filename)
{
	FILE* fTGA;     // 声明文件指针
	errno_t err = fopen_s(&fTGA, filename, "rb");   // 以读模式打开文件

	if (fTGA == NULL)    // 如果此处有错误
	{
		std::cout << "10" << std::endl;
		return false;    // 返回 False
	}

	if (fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)
	{
		std::cout << "11" << std::endl;
		return false;    //  如果失败则返回 False
	}

	// 如果文件头附合未压缩的文件头格式
	if (memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
	{
		// 读取未压缩的TGA文件
		LoadUncompressedTGA(texture, filename, fTGA);
	}
	// 如果文件头附合压缩的文件头格式
	else if (memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
	{
		// 读取压缩的TGA格式
		LoadCompressedTGA(texture, filename, fTGA);
	}
	else      // 如果任一个都不符合
	{
		std::cout << "12" << std::endl;
		return false;    // 返回 False
	}
}
*/

Tga::Tga(const char* FilePath)
{
	std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
	if (!hFile.is_open()) { throw std::invalid_argument("File Not Found."); }

	std::uint8_t Header[18] = { 0 };
	std::vector<std::uint8_t> ImageData;
	static std::uint8_t DeCompressed[12] = { 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	static std::uint8_t IsCompressed[12] = { 0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

	hFile.read(reinterpret_cast<char*>(&Header), sizeof(Header));

	if (!std::memcmp(DeCompressed, &Header, sizeof(DeCompressed)))
	{
		BitsPerPixel = Header[16];//像素所占位数
		width = Header[13] * 256 + Header[12];
		height = Header[15] * 256 + Header[14];
		size = ((width * BitsPerPixel + 31) / 32) * 4 * height;

		if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
		{
			hFile.close();
			throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit Image.");
		}

		ImageData.resize(size);
		ImageCompressed = false;
		hFile.read(reinterpret_cast<char*>(ImageData.data()), size);
	}
	else if (!std::memcmp(IsCompressed, &Header, sizeof(IsCompressed)))
	{
		BitsPerPixel = Header[16];
		width = Header[13] * 256 + Header[12];
		height = Header[15] * 256 + Header[14];
		size = ((width * BitsPerPixel + 31) / 32) * 4 * height;

		if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
		{
			hFile.close();
			throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit Image.");
		}

		PixelInfo Pixel = { 0 };
		int CurrentByte = 0;
		std::size_t CurrentPixel = 0;
		ImageCompressed = true;
		std::uint8_t ChunkHeader = { 0 };
		int BytesPerPixel = (BitsPerPixel / 8);
		ImageData.resize(width * height * sizeof(PixelInfo));

		do
		{
			hFile.read(reinterpret_cast<char*>(&ChunkHeader), sizeof(ChunkHeader));

			if (ChunkHeader < 128)
			{
				++ChunkHeader;
				for (int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
				{
					hFile.read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

					ImageData[CurrentByte++] = Pixel.B;
					ImageData[CurrentByte++] = Pixel.G;
					ImageData[CurrentByte++] = Pixel.R;
					if (BitsPerPixel > 24) ImageData[CurrentByte++] = Pixel.A;
				}
			}
			else
			{
				ChunkHeader -= 127;
				hFile.read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

				for (int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
				{
					ImageData[CurrentByte++] = Pixel.B;
					ImageData[CurrentByte++] = Pixel.G;
					ImageData[CurrentByte++] = Pixel.R;
					if (BitsPerPixel > 24) ImageData[CurrentByte++] = Pixel.A;
				}
			}
		} while (CurrentPixel < (width * height));
	}
	else
	{
		hFile.close();
		throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit TGA File.");
	}

	hFile.close();
	this->Pixels = ImageData;
}
