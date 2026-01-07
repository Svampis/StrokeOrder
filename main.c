#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/src/nanosvg.h"
#include "nanosvg/src/nanosvgrast.h"

#define SCALE 0.4

int main(int argc, char **argv)
{
	struct NSVGimage *image;
	struct NSVGshape *shape;


	if(argc < 2)
	{
		fprintf(stderr, "Provide a kanji\n");
		exit(4);
	}

	char fname[256];
	snprintf(fname, sizeof(fname), "/usr/share/strokeorder/kanji/%s", argv[1]);
	image = nsvgParseFromFile(fname, "px", 96);
	if(image == NULL)
	{
		fprintf(stderr, "Not found: %s\n", fname);
		exit(4);
	}
	unsigned char *img_buffer = malloc(image->height * image->width * 4);
	if(img_buffer == NULL)
	{
		fprintf(stderr, "main: failed to alloc\n");
		exit(4);
	}
	unsigned char *bitmap = malloc(image->height * image->width);
	if(bitmap == NULL)
	{
		fprintf(stderr, "main: failed to alloc\n");
		exit(4);
	}
	
	int image_height = (int)(image->height + 0.5);	
	int image_width = (int)(image->width + 0.5);	

	NSVGrasterizer *rasterizer = nsvgCreateRasterizer();

	/* Get the highest and lowest pixel */
	int highest_pixel = -1;
	int lowest_pixel = -1;
	nsvgRasterize(rasterizer, image, 0, 0, SCALE, img_buffer, image_height, image_width, image->width * 4);
	for(int i = 0; i < image_height; i++)
	{
		for(int j = 0; j < image_width * 4; j++)
		{
			if(img_buffer[i * image_width * 4 + j] != 0)
			{
				if(highest_pixel == -1)
				{
					highest_pixel = i;
				}
				lowest_pixel = i;
			}
		}
	}
	printf("\033[2J\033[H");
	for(shape = image->shapes; shape != NULL; shape = shape->next)
	{
		struct NSVGimage stroke;
		struct NSVGshape stroke_shape;
		

		memcpy(&stroke_shape, shape, sizeof(stroke_shape));
		stroke_shape.next = NULL;
		
		stroke.height = image->height;
		stroke.width = image->width;
		stroke.shapes = &stroke_shape;

		memset(bitmap, 0, image_height * image_width);
		nsvgRasterize(rasterizer, &stroke, 0, 0, SCALE, img_buffer, image_height, image_width, image->width * 4);

		for(int i = 0; i < image_height; i++)
		{
			for(int j = 0; j < image_width; j++)
			{
				unsigned char pixel[4];
				pixel[0] = img_buffer[i * (int)image_height * 4 + 4 * j];
				pixel[1] = img_buffer[i * (int)image_height * 4 + 4 * j + 1];
				pixel[2] = img_buffer[i * (int)image_height * 4 + 4 * j + 2];
				pixel[3] = img_buffer[i * (int)image_height * 4 + 4 * j + 3];
				
				if(pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0 || pixel[3] != 0)
				{
					bitmap[i * image_width + j] = 1;
				}
				else
				{
					bitmap[i * image_width + j] = 0;
				}
			}
		}

		float start_x = stroke_shape.paths->pts[0] * SCALE;
		float start_y = stroke_shape.paths->pts[1] * SCALE;
		bitmap[(int)start_y * image_width + (int)(start_x)] = 2;

		int not_done = 1;
		while(not_done)
		{
			/* Propagate flooding pixels */
			not_done = 0;
			for(int i = 1; i < image_height-1; i++)
			{
				for(int j = 1; j < image_width-1; j++)
				{
					if(bitmap[i * image_width + j] == 2)
					{

						if(bitmap[(i - 1) * image_width + j] == 1)
						{
							not_done = 1;
							bitmap[(i - 1) * image_width + j] = 3;
						}
						if(bitmap[(i + 1) * image_width + j] == 1)
						{
							not_done = 1;
							bitmap[(i + 1) * image_width + j] = 3;
						}
						if(bitmap[(i) * image_width + j - 1] == 1)
						{
							not_done = 1;
							bitmap[(i - 1) * image_width + j - 1] = 3;
						}
						if(bitmap[(i) * image_width + j + 1] == 1)
						{
							not_done = 1;
							bitmap[(i) * image_width + j + 1] = 3;
						}
						if(bitmap[(i - 1) * image_width + j - 1] == 1)
						{
							not_done = 1;
							bitmap[(i - 1) * image_width + j - 1] = 3;
						}
						if(bitmap[(i + 1) * image_width + j - 1] == 1)
						{
							not_done = 1;
							bitmap[(i + 1) * image_width + j - 1] = 3;
						}
						if(bitmap[(i - 1) * image_width + j + 1] == 1)
						{
							not_done = 1;
							bitmap[(i - 1) * image_width + j + 1] = 3;
						}
						if(bitmap[(i + 1) * image_width + j + 1] == 1)
						{
							not_done = 1;
							bitmap[(i + 1) * image_width + j + 1] = 3;
						}
					}
				}
			}
			for(int i = 0; i < image_height; i++)
			{
				for(int j = 0; j < image_width; j++)
				{
					if(bitmap[i * image_width + j] == 3)
					{
						bitmap[i * image_width + j] = 2;
						printf("\033[%d;%dH#", i - highest_pixel + 1, j);
					}
				}
			}

			fflush(stdout);
			/* Goodnight */
			usleep(15000 / SCALE);

		}

	}
	nsvgDeleteRasterizer(rasterizer);
	free(img_buffer);
	free(bitmap);
	
	printf("\033[%d;1H", lowest_pixel);

	return 0;
}
