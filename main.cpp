
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>		   // Open Graphics Library (OpenGL) header
#include <GL/glut.h>	   // The GL Utility Toolkit (GLUT) Header

#include "stb_image.h"     //lib for the textures

#include <time.h>          //for the random type generator


#define Start_game 1
#define Exit 2

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MOVES 30;

typedef struct {
    int width;
	int height;
	char* title;

	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;

glutWindow win;


//stuct for each cube
struct cube{
	float xpos;                              //x coordinate  used for selecting
	float ypos;                              //y coordinate  used for selecting
	int type;                                //type of cube eg:red(0) blue(1) rock(2)...
	int i;                                   //position i in array cubes[][]
	int j;                                   //position j in array cubes[][]
};

int start=0;                                 //flag for when Start Game is selected

int num=0;			                         //for the random type generator

struct cube cubes[15][15];                   //main array containing the cubes

GLdouble ox=0.0, oy=0.0, oz=0.0;             //wolrd coordinates for the mouse

int mouse;                                   //flag for when the mouse is pressed

int sel;                                     //counter for selection of two cubes
cube firstCube,secondCube;                   //the two cubes to be switched

int movesLeft;                               //remaining moves
int moves[2];

int score=0;                                 //displayed score
int r[99999];                                //a large array containg the digits of the score used for printing 

int eat=0;                                   //flag for when a cube eats another cube

GLuint *textures = new GLuint[3];            //array containg the three texture
const char* filename ;                       //filename




//--------------3D ROTATION----------------------------
//angle of rotation of the camera
float angle=0.0f;

//vectors for the camera's direction
float lx=0.0f,lz=-1.0f;

// XZ position of the camera
float x=0.0f,z=2.0f;

//function that moves the camera in the world using the arrow keys
void processSpecialKeys(int key, int xx, int yy){
	switch (key) {
		case GLUT_KEY_UP :
			//moving towards the new direction
			x+=lx*0.05f;
			z+=lz*0.05f;
			break;
		case GLUT_KEY_RIGHT :
			angle+=0.01f;
			//calculating the new direction
			lx=sin(angle);
			lz=-cos(angle);
			break;
		case GLUT_KEY_DOWN :
			x-=lx*0.05f;
			z-=lz*0.05f;
			break;
		case GLUT_KEY_LEFT :
			angle-=0.01f;
			lx=sin(angle);
			lz=-cos(angle);
			break;
	}
}
//--------------3D ROTATION----------------------------


//---------------------------------TEXTURE-----------------------------------------------------
//function to load the RAW file
void loadTexture(GLuint texture, const char* filename){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else{
		printf("Failed to load texture : %s\n",filename);
	}
	stbi_image_free(data);
}

void FreeTexture(GLuint texture){
	glDeleteTextures(1, &texture);
}
//------------------------------TEXTURE--------------------------------------------------

//--------------------------CUBE---------------------------------------------
//function that creates the cube by drawing 6 squares
//glNormal3f is used for the lighting to work 
void drawCube(float size){
	glBegin(GL_QUADS);
	//Square 1
	    glNormal3f(1.0f,0.0f,0.0f);
	    glTexCoord2f(0.0f,1.0f); glVertex3f(size,size,size);
	    glTexCoord2f(0.0f,0.0f); glVertex3f(size,-size,size);
	    glTexCoord2f(1.0f,0.0f); glVertex3f(size,-size,-size);
	    glTexCoord2f(1.0f,1.0f); glVertex3f(size,size,-size);
  	//Square 2
	    glNormal3f(0.0f,0.0f,-1.0f);
	    glTexCoord2f(0.0f,1.0f); glVertex3f(size,size,-size);
	    glTexCoord2f(0.0f,0.0f); glVertex3f(size,-size,-size);
	    glTexCoord2f(1.0f,0.0f); glVertex3f(-size,-size,-size);
	    glTexCoord2f(1.0f,1.0f); glVertex3f(-size,size,-size);
  	//Square 3
	    glNormal3f(-1.0f,0.0f,0.0f);
	    glTexCoord2f(0.0f,1.0f); glVertex3f(-size,size,-size);
	    glTexCoord2f(0.0f,0.0f); glVertex3f(-size,-size,-size);
	    glTexCoord2f(1.0f,0.0f); glVertex3f(-size,-size,size);
	    glTexCoord2f(1.0f,1.0f); glVertex3f(-size,size,size);
	//Square 4
	    glNormal3f(0.0f,0.0f,1.0f);
	    glTexCoord2f(0.0f,1.0f); glVertex3f(-size,size,size);
	    glTexCoord2f(0.0f,0.0f); glVertex3f(-size,-size,size);
	    glTexCoord2f(1.0f,0.0f); glVertex3f(size,-size,size);
	    glTexCoord2f(1.0f,1.0f); glVertex3f(size,size,size);
  	//Square 5
	    glNormal3f(0.0f,1.0f,0.0f);
	    glTexCoord2f(0.0f,1.0f); glVertex3f(-size, size,-size);
	    glTexCoord2f(0.0f,0.0f); glVertex3f(-size, size, size);
	    glTexCoord2f(1.0f,0.0f); glVertex3f( size, size, size);
	    glTexCoord2f(1.0f,1.0f); glVertex3f( size, size,-size);
  	//Square 6
	    glNormal3f(1.0f,-1.0f,0.0f);
	    glTexCoord2f(0.0f,1.0f); glVertex3f(-size,-size, size);
	    glTexCoord2f(0.0f,0.0f); glVertex3f(-size,-size,-size);
	    glTexCoord2f(1.0f,0.0f); glVertex3f( size,-size,-size);
	    glTexCoord2f(1.0f,1.0f); glVertex3f( size,-size, size);
	glEnd();
}
//-------------------------CUBE--------------------------------


//------------------------MOUSE--------------------------------
//function responsible for converting the window coordinates to world coordinates
void mousebutton(int button, int state, int x, int y){
	GLint viewport[4];
	GLdouble modelview[16], projection[16];
	GLfloat wx = x, wy, wz;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && start==1){
		glGetIntegerv(GL_VIEWPORT, viewport);
		y = viewport[3] - y;
		wy = y;
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
		gluUnProject(wx, wy, wz, modelview, projection, viewport, &ox, &oy, &oz);
		glutPostRedisplay();
		mouse=1;
	}
	else if(start==1){
		mouse=0;
	}
}
//------------------------MOUSE--------------------------------


//-----------------------MOVE CUBES--------------------------------------------------------------------
//function that takes the two selected cubes and changes their type practically changes their possition
void switchCubes(cube theCube, cube otherCube){
	//important info for the first cube
	int fci=theCube.i;
	int fcj=theCube.j;
	int fctype=theCube.type;

	//important info for the second cube
	int sci=otherCube.i;
	int scj=otherCube.j;
	int sctype=otherCube.type;

	//this flag is used in boom()
	eat=1;

	//switch the type effecting the main cubes array
	cubes[fci][fcj].type=sctype;
	cubes[sci][scj].type=fctype;
}


//function that selects the two cubes
void select(cube theCube ,float size){
	//select the first cube
	if(sel==0 && mouse==1
		&&(ox>(theCube.xpos-size) && ox<(theCube.xpos+size))       //cursor within cube bounds
		&&(oy>(theCube.ypos-size) && oy<(theCube.ypos+size))){

		sel=1;
		mouse=0;
		firstCube=theCube;
	}
	//select the second cube
	if(sel==1 && mouse==1
		&&(ox>(theCube.xpos-size) && ox<(theCube.xpos+size))
		&&(oy>(theCube.ypos-size) && oy<(theCube.ypos+size))){

		int fci=firstCube.i;
		int fcj=firstCube.j;
		int tci=theCube.i;
		int tcj=theCube.j;

		if(!(abs(tci-fci)==1 && abs(tcj-fcj)==1)             //no diagonal cubes
			&& !(abs(tci-fci)==0 && abs(tcj-fcj)==0)         //not the same cube
			&&(abs(tci-fci)<=1 && abs(tcj-fcj)<=1) ){        //distance 1 

			sel=2;
			mouse=0;
			secondCube=theCube;
		}
	}
	if(sel==2){
		switchCubes(firstCube,secondCube);
		sel=0;                           //made a move, reset sel counter and -1 moves
		movesLeft--;
	}
}
//-----------------------MOVE CUBES--------------------------------------------------------------------

//----------------------------BOARD ELEMENTS------------------------------------------------
//function for initializing cubes positions and i,j
void initBoard(){
	for(int i=0; i<15; i++){
		for(int j=0; j<15; j++){
			cubes[i][j].xpos=(0.0+i)/8-0.87;
			cubes[i][j].i=i;
			cubes[i][j].ypos=(0.0+j)/8-0.87;
			cubes[i][j].j=j;
		}
	}
}



//function to prevent random type generator from putting 3 of the same type together
void checkT(int times){
	srand(time(0));
	for(int i=0; i<15; i++){
		for(int j=0; j<15; j++){
			int theType=cubes[i][j].type;
			if((cubes[i-2][j].type==theType && cubes[i-1][j].type==theType)
				||(cubes[i+2][j].type==theType && cubes[i+1][j].type==theType)
				||(cubes[i][j-2].type==theType && cubes[i][j-1].type==theType)
				||(cubes[i][j+2].type==theType && cubes[i][j+1].type==theType)){
				num=rand()%5;
				while(theType==num){
					num=rand()%5;
				}
				cubes[i][j].type=num;
				//times variable is used so that recusion doesn't go too deep
				times++;
				if(times<=3){
					checkT(times);
				}				
			}
		}
	}
}
	
//function to initialize types randomly
void initTypes(){
	srand(time(0));
	for(int i=0; i<15; i++){
		for(int j=0; j<15; j++){
			num=rand()%5;
			cubes[i][j].type=num;
		}
	}
	checkT(0);
}
//----------------------------BOARD ELEMENTS------------------------------------------------

//------------------------------PRINT ON THE SCREEN-----------------------------------
void printok(float x, float y, float z, char* text, int width, float scale){
	char *c;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 600, 0, 600);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(x,y,z);
	glScalef(scale,scale,z);
	for (c=text; *c != '\0'; c++) {
		glLineWidth(width);
		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

//function to count the score digits and put into an array each one for the printing
int calScore(int n){
	int count=0;
	int nfor=n;
	while (n != 0){ 
		n=n/10;
        count++; 
    }
    if(count==0){
    	count=1;
    }
    for(int i=0; i<count;i++){
		r[i]=nfor%10;
		nfor=nfor/10;
	}
    return count; 
}

int calMoves(int n){
	int count=0;
	int nfor=n;
	while (n != 0){ 
		n=n/10;
        count++; 
    }
    if(count==0){
    	count=1;
    }
    for(int i=0; i<count;i++){
		moves[i]=nfor%10;
		nfor=nfor/10;
	}
    return count; 
}

void printScore(float x, float y, float z, int width, float scale){
	char *c;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 600, 0, 600);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(x,y,z);
	glScalef(scale,scale,z);	
	int size;
	size=calScore(score);
	for(int i=size-1;i>=0;i--){
		char s='0'+r[i];
		glutStrokeCharacter(GLUT_STROKE_ROMAN , s);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void printMovesLeft(float x, float y, float z, int width, float scale){
	char *c;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 600, 0, 600);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(x,y,z);
	glScalef(scale,scale,z);	
	int size;
	size=calMoves(movesLeft);
	for(int i=size-1;i>=0;i--){
		char s='0'+moves[i];
		glutStrokeCharacter(GLUT_STROKE_ROMAN , s);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
//------------------------------PRINT ON THE SCREEN-----------------------------------

//--------------------------------SCORE CALCULATION AND EXPLOSION-----------------------
//function to eat neighbor cubes when moving
void eatCubeNeighbor(cube theCube,int tci,int tcj,int theType){
	if(eat==1){
		//rock type eat scissors
		if(theType==2){
			if(cubes[tci-1][tcj].type==4){
				cubes[tci-1][tcj].type=5;
				score+=2;
			}
			if(cubes[tci+1][tcj].type==4){
				cubes[tci+1][tcj].type=5;
				score+=2;
			}
			if(cubes[tci][tcj-1].type==4){
				cubes[tci][tcj-1].type=5;
				score+=2;
			}
			if(cubes[tci][tcj+1].type==4){
				cubes[tci][tcj+1].type=5;
				score+=2;
			}
		}
		//paper eats rock
		if(theType==3){
			if(cubes[tci-1][tcj].type==2){
				cubes[tci-1][tcj].type=5;
				score+=2;
			}
			if(cubes[tci+1][tcj].type==2){
				cubes[tci+1][tcj].type=5;
				score+=2;
			}
			if(cubes[tci][tcj-1].type==2){
				cubes[tci][tcj-1].type=5;
				score+=2;
			}
			if(cubes[tci][tcj+1].type==2){
				cubes[tci][tcj+1].type=5;
				score+=2;
			}
		}
		//scissors eat paper
		if(theType==4){
			if(cubes[tci-1][tcj].type==3 && tci-1>=0 && tci-1<=14 && tcj>=0 && tcj<=14){
				cubes[tci-1][tcj].type=5;
				score+=2;
			}
			if(cubes[tci+1][tcj].type==3 && tci+1>=0 && tci+1<=14 && tcj>=0 && tcj<=14){
				cubes[tci+1][tcj].type=5;
				score+=2;
			}
			if(cubes[tci][tcj-1].type==3 && tci>=0 && tci<=14 && tcj-1>=0 && tcj-1<=14){
				cubes[tci][tcj-1].type=5;
				score+=2;
			}
			if(cubes[tci][tcj+1].type==3 && tci>=0 && tci<=14 && tcj+1>=0 && tcj+1<=14){
				cubes[tci][tcj+1].type=5;
				score+=2;
			}
		}
		//reset eat value
		eat=0;
	}
}

//function to destory horizontally when 3 of the same type appear
void destoryHorizontal(cube theCube,int tci,int tcj,int theType){
	if(cubes[tci+1][tcj].type==theType && cubes[tci+2][tcj].type==theType && theType!=5){
		score+=10;

		//distance 1
		for(int i=(tci-1);i<=(tci+3);i++){
			for(int j=(tcj-1);j<=(tcj+1);j++){
				//blue and red only destroy itself
				if(theType==0 || theType==1){
					cubes[tci][tcj].type=5;
					cubes[tci+1][tcj].type=5;
					cubes[tci+2][tcj].type=5;
					break;
				}
				//rock destroys everything but paper
				if(theType==2){
					if(cubes[i][j].type!=3 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=2;
						cubes[i][j].type=5;
					}
					else{
						score-=1;
					}
				}
				//paper destroys everything but scissors
				if(theType==3){
					if(cubes[i][j].type!=4 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=2;
						cubes[i][j].type=5;
					}
					else{
						score-=1;
					}
				}
				//scissors destroy everything but rock
				if(theType==4){
					if(cubes[i][j].type!=2 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=2;
						cubes[i][j].type=5;
					}
					else{
						score-=1;
					}
				}
			}
		}
		//distance 2-3
		for(int i=(tci-3);i<=(tci+5);i++){
			for(int j=(tcj-3);j<=(tcj+3);j++){
				//rock only destroys scissors
				if(theType==2){
					if(cubes[i][j].type==4 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=3;
						cubes[i][j].type=5;
					}
				}
				//paper only destroys rock
				if(theType==3){
					if(cubes[i][j].type==2 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=3;
						cubes[i][j].type=5;
					}
				}
				//scissors only destroy paper
				if(theType==4){
					if(cubes[i][j].type==3 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=3;
						cubes[i][j].type=5;
					}
				}
			}
		}
	}
}

//function to destory vertically when 3 of the same type appear
void destoryVertical(cube theCube,int tci,int tcj,int theType){
	if(cubes[tci][tcj+1].type==theType && cubes[tci][tcj+2].type==theType && theType!=5){
		score+=10;

		//distance 1
		for(int i=(tci-1);i<=(tci+1);i++){
			for(int j=(tcj-1);j<=(tcj+3);j++){
				//blue and red only destroy itself
				if(theType==0 || theType==1){
					cubes[tci][tcj].type=5;
					cubes[tci][tcj+1].type=5;
					cubes[tci][tcj+2].type=5;
					break;
				}
				//rock destroys everything but paper
				if(theType==2){
					if(cubes[i][j].type!=3 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=2;
						cubes[i][j].type=5;
					}
					else{
						score-=1;
					}
				}
				//paper destroys everything but scissors
				if(theType==3){
					if(cubes[i][j].type!=4 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=2;
						cubes[i][j].type=5;
					}
					else{
						score-=1;
					}
				}
				//scissors destroy everything but rock
				if(theType==4){
					if(cubes[i][j].type!=2 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=2;
						cubes[i][j].type=5;
					}
					else{
						score-=1;
					}
				}
			}
		}
		//distance 2-3
		for(int i=(tci-3);i<=(tci+3);i++){
			for(int j=(tcj-3);j<=(tcj+5);j++){
				//rock only destroys scissors
				if(theType==2){
					if(cubes[i][j].type==4 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=3;
						cubes[i][j].type=5;
					}
				}
				//paper only destroys rock
				if(theType==3){
					if(cubes[i][j].type==2 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=3;
						cubes[i][j].type=5;
					}
				}
				//scissors only destroy paper
				if(theType==4){
					if(cubes[i][j].type==3 && i>=0 && i<=14 && j>=0 && j<=14){
						score+=3;
						cubes[i][j].type=5;
					}
				}
			}
		}
	}
}

//boom
void boom(cube theCube){
	//cube info
	int tci=theCube.i;
	int tcj=theCube.j;
	int theType=cubes[tci][tcj].type;

	eatCubeNeighbor(theCube,tci,tcj,theType);

	destoryHorizontal(theCube,tci,tcj,theType);
	
	destoryVertical(theCube,tci,tcj,theType);
	
}
//--------------------------------SCORE CALCULATION AND EXPLOSION-----------------------

//----------------------------------MAIN DRAWING FUNCTION-------------------------------
void setUpBoard(){
	// Push and pop the current matrix stack. 
	// This causes that translations and rotations on this matrix wont influence others.
	// 15x15 initial cube grid
	if(start==0){
		glColor3f(0.871, 0.722, 0.529);
		for(int i=0; i<15; i++){
			for(int j=0; j<15; j++){
				glPushMatrix();
				glTranslatef(cubes[i][j].xpos,cubes[i][j].ypos,0.0);
				drawCube(0.05);
				glPopMatrix();
			}
		}
	}

	//player selected 'start game' 
	//set up the board with the different kinds of cubes
	else if(start==1 && movesLeft>0){
		//printing score and remaining moves on the window
		glDisable(GL_LIGHTING);

		char mes[]="SCORE: ";
		glColor3f(2.55,2.55,2.55);
		printok(500,580,0.0,mes,2,0.1);
		printScore(560,580,0.0,2,0.1);

		char mes1[]="MOVES LEFT: ";
		glColor3f(2.55,2.55,2.55);
		printok(50,580,0.0,mes1,2,0.1);
		printMovesLeft(150,580,0.0,2,0.1);

		glEnable(GL_LIGHTING);

		//read cubes array and set the board this time with textures and colors
		for(int i=0; i<15; i++){
			for(int j=0; j<15; j++){
				//RED
				if(cubes[i][j].type==0){
					glDisable(GL_TEXTURE_2D);
					glColor3f(2.55, 0.0, 0.0);
				}
				//BLUE
				else if(cubes[i][j].type==1){
					glDisable(GL_TEXTURE_2D);
					glColor3f(0.0, 0.0, 2.55);
				}
				//ROCK
				else if(cubes[i][j].type==2){
					glEnable(GL_TEXTURE_2D);
					glColor3f(1.10, 1.14, 1.12);
					glBindTexture(GL_TEXTURE_2D, textures[0]);
				}
				//PAPER
				else if(cubes[i][j].type==3){
					glEnable(GL_TEXTURE_2D);
					glColor3f(2.55, 2.55, 2.55);
					glBindTexture(GL_TEXTURE_2D, textures[1]);
				}
				//SCISSORS
				else if(cubes[i][j].type==4){
					glEnable(GL_TEXTURE_2D);
					glColor3f(2.55, 2.55, 0);
					glBindTexture(GL_TEXTURE_2D, textures[2]);
				}
				else if(cubes[i][j].type==5){
					//turn cube to black if it explodes
					glColor3f(0.0,0.0,0.0);
				}
				glPushMatrix();
				glTranslatef(cubes[i][j].xpos,cubes[i][j].ypos,0.0);
				select(cubes[i][j],0.05);                      //selecting two cubes to swap
				boom(cubes[i][j]);                             //destroy 
				drawCube(0.05);                                //and draw
				glDisable(GL_TEXTURE_2D);
				glPopMatrix();
			}
		}	
	}
	//game ended print score
	else if(movesLeft==0){
		glDisable(GL_LIGHTING);
		glColor3f(1.0,0.0,0.0);
		char mess[]="GAME OVER!";
		char mess1[]="FINAL SCORE :";
		printok(100,300,0.0,mess,50,0.5);
		printok(100,200,0.0,mess1,2,0.3);
		printScore(420,200,0.0,2,0.3);
	}
}
//----------------------------------MAIN DRAWING FUNCTION-------------------------------

//--------------------------------------MAIN DISPLAY AND INIT--------------------------
//display and idle func
void display(){
	// Clear Screen and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		     
	glLoadIdentity();

//--------------3D ROTATION----------------------------
	//look at point = line of sight + cam position
	gluLookAt( x, 0.0f, z,x+lx, 0.0f, z+lz,0.0f, 1.0f, 0.0f);
//--------------3D ROTATION----------------------------

	//draw board
	setUpBoard(); 

	glutSwapBuffers();
}


void initialize (){

	initBoard();

   // select projection matrix
    glMatrixMode(GL_PROJECTION);												
 
	// set the viewport
    glViewport(0, 0, win.width, win.height);									
 
	// set matrix mode
    glMatrixMode(GL_PROJECTION);												
 
	// reset projection matrix
    glLoadIdentity();
    GLfloat aspect = (GLfloat) win.width / win.height;
 
	// set up a perspective projection matrix
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);		
 
	// specify which matrix is the current matrix
	glMatrixMode(GL_MODELVIEW);													
    glShadeModel( GL_SMOOTH );
 
	// specify the clear value for the depth buffer
	glClearDepth( 1.0f );														
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
 
	// specify implementation-specific hints
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );					

 	//set up lighting
	GLfloat amb_light[] = { 0.1, 0.1, 0.1, 1};
    GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
    GLfloat specular[] = { 0.1, 0.7, 0.1, 1 };
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable( GL_LIGHT0 );
    glEnable( GL_COLOR_MATERIAL );
    glShadeModel( GL_SMOOTH );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); 
    glClearColor(0.0, 0.0, 0.0, 1.0);

    //load textures
    textures = new GLuint[3]; 
    glGenTextures(3, textures);

    loadTexture(textures[0], "rock.png");
    loadTexture(textures[1], "paper.png");
    loadTexture(textures[2], "scissors.png");
}


//-------------------------------------------MENU--------------------------------------
void processMenuEvents(int option){
	switch (option) {
		case Start_game :
			start=1;
			initTypes();
			movesLeft=MOVES;
			score=0;
			break;
		case Exit :
			exit(1);
			break;
	}
}


void createGLUTMenus(){
	int menu;

	// create the menu and
	// tell glut that "processMenuEvents" will
	// handle the events
	menu = glutCreateMenu(processMenuEvents);

	//add entries to our menu
	glutAddMenuEntry("Start Game!", Start_game);
	glutAddMenuEntry("Exit :\(", Exit);

	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//---------------------------------------------MENU------------------------------------

int main(int argc, char **argv){
	// set window values
	win.width = 600;
	win.height = 600;
	win.title = (char *) "BraChaPsa III";
	win.field_of_view_angle = 55;
	win.z_near = 1.0f;
	win.z_far = 500.0f;

	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);					// set window size
	glutCreateWindow(win.title);								// create Window

	glutDisplayFunc(display);									// register Display Function
	glutIdleFunc(display);									    // register Idle Function
   	createGLUTMenus();                                          // create menus
	initialize();                                               // reshape-lighting-textures

	glutMouseFunc( mousebutton);                                // cube selecting
	glutSpecialFunc(processSpecialKeys);                        // 3d rotation

	glutMainLoop();												// run GLUT mainloops

	//Free our texture
	FreeTexture(textures[0]);
	FreeTexture(textures[1]);
	FreeTexture(textures[2]);
						
	return 0;
}