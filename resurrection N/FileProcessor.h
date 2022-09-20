#pragma once
#include <string>
#include <vector>
#define FOUR_BIT 4
#define EIGHT_BIT 8
#define SIXTEEN_BIT 16

class FileProcessor {
public:
	FileProcessor(std::wstring fileName) {
		this->fileName = fileName;
	}
	void setupImage(int offset, int paletteOffset);
	void loadGameFile4Bit(int offset);
	void loadGameFile8Bit(int offset);
	void loadGameFile16Bit(int offset);
	void writeImgFile(std::wstring outFileName);
	void WidenArray();
	void loadPalette(int paletteOffset, int paletteSize);
	void genPixelArray();
	void nullifyPixelArray();
	void setDims(int width, int height, int mode);
	void flipPaletteMode();
	unsigned char* getPaletteArray();
	unsigned char* getPixelArray();
	
private:
	unsigned char* getBMPHeader();
	std::wstring fileName = L"";
	unsigned char* RGBpointer = NULL;
	unsigned char RGBpalette[256][3];
	int width = 0;
	int height = 0;
	bool defaultPalette = false;
	unsigned char* hexArray = NULL;
	unsigned char* narrowArray = NULL;
	unsigned char* pixelArray = NULL;
	int mode = FOUR_BIT;
};

