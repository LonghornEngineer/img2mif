###img2mif
***
**Image to .mif file converter for use in Altera Quartus FPGA development.**
	
Converts images to Altera Memory Initialization File (.mif) format.
	
How to use:
	Pass the image, width in bits of the memory, the depth of the memory, how many bits per pixel for the memory, and color format. Color format is either 0 for Grayscale or 1 for RGB.
	RGB color will set the RGB elements of a pixel as consecutive memory locations. 

		EX: img2mif tester.bmp 8 8192 2 1

If the image is to large for the memory it will fill up as much of the memory as possible then give a warning.
If the image does not take up all the space the other locations will be filled with 0.

So far RGB is currently not implemented and a couple sections are untested but should work. Seems bitmaps are the best choice as file inputs. 
Uses the [stb image library] (https://github.com/nothings/stb) for opening up images and getting the pixel data. Included is a copy of stb_image.h that I used for building.

Author: Parker Dillmann
Website: [Longhorn Engineer] (www.longhornengineer.com)
***
