#include "FileProcessor.h"
#include <fstream>
#include <list>
#include <iostream>


void FileProcessor::setupImage(int offset, int paletteOffset) {    
    //filepath?
    if (mode == FOUR_BIT) {
        loadGameFile4Bit(offset);
        WidenArray();
        loadPalette(paletteOffset, 16);
    }
    else if (mode == EIGHT_BIT) {
        loadGameFile8Bit(offset);
        loadPalette(paletteOffset, 256);
    }
    else if (mode == SIXTEEN_BIT) {
        loadGameFile16Bit(offset);
    }
}

void FileProcessor::loadGameFile4Bit(int offset) {
    std::ifstream imgFile;
    imgFile.open(fileName, std::ios::binary);
    char hex[1] = { 0 };
    int counter = offset;
    imgFile.seekg(offset);
    while (!imgFile.eof()) {
        imgFile.read(hex, 1);
        if (counter >= offset && counter < offset + (width * height / 2)) {
            *(narrowArray + counter - offset) = *hex;
        }
        else if (counter > offset + (width * height / 2)) {
            break;
        }
        counter++;
    }
    imgFile.close();
}

void FileProcessor::loadGameFile8Bit(int offset) {
    std::ifstream imgFile;
    imgFile.open(fileName, std::ios::binary);
    char hex[1] = { 0 };
    int counter = offset;
    imgFile.seekg(offset);
    while (!imgFile.eof()) {
        imgFile.read(hex, 1);
        if (counter >= offset && counter < offset + (width * height)) {
            *(hexArray + counter - offset) = *hex;
        }
        else if (counter > offset + (width * height)) {
            break;
        }
        counter++;
    }
    imgFile.close();
}

void FileProcessor::loadGameFile16Bit(int offset) {
    std::ifstream imgFile;
    imgFile.open(fileName, std::ios::binary);
    char color[2] = { 0, 0 };
    int counter = offset;
    char temp;
    int i = 0;
    imgFile.seekg(offset);
    while (!imgFile.eof()) {
        imgFile.read((char*)color, 2);
        if (counter >= offset && counter < offset + (width * height * 2)) {
            temp = color[0];
            color[0] = color[1];
            color[1] = temp;
            *(pixelArray + i*3) = (*(unsigned short*)color & 0x7c00) >> 7;
            *(pixelArray + i*3 + 1) = (*(unsigned short*)color & 0x03e0) >> 2;
            *(pixelArray + i*3 + 2) = (*(unsigned short*)color & 0x001f) << 3;
        }
        else if (counter > offset + (width * height  * 2)) {
            break;
        }
        counter += 2;
        i += 1;
    }
    imgFile.close();
}

unsigned char* FileProcessor::getPaletteArray() {
    if (RGBpointer != NULL) {
        free(RGBpointer);
        RGBpointer = NULL;
    }
    RGBpointer = (unsigned char*)malloc(pow(2.0,(double)mode) * 3);
    for (int i = 0; i < pow(2.0, (double)mode); i++) {
        for (int j = 0; j < 3; j++) {
            *(RGBpointer + i * 3 + j) = RGBpalette[i][j];
        }
    }
    return RGBpointer;
}

void FileProcessor::setDims(int width, int height, int mode) {
    this->mode = mode;
    this->width = width;
    this->height = height;
    if (hexArray != NULL) {
        free(hexArray);
    }
    if (narrowArray != NULL) {
        free(narrowArray);
    }
    if (pixelArray != NULL) {
        free(pixelArray);
    }
    hexArray = (unsigned char*)malloc(width * height);
    narrowArray = (unsigned char*)malloc(width * height / 2);
    pixelArray = (unsigned char*)malloc(3 * width * height);
}

void FileProcessor::genPixelArray() {
    if (mode != SIXTEEN_BIT) {
        for (int i = 0; i < width * height; i++) {
            *(pixelArray + i * 3) = RGBpalette[hexArray[i]][0];
            *(pixelArray + i * 3 + 1) = RGBpalette[hexArray[i]][1];
            *(pixelArray + i * 3 + 2) = RGBpalette[hexArray[i]][2];
        }
    }
}

unsigned char* FileProcessor::getPixelArray() {
    return pixelArray;
}

void FileProcessor::flipPaletteMode() {
    defaultPalette = !defaultPalette;
}

void FileProcessor::nullifyPixelArray() {
    pixelArray = NULL;
}

void FileProcessor::WidenArray() {
    int arraySize = width * height;
    for (int i = 0; i < arraySize / 2; i++) {
        *(hexArray + i * 2) = (*(narrowArray + i) >> 4) & 0x0F;
        *(hexArray + i * 2 + 1) = *(narrowArray + i) & 0x0F;
    }
}


void FileProcessor::loadPalette(int paletteOffset, int paletteSize) {
    unsigned char color[2];
    unsigned char temp;
    std::ifstream paletteReader;
    paletteReader.open(fileName, std::ios::binary);
    paletteReader.seekg(paletteOffset);
    if (defaultPalette) {
        for (int i = 0; i < paletteSize; i++) {
            RGBpalette[i][0] = 255 - i * 255 / paletteSize;
            RGBpalette[i][1] = 255 - i * 255 / paletteSize;
            RGBpalette[i][2] = 255 - i * 255 / paletteSize;
        }
        return;
    }
    for (int i = 0; i < paletteSize; i++) {
        paletteReader.read((char*)color, 2);
        temp = color[0];
        color[0] = color[1];
        color[1] = temp;
        RGBpalette[i][0] = (*(unsigned short*)color & 0x7c00) >> 7;
        RGBpalette[i][1] = (*(unsigned short*)color & 0x03e0) >> 2;
        RGBpalette[i][2] = (*(unsigned short*)color & 0x001f) << 3;
    }
}

void FileProcessor::writeImgFile(std::wstring outFileName, int numChunksX, int numChunksY) {
    int bmpNumBytes = width * height + 0x36;
    unsigned char* bmpNumBytesP = (unsigned char*)&bmpNumBytes;
    unsigned char* heightP = (unsigned char*)&height;
    unsigned char* widthP = (unsigned char*)&width;
    unsigned char bmpHeader[0x36] = { 0x42, 0x4D, *bmpNumBytesP, *(bmpNumBytesP + 1), *(bmpNumBytesP + 2),
                                         *(bmpNumBytesP + 3),
                                         0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
                                         *widthP, *(widthP + 1), *(widthP + 2), *(widthP + 3),
                                         *heightP, *(heightP + 1), *(heightP + 2), *(heightP + 3),
                                         0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
                                         0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    std::ofstream imgConverted;
    int tileWidth = width / numChunksX;
    int tileHeight = height / numChunksY;
    int index = 0;
    unsigned char* tileArray = (unsigned char*)malloc(3 * width * height * numChunksX * numChunksY);
    printf("%d %d\n", tileWidth * tileHeight * numChunksX * numChunksY, width * height);
    for (int i = 0; i < tileHeight * numChunksY * numChunksX; i++) {
        for (int j = 0; j < tileWidth; j++) {
            int topSide = ((i * tileWidth + j) / (tileWidth * tileHeight * numChunksX)) * tileHeight + i % tileHeight;
            int leftSide = ((i * tileWidth + j) % (tileWidth * tileHeight * numChunksX) / (tileWidth * tileHeight)) * tileWidth + j;
            index = ((width * height) - ((topSide * width) + (width - leftSide))) * 3;
            *(tileArray + index) = *(pixelArray + (i * tileWidth + j) * 3);
            *(tileArray + index + 1) = *(pixelArray + (i * tileWidth + j) * 3 + 1);
            *(tileArray + index + 2) = *(pixelArray + (i * tileWidth + j) * 3 + 2);
        }
    }
    free(pixelArray);
    pixelArray = tileArray;
    
    imgConverted.open(outFileName, std::ios::binary);
    imgConverted.write((char*)bmpHeader, 0x36);
    imgConverted.write((char*)pixelArray, 3*width*height);
    imgConverted.close();
}

