/*
* Based on example code for PNG
* https://gist.github.com/niw/5963798
*/
#include "imagelib.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <math.h>
#include <string.h>



/** Code by Vicho Dragicevic (https://github.com/vdrg) */
Color rgb_2_lab(uint8_t R, uint8_t G, uint8_t B) {
  float var_R = R/255.0;
  float var_G = G/255.0;
  float var_B = B/255.0;


  if ( var_R > 0.04045 ) var_R = pow( (( var_R + 0.055 ) / 1.055 ), 2.4 );
  else                   var_R = var_R / 12.92;
  if ( var_G > 0.04045 ) var_G = pow( ( ( var_G + 0.055 ) / 1.055 ), 2.4);
  else                   var_G = var_G / 12.92;
  if ( var_B > 0.04045 ) var_B = pow( ( ( var_B + 0.055 ) / 1.055 ), 2.4);
  else                   var_B = var_B / 12.92;

  var_R = var_R * 100.;
  var_G = var_G * 100.;
  var_B = var_B * 100.;

  //Observer. = 2°, Illuminant = D65
  float X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
  float Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
  float Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;


  float var_X = X / 95.047 ;         //ref_X =  95.047   Observer= 2°, Illuminant= D65
  float var_Y = Y / 100.000;          //ref_Y = 100.000
  float var_Z = Z / 108.883;          //ref_Z = 108.883

  if ( var_X > 0.008856 ) var_X = pow(var_X , ( 1./3. ) );
  else                    var_X = ( 7.787 * var_X ) + ( 16. / 116. );
  if ( var_Y > 0.008856 ) var_Y = pow(var_Y , ( 1./3. ));
  else                    var_Y = ( 7.787 * var_Y ) + ( 16. / 116. );
  if ( var_Z > 0.008856 ) var_Z = pow(var_Z , ( 1./3. ));
  else                    var_Z = ( 7.787 * var_Z ) + ( 16. / 116. );

  float l_s = ( 116. * var_Y ) - 16.;
  float a_s = 500. * ( var_X - var_Y );
  float b_s = 200. * ( var_Y - var_Z );
  Color p = {l_s, a_s, b_s};
  return p;
}

/** Code by Vicho Dragicevic (https://github.com/vdrg) */
void lab_2_rgb(Color color, uint8_t* R, uint8_t* G, uint8_t *B) {
	float l_s = color.L;
	float a_s = color.a;
	float b_s = color.b;

  float var_Y = ( l_s + 16. ) / 116.;
  float var_X = a_s / 500. + var_Y;
  float var_Z = var_Y - b_s / 200.;

  if ( pow(var_Y,3) > 0.008856 ) var_Y = pow(var_Y,3);
  else                      var_Y = ( var_Y - 16. / 116. ) / 7.787;
  if ( pow(var_X,3) > 0.008856 ) var_X = pow(var_X,3);
  else                      var_X = ( var_X - 16. / 116. ) / 7.787;
  if ( pow(var_Z,3) > 0.008856 ) var_Z = pow(var_Z,3);
  else                      var_Z = ( var_Z - 16. / 116. ) / 7.787;

  float X = 95.047 * var_X ;    //ref_X =  95.047     Observer= 2°, Illuminant= D65
  float Y = 100.000 * var_Y  ;   //ref_Y = 100.000
  float Z = 108.883 * var_Z ;    //ref_Z = 108.883


  var_X = X / 100. ;       //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
  var_Y = Y / 100. ;       //Y from 0 to 100.000
  var_Z = Z / 100. ;      //Z from 0 to 108.883

  float var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
  float var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
  float var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

  if ( var_R > 0.0031308 ) var_R = 1.055 * pow(var_R , ( 1 / 2.4 ))  - 0.055;
  else                     var_R = 12.92 * var_R;
  if ( var_G > 0.0031308 ) var_G = 1.055 * pow(var_G , ( 1 / 2.4 ) )  - 0.055;
  else                     var_G = 12.92 * var_G;
  if ( var_B > 0.0031308 ) var_B = 1.055 * pow( var_B , ( 1 / 2.4 ) ) - 0.055;
  else                     var_B = 12.92 * var_B;

  *R = (uint8_t) (var_R * 255.);
  *G = (uint8_t) (var_G * 255.);
  *B = (uint8_t) (var_B * 255.);
}

/** Reads a .png file and converts it to a color matrix */
Image* img_png_read_from_file (char* filename) {
	Image* img = malloc(sizeof(Image));

  FILE* fp = fopen(filename, "rb");

  if (!fp) {
    printf("Error al intentar leer %s\n", filename);
    printf("Asegurate de que el archivo exista");
    abort();
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  int width        = png_get_image_width(png, info);
  int height       = png_get_image_height(png, info);
  uint8_t color_type = png_get_color_type(png, info);
  uint8_t bit_depth  = png_get_bit_depth(png, info);

  if (bit_depth == 16)
    png_set_strip_16(png);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_type_GRAY_ALPHA is always 8 or 16bit depth.
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  if (color_type == PNG_COLOR_TYPE_RGB ||
    color_type == PNG_COLOR_TYPE_GRAY ||
    color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if (color_type == PNG_COLOR_TYPE_GRAY ||
    color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

	uint8_t** row_pointers = calloc(height, sizeof(uint8_t*));
  for(int row = 0; row < height; row++) {
		row_pointers[row] = calloc(width * 4, sizeof(uint8_t));
  }

  png_read_image(png, row_pointers);

	img -> pixels = calloc(height, sizeof(Color*));
	for (int row = 0; row < height; row++) {
		img -> pixels[row] = calloc(width, sizeof(Color));
		for(int col = 0; col < width; col++) {
			uint8_t* px = &(row_pointers[row][col * 4]);
			img -> pixels[row][col] = rgb_2_lab(px[0], px[1], px[2]);
		}
		free(row_pointers[row]);
	}
	free(row_pointers);

  fclose(fp);

	png_destroy_read_struct(&png, &info, NULL);

	img -> width = width;
	img -> height = height;

	return img;
}

/** Pinta el cuadrado que va de (row, col) a (row + size - 1, col + size - 1) */
void img_square_paint(Image* img, int row, int col, int size, Color c) {
  /* Asignamos el color a la fila superior del cuadrado */
  for (int curr_col = col; curr_col < col + size; curr_col++) {
    img -> pixels[row][curr_col] = c;
  }

  /* La fila superior de este cuadrado como un arreglo */
  Color* top_row = img -> pixels[row] + col;

  /* Para cada una de las demas filas del cuadrado */
  for (int curr_row = row + 1; curr_row < row + size; curr_row++) {
    /* Tomamos la fila como un arreglo */
    Color* current_row = img -> pixels[curr_row] + col;
    /* Copiamos todo lo de la fila superior a esta fila de una sola pasada */
    memcpy(current_row, top_row, size * sizeof(Color));
  }
}

/** Writes a color matrix on to a .png file */
void img_png_write_to_file(Image* img, char* filename) {
	FILE *fp = fopen(filename, "wb");
  if (!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    img -> width, img -> height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

	uint8_t** row_pointers = calloc(img -> height, sizeof(uint8_t*));
  for(int row = 0; row < img -> height; row++) {
		row_pointers[row] = calloc(img -> width * 4, sizeof(uint8_t));
		for(int col = 0; col < img -> width; col++) {
			uint8_t* px = &(row_pointers[row][col * 4]);
			lab_2_rgb(img -> pixels[row][col], &px[0], &px[1], &px[2]);
			px[3] = 255;
		}
  }

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(int row = 0; row < img -> height; row++) {
    free(row_pointers[row]);
  }
  free(row_pointers);

  fclose(fp);

	if (png && info) png_destroy_write_struct(&png, &info);
}

/** Frees the resources used by the image */
void img_png_destroy(Image* img) {
	for(int row = 0; row < img -> height; row++) {
		free(img -> pixels[row]);
	}
	free(img -> pixels);
	free(img);
}


/* Funciones */

typedef struct QuadNode {

  int x_min;
  int x_max;
  int y_min;
  int y_max;
  int size;
  float sumL1;
  float sumL2;
  float suma1;
  float suma2;
  float sumb1;
  float sumb2;
  float sL;
  float sa;
  float sb;
  float s;
  float n;

  Color color;

  struct QuadNode* H1;
  struct QuadNode* H2;
  struct QuadNode* H3;
  struct QuadNode* H4;

} QuadNode;

QuadNode* quadnode_init(){
  QuadNode* Nodo = (QuadNode*) calloc(1, sizeof(QuadNode));

  Nodo->H1 = NULL;
  Nodo->H2 = NULL;
  Nodo->H3 = NULL;
  Nodo->H4 = NULL;

  return Nodo;
}


QuadNode* arbol_init(Image* img, int x_min, int x_max, int y_min, int y_max){
  QuadNode* Arbol = quadnode_init();

  if (x_max == x_min || y_max == y_min){
    Arbol->color = img->pixels[x_min][y_min];
    Arbol->x_min = x_min;
    Arbol->x_max = x_max;
    Arbol->y_min = y_min;
    Arbol->y_max = y_max;

  }
  else{

  Arbol->H1 = arbol_init(img, x_min, (x_max + x_min)/2, y_min, (y_max + y_min)/2);
  Arbol->H2 = arbol_init(img, (x_max + x_min)/2 + 1, x_max, y_min, (y_max + y_min)/2);
  Arbol->H3 = arbol_init(img, x_min, (x_max + x_min)/2, (y_max + y_min)/2 + 1, y_max);
  Arbol->H4 = arbol_init(img, (x_max + x_min)/2 + 1, x_max, (y_max + y_min)/2 + 1, y_max);

  Arbol->x_min = Arbol->H3->x_min;
  Arbol->x_max = Arbol->H4->x_max;
  Arbol->y_min = Arbol->H1->y_min;
  Arbol->y_max = Arbol->H4->y_max;
  }

  return Arbol;
}

/*
void print_tree1(QuadNode* root){
  printf("X %d Y %d S %d\n", root->x, root->y, root->size);

  if (root->H1){
      print_tree1(root->H1);
  }
  if (root->H2){
      print_tree1(root->H2);
  }
  if (root->H3){
      print_tree1(root->H3);
  }
  if (root->H4){
      print_tree1(root->H4);
  }
}*/
/*
void paint_image(Image* img, QuadNode* root){
  if (root->H1 == NULL){
    img_square_paint(
        img, // Imagen donde se pintara el cuadrado
        root->x, // Fila donde comienza el cuadrado
        root->y, // Columna donde comienza el cuadrado
        root->size, // Largo del lado del cuadrado
        (Color) {.L = root->color.L, .a = root->color.a, .b = root->color.b} // Color a pintar
        );
  }
  else{
    paint_image(img, root->H1);
    paint_image(img, root->H2);
    paint_image(img, root->H3);
    paint_image(img, root->H4);
  }
}*/

void desv_est1(QuadNode* root){

  if (root->H1 == NULL){
    root->sumL1 = root->color.L;
    root->sumL2 = pow(root->color.L, 2);
    root->suma1 = root->color.a;
    root->suma2 = pow(root->color.a, 2);
    root->sumb1 = root->color.b;
    root->sumb2 = pow(root->color.b, 2);
    root->sL = 0;
    root->sa = 0;
    root->sb = 0;
    root->s = 0;
    root->n = 1;
    return;
  }
  else{
    desv_est1(root->H1);
    desv_est1(root->H2);
    desv_est1(root->H3);
    desv_est1(root->H4);

    root->sumL1 = root->H1->sumL1 + root->H2->sumL1 + root->H3->sumL1 + root->H4->sumL1;
    root->sumL2 = root->H1->sumL2 + root->H2->sumL2 + root->H3->sumL2 + root->H4->sumL2;
    root->suma1 = root->H1->suma1 + root->H2->suma1 + root->H3->suma1 + root->H4->suma1;
    root->suma2 = root->H1->suma2 + root->H2->suma2 + root->H3->suma2 + root->H4->suma2;
    root->sumb1 = root->H1->sumb1 + root->H2->sumb1 + root->H3->sumb1 + root->H4->sumb1;
    root->sumb2 = root->H1->sumb2 + root->H2->sumb2 + root->H3->sumb2 + root->H4->sumb2;
    root->n = root->H1->n + root->H2->n + root->H3->n + root->H4->n;
    root->sL = sqrt((root->sumL2) / root->n - pow((root->sumL1) / root->n, 2));
    root->sa = sqrt((root->suma2) / root->n - pow((root->suma1) / root->n, 2));
    root->sb = sqrt((root->sumb2) / root->n - pow((root->sumb1) / root->n, 2));
    root->s = (root->sL + root->sa + root->sb) / 3;
    root->color = (Color) {.L = (root->sumL1 / root->n), .a = (root->suma1 / root->n), .b = (root->sumb1 / root->n)};
    return;
  }

}

int filter_alpha1(QuadNode* root, double alpha, Image* img, int tipo){
  if (root->s <= alpha){
    if (tipo == 1){
      if (root->H1){
        free_tree(root->H1);
        free_tree(root->H2);
        free_tree(root->H3);
        free_tree(root->H4);
        root->H1 = NULL;
        root->H2 = NULL;
        root->H3 = NULL;
        root->H4 = NULL;
      }
      img_square_paint(img, root->x_min, root->y_min, root->x_max - root->x_min + 1, root->color); //Es size + 1 ??
    }
    return 1;
  }
  else{
    int* suma = calloc(1, sizeof(int));
    *suma += filter_alpha1(root->H1, alpha, img, tipo);
    *suma += filter_alpha1(root->H2, alpha, img, tipo);
    *suma += filter_alpha1(root->H3, alpha, img, tipo);
    *suma += filter_alpha1(root->H4, alpha, img, tipo);
    int n = *suma;
    free(suma);
    return n;
  }
}

double compression1(QuadNode* root, Image* img, int h, int min, int max){
  double alpha = (min + max) / 2;
  //printf("Alpha: %f\n", alpha);
  int nh = filter_alpha1(root, alpha, img, 0);
  printf("Hojas 1: %i\n", nh);
  if (nh < h){
    int nh2 = filter_alpha1(root, alpha - 1, img, 0);
    if (nh2 < nh || nh2 < h){
      compression1(root, img, h, min, alpha - 1);
      printf("Hojas 2: %i\n", nh2);
    }
    else{
      return alpha;
    }
  }
  else{
    compression1(root, img, h, alpha + 1, max);
  }
}


double* desv_est(QuadNode* nodo, double* S_n, double* sum_n, int* n, double* x_n){
  double* data = calloc(4, sizeof(double));

  if (nodo->H1 == NULL){
    x_n[0] = nodo->color.L;
    x_n[1] = nodo->color.a;
    x_n[2] = nodo->color.b;

    for (int i=0; i < 3; i++){
      if (n[i] != 0){
        S_n[i] += (x_n[i] - sum_n[i] / n[i]) * (x_n[i] - (sum_n[i] + x_n[i]) / (n[i] + 1));
      }
      sum_n[i] += x_n[i];
      n[i] += 1;
    }
  }
  else {
    double* desv_1 = desv_est(nodo->H1, S_n, sum_n, n, x_n);
    double* desv_2 = desv_est(nodo->H2, S_n, sum_n, n, x_n);
    double* desv_3 = desv_est(nodo->H3, S_n, sum_n, n, x_n);
    double* desv_4 = desv_est(nodo->H4, S_n, sum_n, n, x_n);
    free(desv_1);
    free(desv_2);
    free(desv_3);
    free(desv_4);
  }

  data[0] = (sqrt(S_n[0] / n[0]) + sqrt(S_n[1] / n[1]) + sqrt(S_n[2] / n[2])) / 3;
  data[1] = sum_n[0] / n[0];
  data[2] = sum_n[1] / n[1];
  data[3] = sum_n[2] / n[2];

  return data;
}



int filter_alpha(double alpha, QuadNode* nodo, Image* img, int p){

  if (nodo != NULL){
    if (nodo->H1 == NULL){
      return 1;
    }
    double S_n[3] = {0, 0, 0};
    double sum_n[3] = {0, 0, 0};
    int n[3] = {0, 0, 0};
    double x_n[3] = {0, 0, 0};
    double* data = desv_est(nodo, S_n, sum_n, n, x_n);
    if (data[0] <= alpha){
      nodo->color = (Color) {.L = data[1], .a = data[2], .b = data[3]};

      if (p == 1){
        free_tree(nodo->H1);
        free_tree(nodo->H2);
        free_tree(nodo->H3);
        free_tree(nodo->H4);
        nodo->H1 = NULL;
        nodo->H2 = NULL;
        nodo->H3 = NULL;
        nodo->H4 = NULL;
        free(data);
        return 1;
      }
      return 1;
    }
    free(data);
    int suma = 0;
    suma += filter_alpha(alpha, nodo->H1, img, p);
    suma += filter_alpha(alpha, nodo->H2, img, p);
    suma += filter_alpha(alpha, nodo->H3, img, p);
    suma += filter_alpha(alpha, nodo->H4, img, p);

    return suma;
  }
  return 0;
}


int compression(QuadNode* root, Image* img, int max_h, int i, int f){
  
  int alpha = (int) (i + f) / 2;
  int alpha_1 = filter_alpha((double) alpha, root, img, 0);

  if (alpha_1 < max_h){
    int alpha_2 = filter_alpha((double) alpha - 1, root, img, 0); 

    if (alpha_2 >= alpha_1 && alpha_2 > max_h){
      return alpha;
    }

    return compression(root, img, max_h, i, alpha - 1);
  }
  return compression(root, img, max_h, alpha + 1, f);
}


void free_tree(QuadNode* root){
  if (root->H1 == NULL){
    free(root);
  }
  else{
    free_tree(root->H1);
    free_tree(root->H2);
    free_tree(root->H3);
    free_tree(root->H4);
    free(root);
  }
}

int count_h(QuadNode* root){
  int h = 0;
  if (root->H1 == NULL){
    return 1;
  }
  else{
    h += count_h(root->H1);
    h += count_h(root->H2);
    h += count_h(root->H3);
    h += count_h(root->H4);
  }
  return h;
}