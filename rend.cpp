/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include    <string>

#define PI (float) 3.14159265358979323846

#define	MAX_INTENSITY 4095
#define	MIN_INTENSITY 0

#define DEFAULT_R 3000
#define DEFAULT_G 3000
#define DEFAULT_B 3000
#define DEFAULT_A 0
#define DEFAULT_Z INT_MAX

#define LEFT 0
#define RIGHT 1
#define TOP 2
#define BOTTOM 3

#define W 3

inline float ang2rad(float ang) { return ang * PI / 180; }

void CrossProduct(GzCoord target, GzCoord vec1, GzCoord vec2);
void SortVertex(GzCoord *coord, GzCoord *norm, int edgeOrientation[]);
float FindMax(GzCoord *coord, int axis);
float FindMin(GzCoord *coord, int axis);
void shade(GzCoord norm, GzCoord color);

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
/* HW 3.1
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
*/
	degree = ang2rad(degree);

	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = cos(degree);
	mat[1][2] = -sin(degree);
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = sin(degree);
	mat[2][2] = cos(degree);
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
/* HW 3.2
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
*/
	degree = ang2rad(degree);

	mat[0][0] = cos(degree);
	mat[0][1] = 0;
	mat[0][2] = sin(degree);
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = -sin(degree);
	mat[2][1] = 0;
	mat[2][2] = cos(degree);
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
/* HW 3.3
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
*/
	degree = ang2rad(degree);

	mat[0][0] = cos(degree);
	mat[0][1] = -sin(degree);
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = sin(degree);
	mat[1][1] = cos(degree);
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
/* HW 3.4
// Create translation matrix
// Pass back the matrix using mat value
*/
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = translate[0];

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = translate[1];

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = translate[2];

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
/* HW 3.5
// Create scaling matrix
// Pass back the matrix using mat value
*/
	mat[0][0] = scale[0];
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = scale[1];
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = scale[2];
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
	/* HW1.1 create a framebuffer for MS Windows display:
	 -- set display resolution
	 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
	 -- allocate memory for pixel buffer
	 */
	if (xRes > MAXXRES) { xRes = MAXXRES; }
	if (yRes > MAXYRES) { yRes = MAXYRES; }
	if (xRes < 0) { xRes = 0; }
	if (yRes < 0) { yRes = 0; }
	this->xres = xRes;
	this->yres = yRes;
	this->pixelbuffer = new GzPixel[xRes * yRes];
	this->framebuffer = (char*)malloc(3 * sizeof(char) * xRes * yRes);

	this->num_triangle = 0;

	/* HW 3.6
	- setup Xsp and anything only done once
	- init default camera
	*/
	//Setup Xsp
	Xsp[0][0] = (float)xRes / 2;
	Xsp[0][1] = 0;
	Xsp[0][2] = 0;
	Xsp[0][3] = (float)xRes / 2;

	Xsp[1][0] = 0;
	Xsp[1][1] = (float)yRes * (-1) / 2;
	Xsp[1][2] = 0;
	Xsp[1][3] = (float)yRes / 2;

	Xsp[2][0] = 0;
	Xsp[2][1] = 0;
	Xsp[2][2] = INT_MAX;
	Xsp[2][3] = 0;

	Xsp[3][0] = 0;
	Xsp[3][1] = 0;
	Xsp[3][2] = 0;
	Xsp[3][3] = 1;

	//Setup default camera
	m_camera.FOV = DEFAULT_FOV;

	m_camera.position[X] = DEFAULT_IM_X;
	m_camera.position[Y] = DEFAULT_IM_Y;
	m_camera.position[Z] = DEFAULT_IM_Z;

	m_camera.lookat[X] = 0;
	m_camera.lookat[Y] = 0;
	m_camera.lookat[Z] = 0;

	m_camera.worldup[X] = 0;
	m_camera.worldup[Y] = 1;
	m_camera.worldup[Z] = 0;

	numlights = 0;
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	delete[] pixelbuffer;
	delete[] framebuffer;
}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	for (int j = 0; j < yres; j++)
	{
		for (int i = 0; i < xres; i++)
		{
			GzPut(i, j, DEFAULT_R, DEFAULT_G, DEFAULT_B, DEFAULT_A, DEFAULT_Z);

			framebuffer[ARRAY(i, j) * 3 + 0] = (char)((DEFAULT_B >> 4) & 0xFF);
			framebuffer[ARRAY(i, j) * 3 + 1] = (char)((DEFAULT_G >> 4) & 0xFF);
			framebuffer[ARRAY(i, j) * 3 + 2] = (char)((DEFAULT_R >> 4) & 0xFF);
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
	/* HW 3.7
	- setup for start of each frame - init frame buffer color,alpha,z
	- compute Xiw and projection xform Xpi from camera definition
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw
	- now stack contains Xsw and app can push model Xforms when needed
	*/
	//Setup camera Xpi
	float d_re = tan(ang2rad(m_camera.FOV) / 2);
	m_camera.Xpi[0][0] = 1;
	m_camera.Xpi[0][1] = 0;
	m_camera.Xpi[0][2] = 0;
	m_camera.Xpi[0][3] = 0;

	m_camera.Xpi[1][0] = 0;
	m_camera.Xpi[1][1] = 1;
	m_camera.Xpi[1][2] = 0;
	m_camera.Xpi[1][3] = 0;

	m_camera.Xpi[2][0] = 0;
	m_camera.Xpi[2][1] = 0;
	m_camera.Xpi[2][2] = d_re;
	m_camera.Xpi[2][3] = 0;

	m_camera.Xpi[3][0] = 0;
	m_camera.Xpi[3][1] = 0;
	m_camera.Xpi[3][2] = d_re;
	m_camera.Xpi[3][3] = 1;

	//Setup camera Xiw
	GzCoord X_iw, Y_iw, Z_iw;

	GzCoord cl;
	cl[X] = m_camera.lookat[X] - m_camera.position[X];
	cl[Y] = m_camera.lookat[Y] - m_camera.position[Y];
	cl[Z] = m_camera.lookat[Z] - m_camera.position[Z];
	float cl_norm = sqrt(cl[X] * cl[X] + cl[Y] * cl[Y] + cl[Z] * cl[Z]);
	Z_iw[X] = cl[X] / cl_norm;
	Z_iw[Y] = cl[Y] / cl_norm;
	Z_iw[Z] = cl[Z] / cl_norm;

	float dot_product_result = m_camera.worldup[X] * Z_iw[X] + m_camera.worldup[Y] * Z_iw[Y] + m_camera.worldup[Z] * Z_iw[Z];
	GzCoord up_;
	up_[X] = m_camera.worldup[X] - dot_product_result * Z_iw[X];
	up_[Y] = m_camera.worldup[Y] - dot_product_result * Z_iw[Y];
	up_[Z] = m_camera.worldup[Z] - dot_product_result * Z_iw[Z];
	float up_norm = sqrt(up_[X] * up_[X] + up_[Y] * up_[Y] + up_[Z] * up_[Z]);
	Y_iw[X] = up_[X] / up_norm;
	Y_iw[Y] = up_[Y] / up_norm;
	Y_iw[Z] = up_[Z] / up_norm;

	CrossProduct(X_iw, Y_iw, Z_iw);

	m_camera.Xiw[0][0] = X_iw[X];
	m_camera.Xiw[0][1] = X_iw[Y];
	m_camera.Xiw[0][2] = X_iw[Z];
	m_camera.Xiw[0][3] = -(X_iw[X] * m_camera.position[X] + X_iw[Y] * m_camera.position[Y] + X_iw[Z] * m_camera.position[Z]);

	m_camera.Xiw[1][0] = Y_iw[X];
	m_camera.Xiw[1][1] = Y_iw[Y];
	m_camera.Xiw[1][2] = Y_iw[Z];
	m_camera.Xiw[1][3] = -(Y_iw[X] * m_camera.position[X] + Y_iw[Y] * m_camera.position[Y] + Y_iw[Z] * m_camera.position[Z]);

	m_camera.Xiw[2][0] = Z_iw[X];
	m_camera.Xiw[2][1] = Z_iw[Y];
	m_camera.Xiw[2][2] = Z_iw[Z];
	m_camera.Xiw[2][3] = -(Z_iw[X] * m_camera.position[X] + Z_iw[Y] * m_camera.position[Y] + Z_iw[Z] * m_camera.position[Z]);

	m_camera.Xiw[3][0] = 0;
	m_camera.Xiw[3][1] = 0;
	m_camera.Xiw[3][2] = 0;
	m_camera.Xiw[3][3] = 1;

	Xwi[0][0] = X_iw[X];
	Xwi[0][1] = Y_iw[X];
	Xwi[0][2] = Z_iw[X];
	Xwi[0][3] = 0;//m_camera.position[X];

	Xwi[1][0] = X_iw[Y];
	Xwi[1][1] = Z_iw[Y];
	Xwi[1][2] = Y_iw[Y];
	Xwi[1][3] = 0;// m_camera.position[Y];

	Xwi[2][0] = X_iw[Z];
	Xwi[2][1] = Y_iw[Z];
	Xwi[2][2] = Z_iw[Z];
	Xwi[2][3] = 0;// m_camera.position[Z];

	Xwi[3][0] = 0;
	Xwi[3][1] = 0;
	Xwi[3][2] = 0;
	Xwi[3][3] = 1;

	//Push matrixes into the stack
	matlevel = -1;
	GzPushMatrix(Xsp);
	GzPushMatrix(m_camera.Xpi);
	GzPushMatrix(m_camera.Xiw);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Xsi[i][j] = 0;
			for (int k = 0; k < 4; k++)
			{
				Xsi[i][j] += Xsp[i][k] * m_camera.Xpi[k][j];
			}
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
/* HW 3.8 
/*- overwrite renderer camera structure with new camera definition
*/
	m_camera.FOV = camera.FOV;
	for (int i = 0; i < 3; i++)
	{
		m_camera.position[i] = camera.position[i];
		m_camera.lookat[i] = camera.lookat[i];
		m_camera.worldup[i] = camera.worldup[i];
	}

	return GZ_SUCCESS;	
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
	/* HW 3.9
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	if (matlevel == -1)
	{
		matlevel++;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Ximage[0][i][j] = matrix[i][j];

				Xnorm[0][i][j] = (i == j) ? 1 : 0;
				Xim[0][i][j] = (i == j) ? 1 : 0;
			}
		}
		return GZ_SUCCESS;
	}

	GzMatrix X_top_image;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			X_top_image[i][j] = Ximage[matlevel][i][j];
		}
	}

	if (matlevel + 1 >= MATLEVELS)
	{
		return GZ_FAILURE;
	}
	else
	{
		matlevel++;
	}

	if (matlevel == 1)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Ximage[1][i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					Ximage[1][i][j] += X_top_image[i][k] * matrix[k][j];
				}

				Xnorm[1][i][j] = (i == j) ? 1 : 0;
				Xim[1][i][j] = (i == j) ? 1 : 0;
			}
		}
	}
	else
	{
		GzMatrix X_top_norm, X_top_im;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				X_top_norm[i][j] = Xnorm[matlevel - 1][i][j];
				X_top_im[i][j] = Xim[matlevel - 1][i][j];
			}
		}

		GzMatrix matrix_pure;
		float scale_factor = sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				matrix_pure[i][j] = matrix[i][j] / scale_factor;
			}

			matrix_pure[i][3] = 0;
			matrix_pure[3][i] = 0;
		}
		matrix_pure[3][3] = 1;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Ximage[matlevel][i][j] = 0;
				Xnorm[matlevel][i][j] = 0;
				Xim[matlevel][i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					Ximage[matlevel][i][j] += X_top_image[i][k] * matrix[k][j];
					Xnorm[matlevel][i][j] += X_top_norm[i][k] * matrix_pure[k][j];
					Xim[matlevel][i][j] += X_top_im[i][k] * matrix[k][j];
				}
			}
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (matlevel - 1 < 0)
	{
		return GZ_FAILURE;
	}
	else
	{
		matlevel--;
	}

	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	if (r < MIN_INTENSITY) { r = MIN_INTENSITY; }
	if (r > MAX_INTENSITY) { r = MAX_INTENSITY; }
	if (g < MIN_INTENSITY) { g = MIN_INTENSITY; }
	if (g > MAX_INTENSITY) { g = MAX_INTENSITY; }
	if (b < MIN_INTENSITY) { b = MIN_INTENSITY; }
	if (b > MAX_INTENSITY) { b = MAX_INTENSITY; }
	if (a < MIN_INTENSITY) { a = MIN_INTENSITY; }
	if (a > MAX_INTENSITY) { a = MAX_INTENSITY; }

	if (i >= 0 && i < xres && j >= 0 && j < yres)
	{
		pixelbuffer[ARRAY(i, j)].red = r;
		pixelbuffer[ARRAY(i, j)].green = g;
		pixelbuffer[ARRAY(i, j)].blue = b;
		pixelbuffer[ARRAY(i, j)].alpha = a;
		pixelbuffer[ARRAY(i, j)].z = z;
	}

	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres)
	{
		if (r != NULL) { *r = pixelbuffer[ARRAY(i, j)].red; }
		if (g != NULL) { *g = pixelbuffer[ARRAY(i, j)].green; }
		if (b != NULL) { *b = pixelbuffer[ARRAY(i, j)].blue; }
		if (a != NULL) { *a = pixelbuffer[ARRAY(i, j)].alpha; }
		if (z != NULL) { *z = pixelbuffer[ARRAY(i, j)].z; }
	}

	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\r", xres, yres);

	char rgb[3];
	for (int j = 0; j < yres; j++)
	{
		for (int i = 0; i < xres; i++)
		{
			rgb[0] = (pixelbuffer[ARRAY(i, j)].red >> 4) & 0xFF;
			rgb[1] = (pixelbuffer[ARRAY(i, j)].green >> 4) & 0xFF;
			rgb[2] = (pixelbuffer[ARRAY(i, j)].blue >> 4) & 0xFF;
			fwrite(rgb, sizeof(char), 3, outfile);
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	for (int j = 0; j < yres; j++)
	{
		for (int i = 0; i < xres; i++)
		{
			framebuffer[ARRAY(i, j) * 3 + 0] = (pixelbuffer[ARRAY(i, j)].blue >> 4) & 0xFF;
			framebuffer[ARRAY(i, j) * 3 + 1] = (pixelbuffer[ARRAY(i, j)].green >> 4) & 0xFF;
			framebuffer[ARRAY(i, j) * 3 + 2] = (pixelbuffer[ARRAY(i, j)].red >> 4) & 0xFF;
		}
	}

	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList) 
{
/* HW 2.1
-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
-- In later homeworks set shaders, interpolaters, texture maps, and lights
*/

/*
- GzPutAttribute() must accept the following tokens/values:

- GZ_RGB_COLOR					GzColor		default flat-shade color
- GZ_INTERPOLATE				int			shader interpolation mode
- GZ_DIRECTIONAL_LIGHT			GzLight
- GZ_AMBIENT_LIGHT            	GzLight		(ignore direction)
- GZ_AMBIENT_COEFFICIENT		GzColor		Ka reflectance
- GZ_DIFFUSE_COEFFICIENT		GzColor		Kd reflectance
- GZ_SPECULAR_COEFFICIENT       GzColor		Ks coef's
- GZ_DISTRIBUTION_COEFFICIENT   float		spec power
*/
	for (int i = 0; i < numAttributes; i++)
	{
		switch (nameList[i])
		{
			case GZ_RGB_COLOR:
			{
				GzColor *color = (GzColor *)valueList[i];
				flatcolor[RED] = (*color)[RED];
				flatcolor[GREEN] = (*color)[GREEN];
				flatcolor[BLUE] = (*color)[BLUE];

				break;
			}
			case GZ_INTERPOLATE:
			{
				int *interpStyle = (int *)valueList[i];
				interp_mode = *interpStyle;

				break;
			}
			case GZ_DIRECTIONAL_LIGHT:
			{
				if (numlights + 1 > MAX_LIGHTS)
				{
					return GZ_FAILURE;
				}
				else
				{
					numlights++;
				}

				lights[numlights - 1] = *((GzLight *)valueList[i]);

				break;
			}
			case GZ_AMBIENT_LIGHT:
			{
				ambientlight = *((GzLight *)valueList[i]);

				break;
			}
			case GZ_AMBIENT_COEFFICIENT:
			{
				GzColor *color = (GzColor *)valueList[i];
				Ka[RED] = (*color)[RED];
				Ka[GREEN] = (*color)[GREEN];
				Ka[BLUE] = (*color)[BLUE];

				break;
			}
			case GZ_DIFFUSE_COEFFICIENT:
			{
				GzColor *color = (GzColor *)valueList[i];
				Kd[RED] = (*color)[RED];
				Kd[GREEN] = (*color)[GREEN];
				Kd[BLUE] = (*color)[BLUE];

				break;
			}
			case GZ_SPECULAR_COEFFICIENT:
			{
				GzColor *color = (GzColor *)valueList[i];
				Ks[RED] = (*color)[RED];
				Ks[GREEN] = (*color)[GREEN];
				Ks[BLUE] = (*color)[BLUE];

				break;
			}
			case GZ_DISTRIBUTION_COEFFICIENT:
			{
				float *specpower = (float *)valueList[i];
				spec = *specpower;

				break;
			}
			case GZ_TEXTURE_MAP:
			{
				tex_fun = (GzTexture)valueList[i];

				break;
			}
			default:
				return GZ_FAILURE;
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int triangle_index, int mode)
{
/**********************************************************************************************************************/
	GzCoord coord[3], norm[3];

	//Deal with the triangle_index-th triangle
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			coord[i][j] = triangles[triangle_index][i][j];
			norm[i][j] = triangles[triangle_index][i][j + 3];
		}
	}
/**********************************************************************************************************************/
	//Transfer the triangle from image space to screen space using Xsi

	float coord_homo[3][4];
	for (int i = 0; i < 3; i++)
	{
		coord_homo[i][X] = coord[i][X];
		coord_homo[i][Y] = coord[i][Y];
		coord_homo[i][Z] = coord[i][Z];
		coord_homo[i][W] = 1;
	}

	float coord_homo_transed[3][4];
	for (int i = 0; i < 3; i++)
	{
		coord_homo_transed[i][X] = 0;
		for (int j = 0; j < 4; j++)
		{
			coord_homo_transed[i][X] += Xsi[0][j] * coord_homo[i][j];
		}

		coord_homo_transed[i][Y] = 0;
		for (int j = 0; j < 4; j++)
		{
			coord_homo_transed[i][Y] += Xsi[1][j] * coord_homo[i][j];
		}

		coord_homo_transed[i][Z] = 0;
		for (int j = 0; j < 4; j++)
		{
			coord_homo_transed[i][Z] += Xsi[2][j] * coord_homo[i][j];
		}

		coord_homo_transed[i][W] = 0;
		for (int j = 0; j < 4; j++)
		{
			coord_homo_transed[i][W] += Xsi[3][j] * coord_homo[i][j];
		}
	}

	for (int i = 0; i < 3; i++)
	{
		coord[i][X] = coord_homo_transed[i][X] / coord_homo_transed[i][W];
		coord[i][Y] = coord_homo_transed[i][Y] / coord_homo_transed[i][W];
		coord[i][Z] = coord_homo_transed[i][Z] / coord_homo_transed[i][W];
	}

	if (coord[0][Z] < 0 || coord[1][Z] < 0 || coord[2][Z] < 0)
	{
		return GZ_SUCCESS;
	}

/**********************************************************************************************************************/
	//Sort the vertices, calculate interpolate coefficients same as what we done in previous HWs

	int edgeOrientation[3]; //Indicate whether each edge is a left, right, top, or bottom edge
	SortVertex(coord, norm, edgeOrientation); //Make sure that vertices are sorted CW, and the first vertex is the most top left one

	//Calculate the linear function coefficients of three edges
	float A_l1 = coord[1][Y] - coord[0][Y], B_l1 = coord[0][X] - coord[1][X], C_l1 = -B_l1 * coord[0][Y] - A_l1 * coord[0][X];
	float A_l2 = coord[2][Y] - coord[1][Y], B_l2 = coord[1][X] - coord[2][X], C_l2 = -B_l2 * coord[1][Y] - A_l2 * coord[1][X];
	float A_l3 = coord[0][Y] - coord[2][Y], B_l3 = coord[2][X] - coord[0][X], C_l3 = -B_l3 * coord[2][Y] - A_l3 * coord[2][X];

	//calculate the plane function coefficients of the triangle
	float A_tri, B_tri, C_tri, D_tri;
	GzCoord cross_product_result, vec1, vec2;

	vec1[X] = coord[1][X] - coord[0][X];
	vec1[Y] = coord[1][Y] - coord[0][Y];
	vec1[Z] = coord[1][Z] - coord[0][Z];

	vec2[X] = coord[2][X] - coord[0][X];
	vec2[Y] = coord[2][Y] - coord[0][Y];
	vec2[Z] = coord[2][Z] - coord[0][Z];

	CrossProduct(cross_product_result, vec1, vec2);

	A_tri = cross_product_result[X];
	B_tri = cross_product_result[Y];
	C_tri = cross_product_result[Z];
	D_tri = -(A_tri * coord[0][X] + B_tri * coord[0][Y] + C_tri * coord[0][Z]);

	//The triangle is perpendicular to the camera, no need to rasterize
	if (C_tri == 0) { return GZ_SUCCESS; }

/**********************************************************************************************************************/
	//Since environment mapping needs precise interpolation of normals, so we cannot interpolate normal coords in image space like what we done in HW4
	//We must do what we done for u, v in HW5: first transfer normal coords to screen space, then interpolate them, finally transfer interpolated normal coords back to image space

	//First transfer the normal coords at 3 vertices into screen space, using the same formulas for u, v in HW5
	float Z_s[3];
	for (int i = 0; i < 3; i++)
	{
		Z_s[i] = coord[i][Z];
	}

	float Z_term[3];
	for (int i = 0; i < 3; i++)
	{
		Z_term[i] = Z_s[i] / (INT_MAX - Z_s[i]);
	}

	GzCoord norm_s[3]; //normal coords in screen space
	for (int i = 0; i < 3; i++)
	{
		norm_s[i][X] = norm[i][X] / (Z_term[i] + 1);
		norm_s[i][Y] = norm[i][Y] / (Z_term[i] + 1);
		norm_s[i][Z] = norm[i][Z] / (Z_term[i] + 1);
	}

	//Interpolate coefficients
	float A_norm[3], B_norm[3], C_norm[3], D_norm[3];

	for (int i = 0; i < 3; i++)
	{
		vec1[Z] = norm_s[1][i] - norm_s[0][i];
		vec2[Z] = norm_s[2][i] - norm_s[0][i];

		CrossProduct(cross_product_result, vec1, vec2);

		A_norm[i] = cross_product_result[X];
		B_norm[i] = cross_product_result[Y];
		C_norm[i] = cross_product_result[Z];
		D_norm[i] = -(A_norm[i] * coord[0][X] + B_norm[i] * coord[0][Y] + C_norm[i] * norm_s[0][i]);
	}

/**********************************************************************************************************************/

	switch (mode)
	{
		case LIGHT_GOURAUD:
		{
			//Use LEE for rasterization
			//If one pixel falls on one edge, only rasterize those on left edges and top edges
			GzColor vertex_color[3];// = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
			GzCoord E = { 0, 0, -1 }, R;
			GzLight light;
			for (int i_vertex = 0; i_vertex < 3; i_vertex++)
			{
				if (norm[i_vertex][X] * E[X] + norm[i_vertex][Y] * E[Y] + norm[i_vertex][Z] * E[Z] < 0)
				{
					norm[i_vertex][X] = -norm[i_vertex][X];
					norm[i_vertex][Y] = -norm[i_vertex][Y];
					norm[i_vertex][Z] = -norm[i_vertex][Z];
				}

				vertex_color[i_vertex][RED] = Ka[RED] * ambientlight.color[RED];
				vertex_color[i_vertex][GREEN] = Ka[GREEN] * ambientlight.color[GREEN];
				vertex_color[i_vertex][BLUE] = Ka[BLUE] * ambientlight.color[BLUE];

				for (int j_light = 0; j_light < numlights; j_light++)
				{
					light = lights[j_light];

					float NdotL = norm[i_vertex][X] * light.direction[X] + norm[i_vertex][Y] * light.direction[Y] + norm[i_vertex][Z] * light.direction[Z];
					if (NdotL > 0)
					{
						vertex_color[i_vertex][RED] += Kd[RED] * light.color[RED] * NdotL;
						vertex_color[i_vertex][GREEN] += Kd[GREEN] * light.color[GREEN] * NdotL;
						vertex_color[i_vertex][BLUE] += Kd[BLUE] * light.color[BLUE] * NdotL;

						R[X] = 2 * NdotL * norm[i_vertex][X] - light.direction[X];
						R[Y] = 2 * NdotL * norm[i_vertex][Y] - light.direction[Y];
						R[Z] = 2 * NdotL * norm[i_vertex][Z] - light.direction[Z];

						float RdotE = R[X] * E[X] + R[Y] * E[Y] + R[Z] * E[Z];
						if (RdotE > 0)
						{
							vertex_color[i_vertex][RED] += Ks[RED] * light.color[RED] * pow(RdotE, spec);
							vertex_color[i_vertex][GREEN] += Ks[GREEN] * light.color[GREEN] * pow(RdotE, spec);
							vertex_color[i_vertex][BLUE] += Ks[BLUE] * light.color[BLUE] * pow(RdotE, spec);
						}
					}
				}

				if (vertex_color[i_vertex][RED] > 1) { vertex_color[i_vertex][RED] = 1; }
				if (vertex_color[i_vertex][GREEN] > 1) { vertex_color[i_vertex][GREEN] = 1; }
				if (vertex_color[i_vertex][BLUE] > 1) { vertex_color[i_vertex][BLUE] = 1; }
			}

			float A_color[3], B_color[3], C_color[3], D_color[3];

			for (int i = 0; i < 3; i++)
			{
				//vec1[X] = coord[1][X] - coord[0][X];
				//vec1[Y] = coord[1][Y] - coord[0][Y];
				vec1[Z] = vertex_color[1][i] - vertex_color[0][i];

				//vec2[X] = coord[2][X] - coord[0][X];
				//vec2[Y] = coord[2][Y] - coord[0][Y];
				vec2[Z] = vertex_color[2][i] - vertex_color[0][i];

				CrossProduct(cross_product_result, vec1, vec2);

				A_color[i] = cross_product_result[X];
				B_color[i] = cross_product_result[Y];
				C_color[i] = cross_product_result[Z];
				D_color[i] = -(A_color[i] * coord[0][X] + B_color[i] * coord[0][Y] + C_color[i] * vertex_color[0][i]);
			}

			GzIntensity r, g, b;
			GzColor color_interp;
			int upperBound = (int)ceil(FindMin(coord, Y)), lowerBound = (int)floor(FindMax(coord, Y));
			int leftBound = (int)ceil(FindMin(coord, X)), rigntBound = (int)floor(FindMax(coord, X));
			bool doRast;
			for (int i = leftBound; i <= rigntBound; i++)
			{
				for (int j = upperBound; j <= lowerBound; j++)
				{
					doRast = false;
					//This consider both conditions:
					//1. The current pixel falls inside the triangle
					//2. The current pixel falls on the third edge, which must be an left edge according to our way to arrange the vertices
					if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 <= 0)
					{
						doRast = true;
					}
					//The current pixel falls on a top edge, which must be the first edge
					if (A_l1 * i + B_l1 * j + C_l1 == 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[0] == TOP)
					{
						doRast = true;
					}
					//The current pixel falls on a left edge, which must be the second edge if it is not the third edge
					if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 == 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[1] == LEFT)
					{
						doRast = true;
					}

					if (doRast == true)
					{
						GzDepth cur_z = 0;
						GzGet(i, j, NULL, NULL, NULL, NULL, &cur_z);

						float new_z = (-D_tri - A_tri * i - B_tri * j) / C_tri;
						if (new_z < cur_z)
						{
							color_interp[RED] = (-D_color[RED] - A_color[RED] * i - B_color[RED] * j) / C_color[RED];
							color_interp[GREEN] = (-D_color[GREEN] - A_color[GREEN] * i - B_color[GREEN] * j) / C_color[GREEN];
							color_interp[BLUE] = (-D_color[BLUE] - A_color[BLUE] * i - B_color[BLUE] * j) / C_color[BLUE];

							for (int k = 0; k < 3; k++)
							{
								if (color_interp[k] > 1) { color_interp[k] = 1; }
								if (color_interp[k] < 0) { color_interp[k] = 0; }
							}

							r = ctoi(color_interp[RED]);
							g = ctoi(color_interp[GREEN]);
							b = ctoi(color_interp[BLUE]);

							GzPut(i, j, r, g, b, DEFAULT_A, (GzDepth)new_z);
						}
					}
				}
			}
			break;
		}
		case LIGHT_PHONG:
		{
			//Use LEE for rasterization
			//If one pixel falls on one edge, only rasterize those on left edges and top edges
			float A_norm[3], B_norm[3], C_norm[3], D_norm[3];

			for (int i = 0; i < 3; i++)
			{
				//vec1[X] = coord[1][X] - coord[0][X];
				//vec1[Y] = coord[1][Y] - coord[0][Y];
				vec1[Z] = norm[1][i] - norm[0][i];

				//vec2[X] = coord[2][X] - coord[0][X];
				//vec2[Y] = coord[2][Y] - coord[0][Y];
				vec2[Z] = norm[2][i] - norm[0][i];

				CrossProduct(cross_product_result, vec1, vec2);

				A_norm[i] = cross_product_result[X];
				B_norm[i] = cross_product_result[Y];
				C_norm[i] = cross_product_result[Z];
				D_norm[i] = -(A_norm[i] * coord[0][X] + B_norm[i] * coord[0][Y] + C_norm[i] * norm[0][i]);
			}

			GzCoord E = { 0, 0, -1 }, R;
			GzCoord cur_norm;
			GzLight light;
			GzColor cur_color;
			GzIntensity r, g, b;
			int upperBound = (int)ceil(FindMin(coord, Y)), lowerBound = (int)floor(FindMax(coord, Y));
			int leftBound = (int)ceil(FindMin(coord, X)), rigntBound = (int)floor(FindMax(coord, X));
			bool doRast;
			for (int i = leftBound; i <= rigntBound; i++)
			{
				for (int j = upperBound; j <= lowerBound; j++)
				{
					doRast = false;
					//This consider both conditions:
					//1. The current pixel falls inside the triangle
					//2. The current pixel falls on the third edge, which must be an left edge according to our way to arrange the vertices
					if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 <= 0)
					{
						doRast = true;
					}
					//The current pixel falls on a top edge, which must be the first edge
					if (A_l1 * i + B_l1 * j + C_l1 == 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[0] == TOP)
					{
						doRast = true;
					}
					//The current pixel falls on a left edge, which must be the second edge if it is not the third edge
					if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 == 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[1] == LEFT)
					{
						doRast = true;
					}

					if (doRast == true)
					{
						GzDepth cur_z = 0;
						GzGet(i, j, NULL, NULL, NULL, NULL, &cur_z);

						float new_z = (-D_tri - A_tri * i - B_tri * j) / C_tri;
						if (new_z < cur_z)
						{
							cur_norm[X] = (-D_norm[X] - A_norm[X] * i - B_norm[X] * j) / C_norm[X];
							cur_norm[Y] = (-D_norm[Y] - A_norm[Y] * i - B_norm[Y] * j) / C_norm[Y];
							cur_norm[Z] = (-D_norm[Z] - A_norm[Z] * i - B_norm[Z] * j) / C_norm[Z];
							float normalizer = sqrt(cur_norm[X] * cur_norm[X] + cur_norm[Y] * cur_norm[Y] + cur_norm[Z] * cur_norm[Z]);
							cur_norm[X] /= normalizer;
							cur_norm[Y] /= normalizer;
							cur_norm[Z] /= normalizer;

							if (cur_norm[X] * E[X] + cur_norm[Y] * E[Y] + cur_norm[Z] * E[Z] < 0)
							{
								cur_norm[X] = -cur_norm[X];
								cur_norm[Y] = -cur_norm[Y];
								cur_norm[Z] = -cur_norm[Z];
							}

							cur_color[RED] = Ka[RED] * ambientlight.color[RED];
							cur_color[GREEN] = Ka[GREEN] * ambientlight.color[GREEN];
							cur_color[BLUE] = Ka[BLUE] * ambientlight.color[BLUE];

							for (int j_light = 0; j_light < numlights; j_light++)
							{
								light = lights[j_light];

								float NdotL = cur_norm[X] * light.direction[X] + cur_norm[Y] * light.direction[Y] + cur_norm[Z] * light.direction[Z];
								if (NdotL > 0)
								{
									cur_color[RED] += Kd[RED] * light.color[RED] * NdotL;
									cur_color[GREEN] += Kd[GREEN] * light.color[GREEN] * NdotL;
									cur_color[BLUE] += Kd[BLUE] * light.color[BLUE] * NdotL;

									R[X] = 2 * NdotL * cur_norm[X] - light.direction[X];
									R[Y] = 2 * NdotL * cur_norm[Y] - light.direction[Y];
									R[Z] = 2 * NdotL * cur_norm[Z] - light.direction[Z];

									float RdotE = R[X] * E[X] + R[Y] * E[Y] + R[Z] * E[Z];
									if (RdotE > 0)
									{
										cur_color[RED] += Ks[RED] * light.color[RED] * pow(RdotE, spec);
										cur_color[GREEN] += Ks[GREEN] * light.color[GREEN] * pow(RdotE, spec);
										cur_color[BLUE] += Ks[BLUE] * light.color[BLUE] * pow(RdotE, spec);
									}
								}
							}

							for (int k = 0; k < 3; k++)
							{
								if (cur_color[k] > 1) { cur_color[k] = 1; }
								if (cur_color[k] < 0) { cur_color[k] = 0; }
							}

							r = ctoi(cur_color[RED]);
							g = ctoi(cur_color[GREEN]);
							b = ctoi(cur_color[BLUE]);

							GzPut(i, j, r, g, b, DEFAULT_A, (GzDepth)new_z);
						}
					}
				}
			}
			break;
		}
		case REFLECTION:
		{
			GzCoord E = { 0, 0, -1 }, R;
			GzCoord cur_norm;
			GzLight light;
			GzColor cur_color;
			GzColor color;
			GzIntensity r, g, b;
			int upperBound = (int)ceil(FindMin(coord, Y)), lowerBound = (int)floor(FindMax(coord, Y));
			int leftBound = (int)ceil(FindMin(coord, X)), rigntBound = (int)floor(FindMax(coord, X));
			bool doRast;
			for (int i = leftBound; i <= rigntBound; i++)
			{
				for (int j = upperBound; j <= lowerBound; j++)
				{
					doRast = false;
					if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 <= 0)
					{
						doRast = true;
					}
					if (A_l1 * i + B_l1 * j + C_l1 == 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[0] == TOP)
					{
						doRast = true;
					}
					if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 == 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[1] == LEFT)
					{
						doRast = true;
					}

					if (doRast == true)
					{
						GzDepth cur_z = 0;
						GzGet(i, j, NULL, NULL, NULL, NULL, &cur_z);

						float new_z = (-D_tri - A_tri * i - B_tri * j) / C_tri; //screen space z value at pixel (i, j)
						if (new_z < cur_z)
						{
						/**********************************************************************************************************************/

							//Calculate interpolated normal coords in screen space at pixel (i, j)
							cur_norm[X] = (-D_norm[X] - A_norm[X] * i - B_norm[X] * j) / C_norm[X];
							cur_norm[Y] = (-D_norm[Y] - A_norm[Y] * i - B_norm[Y] * j) / C_norm[Y];
							cur_norm[Z] = (-D_norm[Z] - A_norm[Z] * i - B_norm[Z] * j) / C_norm[Z];

							//Transfer interpolated normal coords at (i, j) back to image space
							float Z_term_interp = new_z / (INT_MAX - new_z);

							cur_norm[X] *= (Z_term_interp + 1);
							cur_norm[Y] *= (Z_term_interp + 1);
							cur_norm[Z] *= (Z_term_interp + 1);

							float normalizer = sqrt(cur_norm[X] * cur_norm[X] + cur_norm[Y] * cur_norm[Y] + cur_norm[Z] * cur_norm[Z]);
							cur_norm[X] /= normalizer;
							cur_norm[Y] /= normalizer;
							cur_norm[Z] /= normalizer;

							//Use reflection function to calculate reflect vector
							float NdotE = cur_norm[X] * E[X] + cur_norm[Y] * E[Y] + cur_norm[Z] * E[Z];
							if (NdotE < 0)
							{
								cur_norm[X] = -cur_norm[X];
								cur_norm[Y] = -cur_norm[Y];
								cur_norm[Z] = -cur_norm[Z];
								NdotE = -NdotE;
							}

							R[X] = 2 * NdotE * cur_norm[X] - E[X];
							R[Y] = 2 * NdotE * cur_norm[Y] - E[Y];
							R[Z] = 2 * NdotE * cur_norm[Z] - E[Z];

						/**********************************************************************************************************************/

							//Decide which cube map we should use
							float max_component = -10000.0;
							int index;
							for (int k = 0; k < 3; k++)
							{
								if (fabs(R[k]) > max_component)
								{
									max_component = fabs(R[k]);
									index = k;
								}
							}
							max_component = R[index];

							//Calculate u, v for texture lookup
							float u, v;
							int u_index, v_index;
							switch (index)
							{
								case X:
								{
									u_index = Y;
									v_index = Z;
									break;
								}
								case Y:
								{
									u_index = X;
									v_index = Z;
									break;
								}
								case Z:
								{
									u_index = X;
									v_index = Y;
									break;
								}
							}

							int surface;
							//Original u, v are in [-1, 1], we need to scale and transfer them to [0, xs - 1] and [0, ys - 1], note that xs == ys
							if (index == X && max_component > 0)
							{
								surface = X_POSITIVE;

								u = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//u = (xs - 1) - u;
								v = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								v = (xs - 1) - v;
							}
							if (index == X && max_component < 0)
							{
								surface = X_NEGATIVE;

								u = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								u = (xs - 1) - u;
								v = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								v = (xs - 1) - v;
							}
							if (index == Y && max_component > 0)
							{
								surface = Y_POSITIVE;

								u = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//u = (xs - 1) - u;
								v = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//v = (xs - 1) - v;
							}
							if (index == Y && max_component < 0)
							{
								surface = Y_NEGATIVE;

								u = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//u = (xs - 1) - u;
								v = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								v = (xs - 1) - v;
							}
							if (index == Z && max_component > 0)
							{
								surface = Z_NEGATIVE;

								u = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								u = (xs - 1) - u;
								v = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//v = (xs - 1) - v;
							}
							if (index == Z && max_component < 0)
							{
								surface = Z_POSITIVE;

								u = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//u = (xs - 1) - u;
								v = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								v = (xs - 1) - v;
							}

							if (u > (xs - 1)) 
							{ 
								u = xs - 1; 
							}
							if (u < 0) 
							{ 
								u = 0; 
							}
							if (v > (xs - 1)) 
							{ 
								v = xs - 1; 
							}
							if (v < 0) 
							{
								v = 0; 
							}

							//Texture lookup like what we done in tex_fun.cpp in HW5
							int x_lower = (int)floor(u), x_upper = (int)ceil(u), y_lower = (int)floor(v), y_upper = (int)ceil(v);

							float s = u - x_lower, t = v - y_lower;

							GzColor color_A, color_B, color_C, color_D;

							color_A[RED] = cube_map[surface][y_lower * xs + x_lower][RED];
							color_A[GREEN] = cube_map[surface][y_lower * xs + x_lower][GREEN];
							color_A[BLUE] = cube_map[surface][y_lower * xs + x_lower][BLUE];

							color_B[RED] = cube_map[surface][y_lower * xs + x_upper][RED];
							color_B[GREEN] = cube_map[surface][y_lower * xs + x_upper][GREEN];
							color_B[BLUE] = cube_map[surface][y_lower * xs + x_upper][BLUE];

							color_C[RED] = cube_map[surface][y_upper * xs + x_upper][RED];
							color_C[GREEN] = cube_map[surface][y_upper * xs + x_upper][GREEN];
							color_C[BLUE] = cube_map[surface][y_upper * xs + x_upper][BLUE];

							color_D[RED] = cube_map[surface][y_upper * xs + x_lower][RED];
							color_D[GREEN] = cube_map[surface][y_upper * xs + x_lower][GREEN];
							color_D[BLUE] = cube_map[surface][y_upper * xs + x_lower][BLUE];

							color[RED] = s * t * color_C[RED] + (1 - s) * t * color_D[RED] + s * (1 - t) * color_B[RED] + (1 - s) * (1 - t) * color_A[RED];
							color[GREEN] = s * t * color_C[GREEN] + (1 - s) * t * color_D[GREEN] + s * (1 - t) * color_B[GREEN] + (1 - s) * (1 - t) * color_A[GREEN];
							color[BLUE] = s * t * color_C[BLUE] + (1 - s) * t * color_D[BLUE] + s * (1 - t) * color_B[BLUE] + (1 - s) * (1 - t) * color_A[BLUE];

							//shade(cur_norm, color);

							for (int k = 0; k < 3; k++)
							{
								if (color[k] > 1) { color[k] = 1; }
								if (color[k] < 0) { color[k] = 0; }
							}

							r = ctoi(color[RED]);
							g = ctoi(color[GREEN]);
							b = ctoi(color[BLUE]);

							GzPut(i, j, r, g, b, DEFAULT_A, (GzDepth)new_z);
						}
					}
				}
			}

			break;
		}
		case REFRACTION:
		{
			GzCoord E = { 0, 0, -1 }, R;
			GzCoord cur_norm;
			GzLight light;
			GzColor cur_color;
			GzColor color;
			GzIntensity r, g, b;
			int upperBound = (int)ceil(FindMin(coord, Y)), lowerBound = (int)floor(FindMax(coord, Y));
			int leftBound = (int)ceil(FindMin(coord, X)), rigntBound = (int)floor(FindMax(coord, X));
			bool doRast;
			for (int i = leftBound; i <= rigntBound; i++)
			{
				for (int j = upperBound; j <= lowerBound; j++)
				{
					doRast = false;
					if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 <= 0)
					{
						doRast = true;
					}
					if (A_l1 * i + B_l1 * j + C_l1 == 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[0] == TOP)
					{
						doRast = true;
					}
					if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 == 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[1] == LEFT)
					{
						doRast = true;
					}

					if (doRast == true)
					{
						GzDepth cur_z = 0;
						GzGet(i, j, NULL, NULL, NULL, NULL, &cur_z);

						float new_z = (-D_tri - A_tri * i - B_tri * j) / C_tri;
						if (new_z < cur_z)
						{
							float Z_term_interp = new_z / (INT_MAX - new_z);

							cur_norm[X] = (-D_norm[X] - A_norm[X] * i - B_norm[X] * j) / C_norm[X];
							cur_norm[Y] = (-D_norm[Y] - A_norm[Y] * i - B_norm[Y] * j) / C_norm[Y];
							cur_norm[Z] = (-D_norm[Z] - A_norm[Z] * i - B_norm[Z] * j) / C_norm[Z];

							cur_norm[X] *= (Z_term_interp + 1);
							cur_norm[Y] *= (Z_term_interp + 1);
							cur_norm[Z] *= (Z_term_interp + 1);

							float normalizer = sqrt(cur_norm[X] * cur_norm[X] + cur_norm[Y] * cur_norm[Y] + cur_norm[Z] * cur_norm[Z]);
							cur_norm[X] /= normalizer;
							cur_norm[Y] /= normalizer;
							cur_norm[Z] /= normalizer;

							float xi, yi, zi;
							xi = (Z_term_interp + 1) * (2 * i - xres) / xres;
							yi = (Z_term_interp + 1) * (yres - 2 * j) / yres;
							zi = Z_term_interp / tan(ang2rad(m_camera.FOV) / 2);

							float NdotE = cur_norm[X] * E[X] + cur_norm[Y] * E[Y] + cur_norm[Z] * E[Z];
							if (NdotE < 0)
							{
								cur_norm[X] = -cur_norm[X];
								cur_norm[Y] = -cur_norm[Y];
								cur_norm[Z] = -cur_norm[Z];
								NdotE = -NdotE;
							}

							float eta = 1.5;
							float coef = NdotE - sqrt(eta * eta - 1 + NdotE * NdotE);

							R[X] = (coef * cur_norm[X] - E[X]) / eta;
							R[Y] = (coef * cur_norm[Y] - E[Y]) / eta;
							R[Z] = (coef * cur_norm[Z] - E[Z]) / eta;

							//Decide which cube map we should use
							float max_component = -10000.0;
							int index;
							for (int k = 0; k < 3; k++)
							{
								if (fabs(R[k]) > max_component)
								{
									max_component = fabs(R[k]);
									index = k;
								}
							}
							max_component = R[index];

							//Calculate u, v for texture lookup
							float u, v;
							int u_index, v_index;
							switch (index)
							{
								case X:
								{
									u_index = Y;
									v_index = Z;
									break;
								}
								case Y:
								{
									u_index = X;
									v_index = Z;
									break;
								}
								case Z:
								{
									u_index = X;
									v_index = Y;
									break;
								}
							}

							int surface;
							if (index == X && max_component > 0)
							{
								surface = X_POSITIVE;

								u = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//u = (xs - 1) - u;
								v = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								v = (xs - 1) - v;
							}
							if (index == X && max_component < 0)
							{
								surface = X_NEGATIVE;

								u = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								u = (xs - 1) - u;
								v = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								v = (xs - 1) - v;
							}
							if (index == Y && max_component > 0)
							{
								surface = Y_POSITIVE;

								u = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								u = (xs - 1) - u;
								v = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
							}
							if (index == Y && max_component < 0)
							{
								surface = Y_NEGATIVE;

								u = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								u = (xs - 1) - u;
								v = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								v = (xs - 1) - v;
							}
							if (index == Z && max_component > 0)
							{
								surface = Z_NEGATIVE;

								u = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								u = (xs - 1) - u;
								v = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//v = (xs - 1) - v;
							}
							if (index == Z && max_component < 0)
							{
								surface = Z_POSITIVE;

								u = R[u_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								//u = (xs - 1) - u;
								v = R[v_index] * ((xs - 1) / 2) / fabs(R[index]) + ((xs - 1) / 2);
								v = (xs - 1) - v;
							}

							if (u > (xs - 1))
							{
								u = xs - 1;
							}
							if (u < 0)
							{
								u = 0;
							}
							if (v > (xs - 1))
							{
								v = xs - 1;
							}
							if (v < 0)
							{
								v = 0;
							}

							int x_lower = (int)floor(u), x_upper = (int)ceil(u), y_lower = (int)floor(v), y_upper = (int)ceil(v);

							float s = u - x_lower, t = v - y_lower;

							GzColor color_A, color_B, color_C, color_D;

							color_A[RED] = cube_map[surface][y_lower * xs + x_lower][RED];
							color_A[GREEN] = cube_map[surface][y_lower * xs + x_lower][GREEN];
							color_A[BLUE] = cube_map[surface][y_lower * xs + x_lower][BLUE];

							color_B[RED] = cube_map[surface][y_lower * xs + x_upper][RED];
							color_B[GREEN] = cube_map[surface][y_lower * xs + x_upper][GREEN];
							color_B[BLUE] = cube_map[surface][y_lower * xs + x_upper][BLUE];

							color_C[RED] = cube_map[surface][y_upper * xs + x_upper][RED];
							color_C[GREEN] = cube_map[surface][y_upper * xs + x_upper][GREEN];
							color_C[BLUE] = cube_map[surface][y_upper * xs + x_upper][BLUE];

							color_D[RED] = cube_map[surface][y_upper * xs + x_lower][RED];
							color_D[GREEN] = cube_map[surface][y_upper * xs + x_lower][GREEN];
							color_D[BLUE] = cube_map[surface][y_upper * xs + x_lower][BLUE];

							color[RED] = s * t * color_C[RED] + (1 - s) * t * color_D[RED] + s * (1 - t) * color_B[RED] + (1 - s) * (1 - t) * color_A[RED];
							color[GREEN] = s * t * color_C[GREEN] + (1 - s) * t * color_D[GREEN] + s * (1 - t) * color_B[GREEN] + (1 - s) * (1 - t) * color_A[GREEN];
							color[BLUE] = s * t * color_C[BLUE] + (1 - s) * t * color_D[BLUE] + s * (1 - t) * color_B[BLUE] + (1 - s) * (1 - t) * color_A[BLUE];

							for (int k = 0; k < 3; k++)
							{
								if (color[k] > 1) { color[k] = 1; }
								if (color[k] < 0) { color[k] = 0; }
							}

							r = ctoi(color[RED]);
							g = ctoi(color[GREEN]);
							b = ctoi(color[BLUE]);

							GzPut(i, j, r, g, b, DEFAULT_A, (GzDepth)new_z);
						}
					}
				}
			}

			break;
		}
	}

	

	return GZ_SUCCESS;
}

int GzRender::GzReadCubeMap()
{
/* Read 6 cube maps
 * The input files must be ppm format square images
 */
	std::string _box = BOX;
	std::string map1 = _box + "_rt.ppm";
	std::string map2 = _box + "_lf.ppm";
	std::string map3 = _box + "_up.ppm";
	std::string map4 = _box + "_dn.ppm";
	std::string map5 = _box + "_bk.ppm";
	std::string map6 = _box + "_ft.ppm";

	const char *map[6];
	map[0] = map1.c_str();
	map[1] = map2.c_str();
	map[2] = map3.c_str();
	map[3] = map4.c_str();
	map[4] = map5.c_str();
	map[5] = map6.c_str();
	
	unsigned char pixel[3];
	unsigned char dummy;
	char foo[8];
	
	FILE *fd;

	for (int i = 0; i < 6; i++)
	{
		fd = fopen(map[i], "rb");
		if (fd == NULL) {
			fprintf(stderr, "texture file not found\n");
			exit(-1);
		}
		fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		fread(pixel, sizeof(pixel), 1, fd);

		cube_map[i] = (GzColor*)malloc(sizeof(GzColor) * (xs * ys + 1));
		if (cube_map[i] == NULL) {
			fprintf(stderr, "malloc for texture image failed\n");
			exit(-1);
		}

		for (int j = 0; j < xs * ys; j++)
		{
			fread(pixel, sizeof(pixel), 1, fd);

			cube_map[i][j][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			cube_map[i][j][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			cube_map[i][j][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		fclose(fd);
	}
	
	return GZ_SUCCESS;
}

int GzRender::GzShowCubeMap()
{
/* Show cube maps
 */
	int map_num = 0;
	GzIntensity r, g, b;
	int index;
	for (int x_offset = 0; x_offset < xs * 3; x_offset += xs)
	{
		for (int y_offset = 0; y_offset < ys * 2; y_offset += ys)
		{
			for (int j = y_offset; j < y_offset + ys; j++)
			{
				for (int i = x_offset; i < x_offset + xs; i++)
				{
					index = (i - x_offset) + (j - y_offset) * xs;
					r = ctoi(cube_map[map_num][index][RED]);
					g = ctoi(cube_map[map_num][index][GREEN]);
					b = ctoi(cube_map[map_num][index][BLUE]);
					GzPut(i, j, r, g, b, DEFAULT_A, DEFAULT_Z);
				}
			}

			map_num++;
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzPutOriginTriangle(GzPointer *valueList)
{
/* In order for precise refraction calculation, we need to first read all triangles into the render
 * This function read one triangle into GzRender::triangles each time
 */
	GzCoord *coord_receive = (GzCoord *)valueList[0];
	GzCoord *norm_receive = (GzCoord *)valueList[1];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			triangles[num_triangle][i][j] = coord_receive[i][j];
			triangles[num_triangle][i][j + 3] = norm_receive[i][j];
			coord_max[j] = (coord_receive[i][j] > coord_max[j]) ? coord_receive[i][j] : coord_max[j];
			coord_min[j] = (coord_receive[i][j] < coord_min[j]) ? coord_receive[i][j] : coord_min[j];
		}
	}
	num_triangle++;

	return GZ_SUCCESS;
}

int GzRender::GzParseTriangles()
{
/* Transfer all triangles to image space
 * Then deal with each triangle for reflection or refraction using GzPutTriangle()
 */
	GzMatrix X_top_im, X_top_norm;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			X_top_im[i][j] = Xim[matlevel][i][j];
			X_top_norm[i][j] = Xnorm[matlevel][i][j];
		}
	}

	GzCoord coord[3], norm[3];
	for (int num = 0; num < num_triangle; num++)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				coord[i][j] = triangles[num][i][j];
				norm[i][j] = triangles[num][i][j + 3];
			}
		}

		//for (int i = 0; i < 3; i++)
		//{
		//	coord[i][X] -= object_center[X];
		//	coord[i][Y] -= object_center[Y];
		//	coord[i][Z] -= object_center[Z];

		//	float max_width = -10000.0;
		//	for (int j = 0; j < 3; j++) { max_width = (max_width > coord_max[j] - coord_min[j]) ? max_width : coord_max[j] - coord_min[j]; }
		//	float scaling = 0.3 * xs / max_width;
		//	coord[i][X] *= scaling;
		//	coord[i][Y] *= scaling;
		//	coord[i][Z] *= scaling;

		//	coord[i][X] += object_center[X];
		//	coord[i][Y] += object_center[Y];
		//	coord[i][Z] += object_center[Z];
		//}

		float coord_homo[3][4], norm_homo[3][4];
		for (int i = 0; i < 3; i++)
		{
			coord_homo[i][X] = coord[i][X];
			coord_homo[i][Y] = coord[i][Y];
			coord_homo[i][Z] = coord[i][Z];
			coord_homo[i][W] = 1;

			norm_homo[i][X] = norm[i][X];
			norm_homo[i][Y] = norm[i][Y];
			norm_homo[i][Z] = norm[i][Z];
			norm_homo[i][W] = 1;
		}

		float coord_homo_transed[3][4], norm_homo_transed[3][4];
		for (int i = 0; i < 3; i++)
		{
			coord_homo_transed[i][X] = 0;
			norm_homo_transed[i][X] = 0;
			for (int j = 0; j < 4; j++)
			{
				coord_homo_transed[i][X] += X_top_im[0][j] * coord_homo[i][j];
				norm_homo_transed[i][X] += X_top_norm[0][j] * norm_homo[i][j];
			}

			coord_homo_transed[i][Y] = 0;
			norm_homo_transed[i][Y] = 0;
			for (int j = 0; j < 4; j++)
			{
				coord_homo_transed[i][Y] += X_top_im[1][j] * coord_homo[i][j];
				norm_homo_transed[i][Y] += X_top_norm[1][j] * norm_homo[i][j];
			}

			coord_homo_transed[i][Z] = 0;
			norm_homo_transed[i][Z] = 0;
			for (int j = 0; j < 4; j++)
			{
				coord_homo_transed[i][Z] += X_top_im[2][j] * coord_homo[i][j];
				norm_homo_transed[i][Z] += X_top_norm[2][j] * norm_homo[i][j];
			}

			coord_homo_transed[i][W] = 0;
			norm_homo_transed[i][W] = 0;
			for (int j = 0; j < 4; j++)
			{
				coord_homo_transed[i][W] += X_top_im[3][j] * coord_homo[i][j];
				norm_homo_transed[i][W] += X_top_norm[3][j] * norm_homo[i][j];
			}
		}

		for (int i = 0; i < 3; i++)
		{
			coord[i][X] = coord_homo_transed[i][X] / coord_homo_transed[i][W];
			coord[i][Y] = coord_homo_transed[i][Y] / coord_homo_transed[i][W];
			coord[i][Z] = coord_homo_transed[i][Z] / coord_homo_transed[i][W];

			norm[i][X] = norm_homo_transed[i][X] / norm_homo_transed[i][W];
			norm[i][Y] = norm_homo_transed[i][Y] / norm_homo_transed[i][W];
			norm[i][Z] = norm_homo_transed[i][Z] / norm_homo_transed[i][W];
		}

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				triangles[num][i][j] = coord[i][j];
				triangles[num][i][j + 3] = norm[i][j];
			}
		}
	}

	for (int i = 0; i < num_triangle; i++)
	{
		//GzShowTriangles(i);
		GzPutTriangle(i, MODE);
	}
	
	return GZ_SUCCESS;
}

int GzRender::GzShowTriangles(int triangle_index)
{
/* Show all triangles using flat shading without reflection or refraction, not important
 */
	GzCoord coord[3], norm[3];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			coord[i][j] = triangles[triangle_index][i][j];
			norm[i][j] = triangles[triangle_index][i][j + 3];
		}
	}

	float coord_homo[3][4];
	for (int i = 0; i < 3; i++)
	{
		coord_homo[i][X] = coord[i][X];
		coord_homo[i][Y] = coord[i][Y];
		coord_homo[i][Z] = coord[i][Z];
		coord_homo[i][W] = 1;
	}

	float coord_homo_transed[3][4];
	for (int i = 0; i < 3; i++)
	{
		coord_homo_transed[i][X] = 0;
		for (int j = 0; j < 4; j++)
		{
			coord_homo_transed[i][X] += Xsi[0][j] * coord_homo[i][j];
		}

		coord_homo_transed[i][Y] = 0;
		for (int j = 0; j < 4; j++)
		{
			coord_homo_transed[i][Y] += Xsi[1][j] * coord_homo[i][j];
		}

		coord_homo_transed[i][Z] = 0;
		for (int j = 0; j < 4; j++)
		{
			coord_homo_transed[i][Z] += Xsi[2][j] * coord_homo[i][j];
		}

		coord_homo_transed[i][W] = 0;
		for (int j = 0; j < 4; j++)
		{
			coord_homo_transed[i][W] += Xsi[3][j] * coord_homo[i][j];
		}
	}

	for (int i = 0; i < 3; i++)
	{
		coord[i][X] = coord_homo_transed[i][X] / coord_homo_transed[i][W];
		coord[i][Y] = coord_homo_transed[i][Y] / coord_homo_transed[i][W];
		coord[i][Z] = coord_homo_transed[i][Z] / coord_homo_transed[i][W];
	}

	if (coord[0][Z] < 0 || coord[1][Z] < 0 || coord[2][Z] < 0)
	{
		return GZ_SUCCESS;
	}

	int edgeOrientation[3]; //Indicate whether each edge is a left, right, top, or bottom edge
	SortVertex(coord, norm, edgeOrientation); //Make sure that vertices are sorted CW, and the first vertex is the most top left one

	//Calculate the linear function coefficients of three edges
	float A_l1 = coord[1][Y] - coord[0][Y], B_l1 = coord[0][X] - coord[1][X], C_l1 = -B_l1 * coord[0][Y] - A_l1 * coord[0][X];
	float A_l2 = coord[2][Y] - coord[1][Y], B_l2 = coord[1][X] - coord[2][X], C_l2 = -B_l2 * coord[1][Y] - A_l2 * coord[1][X];
	float A_l3 = coord[0][Y] - coord[2][Y], B_l3 = coord[2][X] - coord[0][X], C_l3 = -B_l3 * coord[2][Y] - A_l3 * coord[2][X];

	//calculate the plane function coefficients of the triangle
	float A_tri, B_tri, C_tri, D_tri;
	GzCoord cross_product_result, vec1, vec2;

	vec1[X] = coord[1][X] - coord[0][X];
	vec1[Y] = coord[1][Y] - coord[0][Y];
	vec1[Z] = coord[1][Z] - coord[0][Z];

	vec2[X] = coord[2][X] - coord[0][X];
	vec2[Y] = coord[2][Y] - coord[0][Y];
	vec2[Z] = coord[2][Z] - coord[0][Z];

	CrossProduct(cross_product_result, vec1, vec2);

	A_tri = cross_product_result[X];
	B_tri = cross_product_result[Y];
	C_tri = cross_product_result[Z];
	D_tri = -(A_tri * coord[0][X] + B_tri * coord[0][Y] + C_tri * coord[0][Z]);

	//The triangle is perpendicular to the camera, no need to rasterize
	if (C_tri == 0) { return GZ_SUCCESS; }

	shade(norm[0], flatcolor);

	for (int k = 0; k < 3; k++)
	{
		if (flatcolor[k] > 1) { flatcolor[k] = 1; }
		if (flatcolor[k] < 0) { flatcolor[k] = 0; }
	}

	//Use LEE for rasterization
	//If one pixel falls on one edge, only rasterize those on left edges and top edges
	GzIntensity r = ctoi(flatcolor[RED]), g = ctoi(flatcolor[GREEN]), b = ctoi(flatcolor[BLUE]);
	int upperBound = (int)ceil(FindMin(coord, Y)), lowerBound = (int)floor(FindMax(coord, Y));
	int leftBound = (int)ceil(FindMin(coord, X)), rigntBound = (int)floor(FindMax(coord, X));
	bool doRast;
	for (int i = leftBound; i <= rigntBound; i++)
	{
		for (int j = upperBound; j <= lowerBound; j++)
		{
			doRast = false;
			//This consider both conditions:
			//1. The current pixel falls inside the triangle
			//2. The current pixel falls on the third edge, which must be an left edge according to our way to arrange the vertices
			if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 <= 0)
			{
				doRast = true;
			}
			//The current pixel falls on a top edge, which must be the first edge
			if (A_l1 * i + B_l1 * j + C_l1 == 0 && A_l2 * i + B_l2 * j + C_l2 < 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[0] == TOP)
			{
				doRast = true;
			}
			//The current pixel falls on a left edge, which must be the second edge if it is not the third edge
			if (A_l1 * i + B_l1 * j + C_l1 < 0 && A_l2 * i + B_l2 * j + C_l2 == 0 && A_l3 * i + B_l3 * j + C_l3 < 0 && edgeOrientation[1] == LEFT)
			{
				doRast = true;
			}

			if (doRast == true)
			{
				GzDepth cur_z = 0;
				GzGet(i, j, NULL, NULL, NULL, NULL, &cur_z);

				float new_z = (-D_tri - A_tri * i - B_tri * j) / C_tri;
				if (new_z < cur_z)
				{
					GzPut(i, j, r, g, b, DEFAULT_A, (GzDepth)new_z);
				}
			}
		}
	}

	return GZ_SUCCESS;
}

/***********************************************/
/* Helper methods */

void CrossProduct(GzCoord target, GzCoord vec1, GzCoord vec2)
{
	target[X] = vec1[Y] * vec2[Z] - vec2[Y] * vec1[Z];
	target[Y] = vec2[X] * vec1[Z] - vec1[X] * vec2[Z];
	target[Z] = vec1[X] * vec2[Y] - vec2[X] * vec1[Y];
}

void SortVertex(GzCoord *coord, GzCoord *norm, int edgeOrientation[])
{
	/*
	-- Sort the vertices so that they are arranged CW, and record their orientation.
	-- The first vertex must be the toppest. If there are two toppest vertices, the first one is the left one.
	-- The orientations of edges only have 4 possible conditions, which are listed below.
	*/
	GzCoord temp;
	//Sort the vertices so that the one with the min Y is the 1st,
	//and the one with the max Y is the 3rd
	if (coord[1][Y] < coord[0][Y] && coord[1][Y] < coord[2][Y]) //If the current 2nd has the min Y, move it to the 1st place
	{
		for (int i = 0; i < 3; i++)
		{
			temp[i] = coord[0][i];
			coord[0][i] = coord[1][i];
			coord[1][i] = temp[i];

			temp[i] = norm[0][i];
			norm[0][i] = norm[1][i];
			norm[1][i] = temp[i];
		}
	}
	if (coord[2][Y] < coord[0][Y] && coord[2][Y] < coord[1][Y]) //If the current 3rd has the min Y, move it to the 1st place
	{
		for (int i = 0; i < 3; i++)
		{
			temp[i] = coord[0][i];
			coord[0][i] = coord[2][i];
			coord[2][i] = temp[i];

			temp[i] = norm[0][i];
			norm[0][i] = norm[2][i];
			norm[2][i] = temp[i];
		}
	}
	if (coord[1][Y] > coord[2][Y]) //The 1st is already the one with the min Y, so move the one with the max Y to the 3rd place
	{
		for (int i = 0; i < 3; i++)
		{
			temp[i] = coord[1][i];
			coord[1][i] = coord[2][i];
			coord[2][i] = temp[i];

			temp[i] = norm[1][i];
			norm[1][i] = norm[2][i];
			norm[2][i] = temp[i];
		}
	}
	if (coord[0][Y] > coord[2][Y]) //The only condition we have not coverd yet is: 1st > 2nd = 3rd, this step solve this problem
	{
		for (int i = 0; i < 3; i++)
		{
			temp[i] = coord[0][i];
			coord[0][i] = coord[2][i];
			coord[2][i] = temp[i];

			temp[i] = norm[0][i];
			norm[0][i] = norm[2][i];
			norm[2][i] = temp[i];
		}
	}

	//Make sure that the vertices are sorted CW when there are two vertices with the same Y
	if (coord[0][Y] == coord[1][Y] && coord[0][X] > coord[1][X])
	{
		for (int i = 0; i < 3; i++)
		{
			temp[i] = coord[0][i];
			coord[0][i] = coord[1][i];
			coord[1][i] = temp[i];

			temp[i] = norm[0][i];
			norm[0][i] = norm[1][i];
			norm[1][i] = temp[i];
		}
		edgeOrientation[0] = TOP;
		edgeOrientation[1] = RIGHT;
		edgeOrientation[2] = LEFT;
	}

	if (coord[1][Y] == coord[2][Y] && coord[1][X] < coord[2][X])
	{
		for (int i = 0; i < 3; i++)
		{
			temp[i] = coord[1][i];
			coord[1][i] = coord[2][i];
			coord[2][i] = temp[i];

			temp[i] = norm[1][i];
			norm[1][i] = norm[2][i];
			norm[2][i] = temp[i];
		}
		edgeOrientation[0] = RIGHT;
		edgeOrientation[1] = BOTTOM;
		edgeOrientation[2] = LEFT;
	}

	//Make sure that the vertices are sorted CW when all vertices have different Y
	if (coord[0][X] + (coord[2][X] - coord[0][X]) / (coord[2][Y] - coord[0][Y]) * (coord[1][Y] - coord[0][Y]) > coord[1][X])
	{
		for (int i = 0; i < 3; i++)
		{
			temp[i] = coord[1][i];
			coord[1][i] = coord[2][i];
			coord[2][i] = temp[i];

			temp[i] = norm[1][i];
			norm[1][i] = norm[2][i];
			norm[2][i] = temp[i];
		}
		edgeOrientation[0] = RIGHT;
		edgeOrientation[1] = LEFT;
		edgeOrientation[2] = LEFT;
	}
	else
	{
		edgeOrientation[0] = RIGHT;
		edgeOrientation[1] = RIGHT;
		edgeOrientation[2] = LEFT;
	}
}

float FindMax(GzCoord *coord, int axis)
{
	float max = coord[0][axis];
	if (coord[1][axis] > max) { max = coord[1][axis]; }
	if (coord[2][axis] > max) { max = coord[2][axis]; }
	return max;
}

float FindMin(GzCoord *coord, int axis)
{
	float min = coord[0][axis];
	if (coord[1][axis] < min) { min = coord[1][axis]; }
	if (coord[2][axis] < min) { min = coord[2][axis]; }
	return min;
}

void shade(GzCoord norm, GzCoord color)
{
	GzCoord	light;
	float		coef;

	light[0] = 0.707f;
	light[1] = 0.5f;
	light[2] = 0.5f;

	coef = light[0] * norm[0] + light[1] * norm[1] + light[2] * norm[2];
	if (coef < 0) 	coef *= -1;

	if (coef > 1.0)	coef = 1.0;
	color[0] = coef * 0.95f;
	color[1] = coef * 0.65f;
	color[2] = coef * 0.88f;
}