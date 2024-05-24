#pragma once
#include <easyx.h>
#include <Eigen>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
using namespace Eigen;

//#pragma pack(push, 1)
//#pragma pack(pop)

struct Color
{
    uint8_t r, g, b, a;
};

inline std::vector<std::vector<Color>> loadBMP(const std::string & filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open file");
    }

    BITMAPFILEHEADER fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.bfType != 0x4D42)
    { // 'BM' in little-endian
        throw std::runtime_error("Not a BMP file");
    }

    BITMAPINFOHEADER infoHeader;
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32)
    {
        throw std::runtime_error("Unsupported BMP format");
    }

    file.seekg(fileHeader.bfOffBits, std::ios::beg);

    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;
    bool flip = height > 0;
    height = std::abs(height);

    std::vector<std::vector<Color>> textureData(height, std::vector<Color>(width));

    int row_padded = (width * (infoHeader.biBitCount / 8) + 3) & (~3);
    std::vector<uint8_t> row_data(row_padded);

    for (int y = 0; y < height; ++y)
    {
        int row = flip ? height - 1 - y : y;
        file.read(reinterpret_cast<char*>(row_data.data()), row_padded);

        for (int x = 0; x < width; ++x)
        {
            if (infoHeader.biBitCount == 24)
            {
                textureData[row][x] = { row_data[x * 3 + 2], row_data[x * 3 + 1], row_data[x * 3], 255 };
            }
            else if (infoHeader.biBitCount == 32)
            {
                textureData[row][x] = { row_data[x * 4 + 2], row_data[x * 4 + 1], row_data[x * 4], row_data[x * 4 + 3] };
            }
        }
    }
    return textureData;
}

class Texture
{
private:
    std::vector<std::vector<Color>> _image;

public:
	Texture(const std::string& bmpName)
	{
		const std::string path = "Res/Texture/" + bmpName+".bmp";
        _image = loadBMP(path);
	}

    Color GetColor(float u, float v)
	{
		u = u - floor(u);
		v = v - floor(v);
		//因为从0开始，所以长宽需要－1
		float uT = u * (_image.size() - 1);
		float vT = v * (_image[0].size() - 1);

        return _image[uT][vT];
	}

};
