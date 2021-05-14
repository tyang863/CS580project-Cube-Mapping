/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include	"math.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */
  if (u > 1) { u = 1; }
  if (u < 0) { u = 0; }
  if (v > 1) { v = 1; }
  if (v < 0) { v = 0; }

  float u_scaled = (xs - 1) * u, v_scaled = (ys - 1) * v;
  
  int x_lower = (int)floor(u_scaled), x_upper = (int)ceil(u_scaled), y_lower = (int)floor(v_scaled), y_upper = (int)ceil(v_scaled);

  float s = u_scaled - x_lower, t = v_scaled - y_lower;

  GzColor color_A, color_B, color_C, color_D;
  
  color_A[RED] = image[y_lower * xs + x_lower][RED];
  color_A[GREEN] = image[y_lower * xs + x_lower][GREEN];
  color_A[BLUE] = image[y_lower * xs + x_lower][BLUE];

  color_B[RED] = image[y_lower * xs + x_upper][RED];
  color_B[GREEN] = image[y_lower * xs + x_upper][GREEN];
  color_B[BLUE] = image[y_lower * xs + x_upper][BLUE];

  color_C[RED] = image[y_upper * xs + x_upper][RED];
  color_C[GREEN] = image[y_upper * xs + x_upper][GREEN];
  color_C[BLUE] = image[y_upper * xs + x_upper][BLUE];

  color_D[RED] = image[y_upper * xs + x_lower][RED];
  color_D[GREEN] = image[y_upper * xs + x_lower][GREEN];
  color_D[BLUE] = image[y_upper * xs + x_lower][BLUE];

  color[RED] = s * t * color_C[RED] + (1 - s) * t * color_D[RED] + s * (1 - t) * color_B[RED] + (1 - s) * (1 - t) * color_A[RED];
  color[GREEN] = s * t * color_C[GREEN] + (1 - s) * t * color_D[GREEN] + s * (1 - t) * color_B[GREEN] + (1 - s) * (1 - t) * color_A[GREEN];
  color[BLUE] = s * t * color_C[BLUE] + (1 - s) * t * color_D[BLUE] + s * (1 - t) * color_B[BLUE] + (1 - s) * (1 - t) * color_A[BLUE];
  
	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	float u_scaled = 100 * u, v_scaled = 100 * v;

	int x_lower = (int)floor(u_scaled), y_lower = (int)floor(v_scaled);

	if (x_lower % 2 == y_lower % 2)
	{
		color[RED] = 0;
		color[GREEN] = 0;
		color[BLUE] = 0;
	}
	else
	{
		color[RED] = 0.5;
		color[GREEN] = 0.5;
		color[BLUE] = 0.5;
	}

	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

