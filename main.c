#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/src/nanosvg.h"
#include "nanosvg/src/nanosvgrast.h"

#define DEFAULT_SCALE 0.4

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
	
	int print_raw = 0;
	float scale = DEFAULT_SCALE;
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-v") == 0)
		{
			print_raw = 1;
		}
		if(strcmp(argv[i], "-s") == 0)
		{
			if(argc <= i + 1)
			{
				fprintf(stderr, "main: -s: provide a scale as a decimal number\n");
				exit(4);
			}
			if(sscanf(argv[i+1], "%f", &scale) != 1)
			{
				fprintf(stderr, "main: -s: '%s': invalid scale\n", argv[i+1]);
				exit(4);
			}
			scale *= DEFAULT_SCALE;
		}
	}


	image = nsvgParseFromFile(fname, "px", 96);
	if(image == NULL)
	{
		fprintf(stderr, "Not found: %s\n", fname);
		exit(4);
	}
	int image_height = (int)(image->height + 0.5);	
	int image_width = (int)(image->width + 0.5);	
	
	int out_width = (int)(image_width * scale);
	int out_height = (int)(image_height * scale);

	unsigned char *img_buffer = malloc(out_width * out_height * 4);
	if(img_buffer == NULL)
	{
		fprintf(stderr, "main: failed to alloc\n");
		exit(4);
	}
	unsigned char *bitmap = malloc(out_width * out_height * 4);
	if(bitmap == NULL)
	{
		fprintf(stderr, "main: failed to alloc\n");
		exit(4);
	}
	

	NSVGrasterizer *rasterizer = nsvgCreateRasterizer();

	/* Get the highest and lowest pixel */
	int highest_pixel = -1;
	int lowest_pixel = -1;
	nsvgRasterize(rasterizer, image, 0, 0, scale, img_buffer, out_width, out_height, out_width * 4);
	for(int i = 0; i < out_height; i++)
	{
		for(int j = 0; j < out_width * 4; j++)
		{
			if(img_buffer[i * out_width * 4 + j] != 0)
			{
				if(highest_pixel == -1)
				{
					highest_pixel = i;
				}
				lowest_pixel = i;
			}
		}
	}
	if(print_raw)
	{
		int stroke_num = 1;
		
		struct NSVGshape *temp_shape;
		struct NSVGshape **current_shape;
		current_shape = &(image->shapes);
		while(*current_shape != NULL)
		{
			temp_shape = (*current_shape)->next;
			(*current_shape)->next = NULL;

			memset(bitmap, 0, out_height * out_width);
			nsvgRasterize(rasterizer, image, 0, 0, scale, img_buffer, out_width, out_height, out_width * 4);

			for(int i = highest_pixel; i < lowest_pixel; i++)
			{
				for(int j = 0; j < out_width; j++)
				{
					unsigned char pixel[4];
					pixel[0] = img_buffer[i * (int)out_width * 4 + 4 * j];
					pixel[1] = img_buffer[i * (int)out_width * 4 + 4 * j + 1];
					pixel[2] = img_buffer[i * (int)out_width * 4 + 4 * j + 2];
					pixel[3] = img_buffer[i * (int)out_width * 4 + 4 * j + 3];
				
					if(pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0 || pixel[3] != 0)
					{
						bitmap[i * out_width + j] = 1;
					}
					else
					{
						bitmap[i * out_width + j] = 0;
					}
				}
			}

			float start_x = (*current_shape)->paths->pts[0] * scale;
			float start_y = (*current_shape)->paths->pts[1] * scale;
			bitmap[(int)start_y * out_width + (int)(start_x)] = 2;
		
			printf("Stroke %d\n\n", stroke_num);	
			for(int i = highest_pixel; i < lowest_pixel; i++)
			{
				for(int j = 0; j < out_width; j++)
				{
					if(bitmap[i * out_width + j] == 1)
					{
						printf("#");
					}
					else if(bitmap[i * out_width + j] == 2)
					{
						printf("%d", stroke_num);
						j += snprintf(NULL, 0, "%d", stroke_num);
						stroke_num++;
					}
					else
					{
						printf(" ");
					}
				}
				printf("\n");
			}
			printf("\n");
			(*current_shape)->next = temp_shape;
			current_shape = &((*current_shape)->next);
		}
		nsvgDeleteRasterizer(rasterizer);
		free(img_buffer);
		free(bitmap);
		return 0;
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

		memset(bitmap, 0, out_height * out_width);
		nsvgRasterize(rasterizer, &stroke, 0, 0, scale, img_buffer, out_height, out_width, out_width * 4);

		for(int i = 0; i < out_height; i++)
		{
			for(int j = 0; j < out_width; j++)
			{
				unsigned char pixel[4];
				pixel[0] = img_buffer[i * (int)out_width * 4 + 4 * j];
				pixel[1] = img_buffer[i * (int)out_width * 4 + 4 * j + 1];
				pixel[2] = img_buffer[i * (int)out_width * 4 + 4 * j + 2];
				pixel[3] = img_buffer[i * (int)out_width * 4 + 4 * j + 3];
				
				if(pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0 || pixel[3] != 0)
				{
					bitmap[i * out_width + j] = 1;
				}
				else
				{
					bitmap[i * out_width + j] = 0;
				}
			}
		}

		float start_x = stroke_shape.paths->pts[0] * scale;
		float start_y = stroke_shape.paths->pts[1] * scale;
		bitmap[(int)start_y * out_width + (int)(start_x)] = 2;

		int not_done = 1;
		while(not_done)
		{
			/* Propagate flooding pixels */
			not_done = 0;
			for(int i = 1; i < out_height-1; i++)
			{
				for(int j = 1; j < out_width-1; j++)
				{
					if(bitmap[i * out_width + j] == 2)
					{

						if(bitmap[(i - 1) * out_width + j] == 1)
						{
							not_done = 1;
							bitmap[(i - 1) * out_width + j] = 3;
						}
						if(bitmap[(i + 1) * out_width + j] == 1)
						{
							not_done = 1;
							bitmap[(i + 1) * out_width + j] = 3;
						}
						if(bitmap[(i) * out_width + j - 1] == 1)
						{
							not_done = 1;
							bitmap[(i - 1) * out_width + j - 1] = 3;
						}
						if(bitmap[(i) * out_width + j + 1] == 1)
						{
							not_done = 1;
							bitmap[(i) * out_width + j + 1] = 3;
						}
						if(bitmap[(i - 1) * out_width + j - 1] == 1)
						{
							not_done = 1;
							bitmap[(i - 1) * out_width + j - 1] = 3;
						}
						if(bitmap[(i + 1) * out_width + j - 1] == 1)
						{
							not_done = 1;
							bitmap[(i + 1) * out_width + j - 1] = 3;
						}
						if(bitmap[(i - 1) * out_width + j + 1] == 1)
						{
							not_done = 1;
							bitmap[(i - 1) * out_width + j + 1] = 3;
						}
						if(bitmap[(i + 1) * out_width + j + 1] == 1)
						{
							not_done = 1;
							bitmap[(i + 1) * out_width + j + 1] = 3;
						}
					}
				}
			}
			for(int i = 0; i < out_height; i++)
			{
				for(int j = 0; j < out_width; j++)
				{
					if(bitmap[i * out_width + j] == 3)
					{
						bitmap[i * out_width + j] = 2;
						printf("\033[%d;%dH#", i - highest_pixel + 1, j);
					}
				}
			}

			fflush(stdout);
			/* Goodnight */
			usleep(15000 / scale);

		}

	}
	nsvgDeleteRasterizer(rasterizer);
	free(img_buffer);
	free(bitmap);
	
	printf("\033[%d;1H", lowest_pixel);

	return 0;
}
