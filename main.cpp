#include <GL/glut.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <strings.h>

//ALUNOS:
//VITOR VIEIRA
//GABRIEL AGUIAR
//VITOR CARLOS
//VICTORIA MARIA

float theta=0.0;
GLfloat angulo, aspecto;
GLfloat anguloy;
GLfloat anguloz;
GLfloat aviaox, aviaoy, aviaoz;


//OBSERVADOR
GLfloat eyex = 0;
GLfloat eyey = 450;
GLfloat eyez = 150;

void timer (int);
void timeGameOver (int);
//////////////////////////

//inicializa par?metros de renderiza??o

GLuint texture[4];

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};

typedef struct Image Image;

#define checkImageWidth 64
#define checkImageHeight 64

GLubyte checkImage[checkImageWidth][checkImageHeight][4];

void makeCheckImage(void){
    int i, j, c;
    for (i = 0; i < checkImageWidth; i++) {
        for (j = 0; j < checkImageHeight; j++) {
            c = ((((i&0x8)==0)^((j&0x8)==0)))*255;

            checkImage[i][j][0] = (GLubyte) c;
            checkImage[i][j][1] = (GLubyte) c;
            checkImage[i][j][2] = (GLubyte) c;
            checkImage[i][j][3] = (GLubyte) c;
        }
    }
}

/*==============================================================================
                                    IMAGE LOADER
==============================================================================*/

int ImageLoad(char *filename, Image *image) {
    FILE *file;

    unsigned long size; // size of the image in bytes.
    unsigned long i; // standard counter.
    unsigned short int planes; // number of planes in image (must be 1)
    unsigned short int bpp; // number of bits per pixel (must be 24)
    char temp; // temporary color storage for bgr-rgb conversion.
    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL){
        printf("File Not Found : %s\n",filename);
        return 0;
    }
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);
    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
        printf("Error reading width from %s.\n", filename);
        return 0;
    }
    //printf("Width of %s: %lu\n", filename, image->sizeX);
    // read the height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
        printf("Error reading height from %s.\n", filename);
        return 0;
    }
    //printf("Height of %s: %lu\n", filename, image->sizeY);
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;
    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }
    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }
    // read the bitsperpixel
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
       printf("Error reading bpp from %s.\n", filename);
        return 0;
    }
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);
    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }
    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }
    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }
    // we're done.
    return 1;
}

Image * loadTexture(int id){
    Image *image1;
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
        printf("Error allocating space for image");
        exit(0);
    }
    switch(id){
        case 0:

            if (!ImageLoad("C:\\Users\\vitor\\Desktop\\projetos icg\\aviaoem3d\\stone.bmp", image1)) {
                exit(1);
            }break;
        case 1:
            if (!ImageLoad("C:\\Users\\vitor\\Desktop\\projetos icg\\aviaoem3d\\aviao.bmp", image1)) {
                exit(1);
            }break;
        case 2:
            if (!ImageLoad("C:\\Users\\vitor\\Desktop\\projetos icg\\aviaoem3d\\gameover.bmp", image1)) {
                exit(1);
            }break;
    }
    return image1;
}


void prepararMaterial(){

GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
GLint especMaterial = 60;
GLfloat ciano [] = {0.f, .8f, .8f, 1.f};

//glMaterialfv(GL_FRONT,GL_SPECULAR,ciano);
 glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

}
void prepararIluminacao(){

    GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};

    GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};

    GLfloat luzEspecular[4]={1.0,1.0,1.0,1.0};

    GLfloat posicaoLuz[4]={-2000.0f,0,1800,10.0};

    glLightfv(GL_LIGHT0,GL_AMBIENT,luzAmbiente);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,luzDifusa);
    glLightfv(GL_LIGHT0,GL_SPECULAR,luzEspecular);
    glLightfv(GL_LIGHT0,GL_POSITION,posicaoLuz);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,luzAmbiente);
}

void inicializar(void)
{
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    //gluOrtho2D(60000.0f, 60000.0f, 60000.0f, 60000.0f);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

     angulo=45;
     anguloy = 0;
     anguloz = 0;
     aviaox = 0;
     aviaoy = 0;
     aviaoz = 0;

     //textura
     //gluOrtho2D(0.0f, 600.0f, 0.0f, 6000.0f);


     glEnable(GL_DEPTH_TEST);
     glDepthFunc(GL_LESS);
     Image *image1 = loadTexture(0);
     Image *image2 = loadTexture(1);
     Image *image3 = loadTexture(2);
     if(image1 == NULL){
        printf("Image was not returned from loadTexture\n");
        exit(0);
    }
    makeCheckImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Create Texture
    glGenTextures(4, texture);

    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image2->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image3->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//fun??o callback chamada para fazer o desenho
void chao(void)
{
     //limpa a janela de visualiza??o com a cor de fundo especificada
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     //textura do chao
     glPushMatrix();

     glBindTexture(GL_TEXTURE_2D, texture[0]);
     glColor3f(1.0f,1.0f, 1.0f);
     glBegin(GL_QUADS);
     glTexCoord2f(50.0f, -50.0f); glVertex3f(5000,0,-8500);
     glTexCoord2f(50.0f, 50.0f); glVertex3f(5000,0,1200);
     glTexCoord2f(-50.0f, 50.0f); glVertex3f(-5000,0,1200);
     glTexCoord2f(-50.0f, -50.0f); glVertex3f(-5000,0,-8500);
     glEnd();


     glPopMatrix();
     glFlush();
}

float Overy = 0;

void gameOver(){
     glPushMatrix();

     glTranslatef(0,-5000,0);
     glBindTexture(GL_TEXTURE_2D, texture[2]);
     glColor3f(1.0f, 1.0f, 1.0f);
     glBegin(GL_QUADS);
     glTexCoord2f(1.0f, 1.0f); glVertex3f(aviaox+50,230,aviaoz+385);
     glTexCoord2f(1.0f, 0.0f); glVertex3f(aviaox+60,100,aviaoz+400);
     glTexCoord2f(0.0f, 0.0f); glVertex3f(aviaox-60,100,aviaoz+400);
     glTexCoord2f(0.0f, 1.0f); glVertex3f(aviaox-50,230,aviaoz+385);
     glEnd();
     glPopMatrix();
     glFlush();
}

void setarGameOver(){
     glPushMatrix();

     glTranslatef(0,-100,0);
     glBindTexture(GL_TEXTURE_2D, texture[2]);
     glColor3f(1.0f, 1.0f, 1.0f);
     glBegin(GL_QUADS);
     glTexCoord2f(1.0f, 1.0f); glVertex3f(aviaox+50,230,aviaoz+385);
     glTexCoord2f(1.0f, 0.0f); glVertex3f(aviaox+60,100,aviaoz+400);
     glTexCoord2f(0.0f, 0.0f); glVertex3f(aviaox-60,100,aviaoz+400);
     glTexCoord2f(0.0f, 1.0f); glVertex3f(aviaox-50,230,aviaoz+385);
     glEnd();

     glPopMatrix();

     glutSwapBuffers();
     Sleep(6000);
}

void aviao(void){

    //corpo Lado1
     glPushMatrix();

     glTranslatef(aviaox,aviaoy+120,aviaoz+150);
     glRotatef(anguloz,0,0,1);
     glTranslatef(0,10, 80);

     glScalef(30,30,30);
     //glRotatef(0,0,0,1);

     glBindTexture(GL_TEXTURE_2D, texture[1]);
     glColor3f(1.0f,1.0f, 1.0f);
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.1f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.0f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.1f, -1.0f);
     glEnd();
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.1f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.3f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.3f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.1f, -1.0f);
     glEnd();
     glBegin(GL_QUADS);
     glVertex3f(-0.2f, -0.3f, 0.8f);
     glVertex3f(0.0f, -0.4f, 0.8f);
     glVertex3f(0.0f, -0.4f, -1.0f);
     glVertex3f(-0.2f, -0.3f, -1.0f);

     glEnd();
     //corpo Lado2
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.1f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.0f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.1f, -1.0f);
     glEnd();
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.1f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.3f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.3f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.1f, -1.0f);
     glEnd();
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.3f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.4f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.4f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.3f, -1.0f);
     glEnd();

     //Traseira
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.0f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.1f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.3f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.4f, 0.8f);
     glEnd();
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.0f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.1f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.3f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.4f, 0.8f);
     glEnd();

     //Frente
     glBegin(GL_TRIANGLES);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.0f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.1f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.1f, -1.2f);
     glEnd();
     glBegin(GL_TRIANGLES);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.0f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.1f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.1f, -1.2f);
     glEnd();

     //vidro
     //glColor3f(1.0f, 1.0f, 0.0f);
     glBegin(GL_QUADS);
     glVertex3f(-0.2f, -0.1f, -1.0f);
     glVertex3f(0.0f, -0.1f, -1.2f);
     glVertex3f(0.0f, -0.15f, -1.2f);
     glVertex3f(-0.2f, -0.3f, -1.0f);
     glBegin(GL_QUADS);
     glVertex3f(0.2f, -0.1f, -1.0f);
     glVertex3f(0.0f, -0.1f, -1.2f);
     glVertex3f(0.0f, -0.15f, -1.2f);
     glVertex3f(0.2f, -0.3f, -1.0f);
     glEnd();

     //bico
     //cima

     //glBindTexture(GL_TEXTURE_2D, texture[1]);
     glBegin(GL_TRIANGLES);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.15f, -1.2f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.3f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.25f, -1.5f);
     glEnd();
     glBegin(GL_TRIANGLES);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.15f, -1.2f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.3f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.25f, -1.5f);
     glEnd();
     //baixo
     glBegin(GL_TRIANGLES);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.4f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.3f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.25f, -1.5f);
     glEnd();
     glBegin(GL_TRIANGLES);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.4f, -1.0);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.3f, -1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, -0.25f, -1.5f);
     glEnd();

     //Asa
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-1.5f, -0.18f, -0.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.9f, -0.18f, -0.4f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.18f, -0.6f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.18f, -0.2f);

     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(1.5f, -0.18f, -0.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.9f, -0.18f, -0.4f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.18f, -0.6f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.18f, -0.2f);
     glEnd();
     //traseira
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.4f, 1.0f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.4f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
     glEnd();

     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.6f, -0.18f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.6f, -0.18f, 0.9f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.18f, 0.75f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(-0.2f, -0.18f, 0.0f);
     glEnd();
     glBegin(GL_QUADS);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.6f, -0.18f, 0.8f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.6f, -0.18f, 0.9f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.18f, 0.75f);
     glTexCoord2f(-1.0f, 1.0f); glVertex3f(0.2f, -0.18f, 0.0f);
     glEnd();
     glBindTexture(GL_TEXTURE_2D, 1);
     glutPostRedisplay();
     glPopMatrix();

    glFlush();
}




void desenharSol()
{
    //glPushMatrix();

    //glBindTexture(GL_TEXTURE_2D, texture[0]);
    //glColor3f(1.0f,1.0f, 0.0f);
    //glTranslatef(-150.0f,180.0f,0.0f);
	//glutSolidSphere(10,100,10);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glPopMatrix();
	//glFlush();
}


//Fun??o usada para especificar o volume de visualiza??o
void configurarVisualizacao(void)
{
     //Especifica sistema de coordenadas de proje??o
     glMatrixMode(GL_PROJECTION);
     //Inicializa sistema de coordenadas de proje??o
     glLoadIdentity();

     //Especifica a proje??o perspectiva
     gluPerspective(angulo, aspecto,0.1,50000);

     //Especifica sistema de coordenadas do modelo
     glMatrixMode(GL_MODELVIEW);
     //Inicializa sistema de coordenadas do modelo
     glLoadIdentity();

     //Especifica posi??o do observador e do alvo
     //glRotatef(anguloy+10,0,100,0);

     gluLookAt(aviaox,aviaoy+200,aviaoz+500, aviaox, aviaoy+50,aviaoz, 0,1,0);
     //glRotatef(eyex, 1, 0, 0);

}

//Fun??o callback chamada quando o tamanho da janela ? alterado
void alterarTamanhoJanela(GLsizei w, GLsizei h)
{
     //Para previnir uma divis?o por zero
     if( h == 0 ) h = 1;

     //Espec?fica o tamanho da viewport
     glViewport(0, 0, w, h);

     //Calcula a corre??o de aspecto
     aspecto = (GLfloat)w/(GLfloat)h;

     configurarVisualizacao();
}

void responderTeclado (unsigned char key, int x, int y)
{
    switch (key) {
      case 'A':
      case 'a':
        aviaox -= 10;
        if(anguloz < 80)
            anguloz+=10;
        break;
      case 'D':
      case 'd':
        aviaox += 10;
        if(anguloz > -80)
            anguloz-=10;
        break;
      case 'W':
      case 'w':
        aviaoy += 10;
        break;
      case 'S':
      case 's':
        aviaoy -= 10;
        break;
      case 'Z':
      case 'z':
        aviaoz += 10;
        break;
      case 'X':
      case 'x':
        aviaoz -= 10;
        eyez-=5;
        configurarVisualizacao();
        glutPostRedisplay();
        break;
      case 'p':
          glDisable(GL_LIGHT0);
          glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
          break;
      case 'o':
          glEnable(GL_LIGHT0);
          glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
          break;
   }
   configurarVisualizacao();
	glutPostRedisplay();

}

void drawBitmapText(char *string,float x,float y,float z)
{
    (GL_TEXTURE_2D, texture[0]);
	char *c;
	glRasterPos3f(x, y,z);
	while(*string)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *string++);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void timer (int){
    glutTimerFunc(5, timer, 0);
    //rotNuvem += 0.1;
    aviaoz -= 10;
    configurarVisualizacao();
    glutPostRedisplay();

    if(aviaoy<=-100.0){
         setarGameOver();

         angulo=45;
         anguloy = 0;
         anguloz = 0;
         aviaox = 0;
         aviaoy = 0;
         aviaoz = 0;
         eyex = 0;
         eyey = 450;
         eyez = 150;


    }
    }
void timeGameOver (int){
     glutTimerFunc(5000, timeGameOver,0);
}

//Fun??o callback chamada para gerenciar eventos do mouse
void eventoMouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON)
        if (state == GLUT_DOWN) {  //Zoom-in
                  //aviaoz -= 10;
                  //anguloy -= 10;
          }
    if(button == GLUT_RIGHT_BUTTON)
         if (state == GLUT_DOWN) { //Zomm-out
                   //anguloy += 10;
                   aviaoz += 5;
          }
    configurarVisualizacao();
    glutPostRedisplay();
}



 int random_number(int min_num, int max_num)
    {
        int result = 0, low_num = 0, hi_num = 0;

        if (min_num < max_num)
        {
            low_num = min_num;
            hi_num = max_num + 1; // include max_num in output
        } else {
            low_num = max_num + 1; // include max_num in output
            hi_num = min_num;
        }

        srand(time(NULL));
        result = (rand() % (hi_num - low_num)) + low_num;
        return result;
    }


void arvore1(void)
{

    GLfloat xarvore, yarvore, zarvore;
    xarvore = -100.0f;
    yarvore=2.0f;
    zarvore=50.0f;
    for(GLfloat xarvoreRS = -2500;xarvoreRS<=2500;xarvoreRS+=65){
        for(GLfloat zarvoreRS=500;zarvoreRS>=-6000;zarvoreRS-=65){
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glScalef(1,-80.0,1);
            glTranslatef(xarvoreRS,yarvore,zarvoreRS);
            glColor3f(0.600f,0.31f,0.0f);
            glutSolidCube(5);
            glBindTexture(GL_TEXTURE_2D, 0);
            glPopMatrix();

            glPushMatrix();

            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glTranslatef(xarvoreRS,45.0f,zarvoreRS);
            glColor3f( 0.0f,1.0f, 0.1f );
            glutSolidCube(10);
            glBindTexture(GL_TEXTURE_2D, 0);

            glPopMatrix();
        }
    }



//folha
    //glPushMatrix();

      //glBindTexture(GL_TEXTURE_2D, texture[0]);
      //glTranslatef(-100.0f,45.0f,50.0f);
      //glColor3f( 0.0f,1.0f, 0.1f );
      //glutSolidSphere( 15 ,30 , 40 );
	  //glBindTexture(GL_TEXTURE_2D, 0);

	//glPopMatrix();

	glFlush();
}


void display()
{
    prepararMaterial();
    prepararIluminacao();
    chao();
    arvore1();
    aviao();
    gameOver();
    //setarGameOver();
    desenharSol();
    //drawBitmapText("Pontos:",80+aviaox,210+eyey,0+aviaoz);

    glutSwapBuffers();
}

//Programa Principal
int main(int argc, char *argv[])
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(350,300);
    glutCreateWindow("Visualizacao 3D");
    glutDisplayFunc(display);
    glutReshapeFunc(alterarTamanhoJanela);
    glutMouseFunc(eventoMouse);
    glutKeyboardFunc(responderTeclado);
    glutTimerFunc(0, timer, 0);
    inicializar();
    glutMainLoop();

}
