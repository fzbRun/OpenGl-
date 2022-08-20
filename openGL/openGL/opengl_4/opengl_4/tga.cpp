#include "tga.h"    // �������Ǹոս�����ͷ�ļ�

/*bool tga::LoadUncompressedTGA(tgaTexture* texture, const char* filename, FILE* fTGA)
{
	std::cout << "uncompress" << std::endl;
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
	{
		std::cout << "0" << std::endl;
		return false;    // ���� False
	}

	texture->width = tga.header[1] * 256 + tga.header[0]; // ����߶�
	texture->height = tga.header[3] * 256 + tga.header[2]; // ������
	texture->bpp = tga.header[4];   // ����BPP
	tga.Width = texture->width;    // ����Width�����ؽṹ��ȥ
	tga.Height = texture->height;   // ����Height�����ؽṹ��ȥ
	tga.Bpp = texture->bpp;    // ����Bpp�����ؽṹ��ȥ

	if ((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp != 32)))
	{
		std::cout << "1" << std::endl;
		return false;    // ���� False
	}

	if (texture->bpp == 24)    // ��24 bitͼ����
	{
		texture->type = GL_RGB;   //����ǣ���������ΪGL_RGB
	}
	else      // �������24bit,�����32bit
	{
		texture->type = GL_RGBA;  //������������ΪGL_RGBA
	}

	tga.bytesPerPixel = (tga.Bpp / 8);  // ����BPP
	// ����洢ͼ��������ڴ�
	tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);

	// �����ڴ�
	texture->imageData = (GLubyte*)malloc(tga.imageSize);
	if (texture->imageData == NULL)   // ȷ���Ѿ�����ɹ�
	{
		std::cout << "2" << std::endl;
		return false;    // ȷ���Ѿ�����ɹ�
	}

	// ���Զ�ȡ����ͼ������
	if (fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)
	{
		std::cout << "3" << std::endl;
		return false;    // ������ܣ�����false
	}

	for (GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		// ��һ�ֽ� XOR�����ֽ�XOR ��һ�ֽ� XOR �����ֽ�
		texture->imageData[cswap] ^= texture->imageData[cswap + 2] ^=
		texture->imageData[cswap] ^= texture->imageData[cswap + 2];
	}

	fclose(fTGA);     // �ر��ļ�
	return true;     // ���سɹ�
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

	if (texture->bpp == 24)    // ��24 bitͼ����
	{
		texture->type = GL_RGB;   //����ǣ���������ΪGL_RGB
		std::cout << "rgb" << std::endl;
	}
	else      // �������24bit,�����32bit
	{
		texture->type = GL_RGBA;  //������������ΪGL_RGBA
		std::cout << "rgba" << std::endl;
	}

	tga.bytesPerPixel = (tga.Bpp / 8);
	tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);

	texture->imageData = (GLubyte*)malloc(tga.imageSize);
	if (texture->imageData == NULL)   // ������ܷ����ڴ�
	{
		std::cout << "6" << std::endl;
		//...Error code here...
		return false;    // ���� False
	}

	GLuint pixelcount = tga.Height * tga.Width; // ͼ���е�������
	GLuint currentpixel = 0;  // ��ǰ���ڶ�ȡ������
	GLuint currentbyte = 0;   // ��ǰ������ͼ����д�������
   // һ�����صĴ洢�ռ�
	GLubyte* colorbuffer = (GLubyte*)malloc(tga.bytesPerPixel);

	do      // ��ʼѭ��
	{
		GLubyte chunkheader = 0;    // �洢Id��ֵ�ı���
		if (fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0) // ���Զ�ȡ���ͷ
		{
			std::cout << "7" << std::endl;
			//...Error code...
			return false;    // If It Fails, Return False
		}
		if (chunkheader < 128)    // �����RAW��
		{
			chunkheader++;    // ����ֵ��1�Ի�ȡRAW���ص�����
			// ��ʼ���ض�ȡѭ��
			for (short counter = 0; counter < chunkheader; counter++)
			{
				// ���Զ�ȡһ������
				if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
				{
					std::cout << "8" << std::endl;
					//...Error code...
					return false;   // ���ʧ�ܣ�����false
				}

				texture->imageData[currentbyte] = colorbuffer[2];  // д��R���ֽ�
				texture->imageData[currentbyte + 1] = colorbuffer[1]; //д��G���ֽ�
				texture->imageData[currentbyte + 2] = colorbuffer[0]; // д��B���ֽ�
				if (tga.bytesPerPixel == 4)     // �����32λͼ��...
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3]; // д��A���ֽ�
				}
				// ����ÿ���ص��ֽ��������ֽڼ�����
				currentbyte += tga.bytesPerPixel;
				currentpixel++;     // ���ؼ�������1
			}
		}
		else      // �����RLEͷ
		{
			chunkheader -= 127;   //  ��ȥ127���ID Bit��Rid
			// ��ȡ��һ������
			if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
			{
				std::cout << "9" << std::endl;
				//...Error code...
				return false;    // ���ʧ�ܣ�����false
			}
			// ��ʼѭ��
			for (short counter = 0; counter < chunkheader; counter++)
			{
				// ������R���ֽ�
				texture->imageData[currentbyte] = colorbuffer[2];
				// ������G���ֽ�
				texture->imageData[currentbyte + 1] = colorbuffer[1];
				// ������B���ֽ�
				texture->imageData[currentbyte + 2] = colorbuffer[0];
				if (tga.bytesPerPixel == 4)  // �����32λͼ��
				{
					// ������A���ֽ�
					texture->imageData[currentbyte + 3] = colorbuffer[3];
				}
				currentbyte += tga.bytesPerPixel; // �����ֽڼ�����
				currentpixel++;   // �����ֽڼ�����
			}
		}
	} while (currentpixel < pixelcount); // �Ƿ��и��������Ҫ��ȡ����ʼѭ��ֱ�����
	fclose(fTGA);   // �ر��ļ�
	return true;   // ���سɹ�
}


// ��ȡһ��TGA�ļ�!
bool tga::LoadTGA(tgaTexture* texture, const char* filename)
{
	FILE* fTGA;     // �����ļ�ָ��
	errno_t err = fopen_s(&fTGA, filename, "rb");   // �Զ�ģʽ���ļ�

	if (fTGA == NULL)    // ����˴��д���
	{
		std::cout << "10" << std::endl;
		return false;    // ���� False
	}

	if (fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)
	{
		std::cout << "11" << std::endl;
		return false;    //  ���ʧ���򷵻� False
	}

	// ����ļ�ͷ����δѹ�����ļ�ͷ��ʽ
	if (memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
	{
		// ��ȡδѹ����TGA�ļ�
		LoadUncompressedTGA(texture, filename, fTGA);
	}
	// ����ļ�ͷ����ѹ�����ļ�ͷ��ʽ
	else if (memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
	{
		// ��ȡѹ����TGA��ʽ
		LoadCompressedTGA(texture, filename, fTGA);
	}
	else      // �����һ����������
	{
		std::cout << "12" << std::endl;
		return false;    // ���� False
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
		BitsPerPixel = Header[16];//������ռλ��
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
