//	img2mif.cpp
//	
//	Converts images to Altera Memory Initialization File (.mif) format.
//	
//	How to use:
//		Pass the image, width in bits of the memory, the depth of the memory, how many bits per pixel for the memory, and color format. Color format is either 0 for Grayscale or 1 for RGB.
//		RGB color will set the RGB elements of a pixel as consecutive memory locations. 
//
//			EX: im2mif tester.bmp 8 8192 2 1
//
//		If the image is to large for the memory it will fill up as much of the memory as possible then give a warning.
//		If the image does not take up all the space the other locations will be filled with 0.
//
//	Author: Parker Dillmann
//	Website: www.longhornengineer.com

#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int x;
int y;
int comp;
int convert2bw;
int bitwidth; 
int memdepth;
int bbp;

unsigned char *data;

char *filename;

int i;
int j;
int k;
int m;
int max_value;
int min_value;
int diff;
unsigned char bytebuilder;

char *mif_header0 = "-- Created with Parker Dillmann's im2mif file converter.\n-- www.longhornengineer.com\n\n-- Quartus II generated Memory Initialization File (.mif)\n\n";

int main(int argc, char* argv[])
{
	//Error if not enough arguements and display "help".
	if (argc != 6)
	{
		printf("\nInvalid arguements.");
		printf("\nUSAGE: img2mif -width -depth -bpp -color");
		printf("\n");
		printf("\n-width : Width of memory in bits. Only support for 8, 16, 32, 64, 128 at the moment.\n-depth : Depth of the memory structure. Only valid choices are 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288.\n-bpp : Bits per pixel of the output. Uses basic thresholding to truncate image data.\n-color: Set to 0 for auto RGB output. Set to 1 for Grayscale.");
		printf("\n");
		printf("\nEX : img2mif tester.bmp 8 8192 2 1");
		printf("\n");
		return(1);
	}
	//Open file
	FILE *fi = fopen(argv[1], "r");

	if (fi == 0)
	{
		printf("\nCould not open file");
		return 1;
	}

	printf("\nFile: %s found and opened. ", argv[1]);

	filename = strtok(argv[1], ".");
	
	bitwidth = atoi(argv[2]);
	memdepth = atoi(argv[3]);
	bbp = atoi(argv[4]);
	convert2bw = atoi(argv[5]);

	//Scrub inputs. 
	if (convert2bw != 0 && convert2bw != 1)
	{
		printf("\nIncorrect flag for Color Set. Setting to RGB output.");
		convert2bw = 0;
	}

	if (bitwidth != 8 && bitwidth != 16 && bitwidth != 32 && bitwidth != 64 && bitwidth != 128)
	{
		printf("\nBit width for memory invalid. Only support for 8, 16, 32, 64, 128 at the moment.");
		return 1;
	}

	if (memdepth*bitwidth != 128 && memdepth*bitwidth != 256 && memdepth*bitwidth != 512 && memdepth*bitwidth != 1024 && memdepth*bitwidth != 2048 && memdepth*bitwidth != 4096 && memdepth*bitwidth != 8192 && memdepth*bitwidth != 16384 && memdepth*bitwidth != 32768 && memdepth*bitwidth != 65536 && memdepth*bitwidth != 131072 && memdepth*bitwidth != 262144 && memdepth*bitwidth != 524288)
	{
		printf("\nMemory depth is invalid. Only valid choices are 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288.");
		return 1;
	}

	//Load image into the stb_image.h library. 
	data = stbi_load_from_file(fi, &x, &y, &comp, convert2bw);

	fclose(fi);

	//Check to see if stb_image.h failed to load image. 
	if (data == NULL)
	{
		printf("\nFailed to load %s into the converter...",argv[1]);
		printf("\nFailure Reason: %s\n", stbi_failure_reason);
		return 1;
	}

	//Meta info about the image loaded. 
	printf("File is X: %d Y: %d Comp: %d.\n", x, y, comp);

	printf("Create %s.mif file with Width: %d and Depth: %d.\n", filename, bitwidth, memdepth);

	//Create output file. Same name as input image.
	FILE *fo = fopen(strcat(filename, ".mif"), "w");

	if (fo == 0)
	{
		printf("Could not create the file.\n");
		return 1;
	}

	//Print header infomation.
	fprintf(fo, "%s", mif_header0);

	fprintf(fo, "WIDTH=%d;\nDEPTH=%d;\n\nADDRESS_RADIX=UNS;\nDATA_RADIX=HEX;\n\nCONTENT BEGIN\n", bitwidth, memdepth);

	//Color
	if (convert2bw == 0)
	{
		printf("\nRGB IS NOT SUPPORTED YET!");
	}
	//Grayscale
	else
	{	
		//Only support for 8bit Grayscale output. 
		if (bbp > 8)
		{
			printf("\nSupport only for 8bit Grayscale MAX. Changing bits per pixel to 8.");
			bbp = 8;
		}
		//No truncation. Bitwidths the same.
		if (bitwidth == bbp)
		{
			if (memdepth < x*y)
			{
				printf("\nWARNING: Not enough memory. Memory Depth is %d. Need %d. Continuing...", memdepth, x*y);
			}
			for (i = 0; i < x*y && i < memdepth; i++)
			{
				fprintf(fo, "%d\t:\t%02X\t;\n", i, data[i]);
			}

			if (i < memdepth)
			{
				fprintf(fo, "[%d..%d]\t:\t00\t;\n", i + 1, memdepth);
			}
		}
		else if (bitwidth % bbp != 0)
		{
			printf("\nUneven pixels per memory location. Will leave uninitialized bits to 0.");
			printf("\nTruncating image from 8bit Grayscale to %dbit Grayscale...", bbp);
			printf("\nNOT SUPPORTED YET!");
		}
		//Truncate images through thresholding. 
		else
		{
			printf("\nTruncating image from 8bit Grayscale to %dbit Grayscale...", bbp);

			//Fine Max and Min values to set up threshold values. 
			max_value = INT_MIN;
			min_value = INT_MAX;

			for (i = 0; i < x*y; i++)
			{
				if (data[i] > max_value)
				{
					max_value = data[i];
				}
				if (data[i] < min_value)
				{
					min_value = data[i];
				}
			}

			printf("\nMax Value: %02X Min Value: %02X", max_value, min_value);

			//diff value is spacing between threshold levels. 
			diff = (max_value - min_value) / (pow(double (2),bbp));

			//Check to see if there is enough memory to load the entire image. If not throw a warning and fill as much as we can. 
			if (memdepth < (x*y) / (bitwidth / bbp))
			{
				printf("\nWARNING: Not enough memory. Memory Depth is %d. Need %d. Continuing...", memdepth, (x*y) / (bitwidth / bbp));
			}
			//1 bit per pixel. Super Thresholding!
			if (bbp == 1)
			{
				j = 0;
				k = 0;
				m = 0;
				for (i = 0; i < x*y; i = i + (bitwidth / bbp))
				{
					fprintf(fo, "%d\t:\t", k);
					bytebuilder = 0;
					for (j = 0; j < bitwidth / bbp; j++)
					{
						if (data[j + i] >(max_value - diff))
						{
							bytebuilder |= 0x80 >> j;
						}

						m++;
						if (m > 7)
						{
							fprintf(fo, "%02X", bytebuilder);
							bytebuilder = 0;
							m = 0;
						}
					}
					k++;

					fprintf(fo, "\t;\n");
				}
			}
			//2 bits per pixel.
			else if (bbp == 2)
			{
				j = 0;
				k = 0;
				m = 0;
				for (i = 0; i < x*y; i = i + (bitwidth / bbp))
				{
					fprintf(fo, "%d\t:\t", k);
					bytebuilder = 0;
					for (j = 0; j < bitwidth / bbp; j++)
					{
						if (data[j + i] >(max_value - diff))
						{
							bytebuilder |= 0xC0 >> (m * 2);
						}
						else if (data[j + i] >(max_value - (2 *diff)))
						{
							bytebuilder |= 0x80 >> (m * 2);
						}
						else if (data[j + i] >(max_value - (3 * diff)))
						{
							bytebuilder |= 0x40 >> (m * 2);
						}

						m++;
						if (m > 3)
						{
							fprintf(fo, "%02X", bytebuilder);
							bytebuilder = 0;
							m = 0;
						}
					}
					k++;

					fprintf(fo, "\t;\n");
				}
			}
			//4 bits per pixel.
			else if (bbp == 4)
			{
				printf("\nBits per pixel = 4 untested!");
				j = 0;
				k = 0;
				m = 0;
				for (i = 0; i < x*y; i = i + (bitwidth / bbp))
				{
					fprintf(fo, "%d\t:\t", k);
					bytebuilder = 0;
					for (j = 0; j < bitwidth / bbp; j++)
					{
						if (data[j + i] > (max_value - diff))
						{
							bytebuilder |= 0xF0 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (2 * diff)))
						{
							bytebuilder |= 0xE0 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (3 * diff)))
						{
							bytebuilder |= 0xD0 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (4 * diff)))
						{
							bytebuilder |= 0xC0 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (5 * diff)))
						{
							bytebuilder |= 0xB0 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (6 * diff)))
						{
							bytebuilder |= 0xA0 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (7 * diff)))
						{
							bytebuilder |= 0x90 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (8 * diff)))
						{
							bytebuilder |= 0x80 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (9 * diff)))
						{
							bytebuilder |= 0x70 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (10 * diff)))
						{
							bytebuilder |= 0x60 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (11 * diff)))
						{
							bytebuilder |= 0x50 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (12 * diff)))
						{
							bytebuilder |= 0x40 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (13 * diff)))
						{
							bytebuilder |= 0x30 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (14 * diff)))
						{
							bytebuilder |= 0x20 >> (m * 4);
						}
						else if (data[j + i] > (max_value - (15 * diff)))
						{
							bytebuilder |= 0x10 >> (m * 4);
						}

						m++;
						if (m > 1)
						{
							fprintf(fo, "%02X", bytebuilder);
							bytebuilder = 0;
							m = 0;
						}
					}
					k++;

					fprintf(fo, "\t;\n");
				}
			}
			//8 bits per pixel. This happens if there are multiple 8bit pixels per memory address. 
			else if (bbp == 8)
			{
				printf("\nBits per pixel = 8 untested!");
				j = 0;
				k = 0;
				m = 0;
				for (i = 0; i < x*y; i = i + (bitwidth / bbp))
				{
					fprintf(fo, "%d\t:\t", k);
					for (j = 0; j < bitwidth / bbp; j++)
					{
						fprintf(fo, "%02X", data[i+j]);
					}
					k++;

					fprintf(fo, "\t;\n");
				}
			}
			else
			{
				printf("\nHow did you get here?!");
				return 1;
			}

			//If left over space in RAM fill with zeros so Altera does not complain. 
			if (k < memdepth)
			{
				fprintf(fo, "[%d..%d]\t:\t", k, memdepth-1);
				for (i = 0; i < (bitwidth / 4); i++)
				{
					fprintf(fo, "0");
				}
				fprintf(fo, "\t;\n");
			}
		}
	}

	//Finish that .mif file!
	fprintf(fo, "END;\n");

	//Free up memory. Garbage Collecting and stuff.
	stbi_image_free(data);

	//close .mif file.
	fclose(fo);

	return 0;
}


