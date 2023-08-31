#ifndef BMP_h
#define BMP_h

#include <iostream>


#pragma pack(2) //影响了“对齐”。可以实验前后 sizeof(BITMAPFILEHEADER) 的差别

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long    LONG;

//BMP文件头：
struct BITMAPFILEHEADER
{
    WORD  bfType;        //文件类型标识，必须为ASCII码“BM”
    DWORD bfSize;        //文件的尺寸，以byte为单位
    WORD  bfReserved1;    //保留字，必须为0
    WORD  bfReserved2;    //保留字，必须为0
    DWORD bfOffBits;    //一个以byte为单位的偏移，从BITMAPFILEHEADER结构体开始到位图数据
};

//BMP信息头：
struct BITMAPINFOHEADER
{
    DWORD biSize;            //这个结构体的尺寸
    LONG  biWidth;            //位图的宽度
    LONG  biHeight;            //位图的长度
    WORD  biPlanes;            //The number of planes for the target device. This value must be set to 1.
    WORD  biBitCount;        //一个像素有几位
    DWORD biCompression;    //0：不压缩，1：RLE8，2：RLE4
    DWORD biSizeImage;      //4字节对齐的图像数据大小
    LONG  biXPelsPerMeter;  //用象素/米表示的水平分辨率
    LONG  biYPelsPerMeter;  //用象素/米表示的垂直分辨率
    DWORD biClrUsed;        //实际使用的调色板索引数，0：使用所有的调色板索引
    DWORD biClrImportant;    //重要的调色板索引数，0：所有的调色板索引都重要
};

//一个像素的颜色信息
struct RGBColor
{
    char B;        //蓝
    char G;        //绿
    char R;        //红
};

//将颜色数据写到一个BMP文件中
//FileName:文件名
//ColorBuffer:颜色数据
//ImageWidth:图像宽度
//ImageHeight:图像长度
/// TODO: 这个函数貌似创建了错误的bmp，待研究中～
void WriteBMP(const char* FileName, RGBColor* ColorBuffer, int ImageWidth, int ImageHeight)
{
    //颜色数据总尺寸：
    const int ColorBufferSize = ImageHeight * ImageWidth * sizeof(RGBColor);

    //文件头
    BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4D42;    //0x42是'B'；0x4D是'M'
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ColorBufferSize;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    //信息头
    BITMAPINFOHEADER bitmapHeader = { 0 };
    bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapHeader.biHeight = ImageHeight;
    bitmapHeader.biWidth = ImageWidth;
    bitmapHeader.biPlanes = 1;
    bitmapHeader.biBitCount = 24;
    bitmapHeader.biSizeImage = ColorBufferSize;
    bitmapHeader.biCompression = 0; //BI_RGB


    FILE *fp;//文件指针

    //打开文件（没有则创建）
    fp = fopen(FileName, "wb");

    
    //写入文件头和信息头
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);
    //写入颜色数据
    fwrite(ColorBuffer, ColorBufferSize, 1, fp);
    
    fclose(fp);
}


#endif /* BMP_h */
