// **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
//  Improvements by Ricardo Süffert and Vinícius Turani (added documented functions implementing the algorithms for point inclusion in concave/convex polygons)
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <stdlib.h>
#include <string>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"
#include "DiagramaVoronoi.h"
#include "Envelope.h"

#include "ListaDeCoresRGB.h"

#include "Temporizador.h"

string colorNames[] = {
    "Aquamarine",
    "BlueViolet",
    "IndianRed",
    "CadetBlue",
    "Coral",
    "CornflowerBlue",
    "DarkGreen",
    "DarkOliveGreen",
    "DarkOrchid",
    "DarkSlateBlue",
    "DarkSlateGray",
    "DarkSlateGrey",
    "DarkTurquoise",
    "Firebrick",
    "ForestGreen",
    "Gold",
    "Goldenrod",
    "GreenYellow",
    "Brown",
    "Khaki",
    "LightBlue",
    "LightSteelBlue",
    "LimeGreen",
    "Maroon",
    "MediumAquamarine",
    "MediumBlue",
    "MediumForestGreen",
    "MediumGoldenrod",
    "MediumOrchid",
    "MediumSeaGreen",
    "MediumSlateBlue",
    "MediumSpringGreen",
    "MediumTurquoise",
    "MediumVioletRed",
    "MidnightBlue",
    "Navy",
    "NavyBlue",
    "Orange",
    "OrangeRed",
    "Orchid",
    "PaleGreen",
    "Pink",
    "Plum",
    "Salmon",
    "SeaGreen",
    "Sienna",
    "SkyBlue",
    "SlateBlue",
    "SpringGreen",
    "SteelBlue",
    "Tan",
    "Thistle",
    "Turquoise",
    "Violet",
    "VioletRed",
    "Wheat",
    "YellowGreen",
    "SummerSky",
    "RichBlue",
    "Brass",
    "Copper",
    "Bronze",
    "Bronze2",
    "Silver",
    "BrightGold",
    "OldGold",
    "Feldspar",
    "Quartz",
    "NeonPink",
    "DarkPurple",
    "NeonBlue",
    "CoolCopper",
    "MandarinOrange",
    "LightWood",
    "MediumWood",
    "DarkWood",
    "SpicyPink",
    "SemiSweetChoc",
    "BakersChoc",
    "Flesh",
    "NewTan",
    "NewMidnightBlue",
    "VeryDarkBrown",
    "DarkBrown",
    "DarkTan",
    "GreenCopper",
    "DkGreenCopper",
    "DustyRose",
    "HuntersGreen",
    "Scarlet",
    "Med_Purple",
    "Light_Purple",
    "Very_Light_Purple"
};
const int colorsSize = 92;

Temporizador T;
double AccumDeltaT=0;

Poligono Pontos;

Voronoi Voro;
int *CoresDosPoligonos;

// Limites logicos da area de desenho
Ponto Min, Max, PontoClicado;

bool desenha = false;
bool FoiClicado = false;

float angulo=0.0;

Ponto movingPoint;     // point that the user will move around in the screen
int currentPolygonIdx; // index of the polygon in which the movingPoint is currently located
Ponto voroMin, voroMax;

#define ENABLE_DEBUG_FEATURES 1

// **********************************************************************
//
// **********************************************************************
void printString(string s, int posX, int posY)
{
    //defineCor(cor);
    glColor3f(1,1,1);
    glRasterPos3i(posX, posY, 0); //define posicao na tela
    for (int i = 0; i < s.length(); i++)
    {
//GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_TIMES_ROMAN_24,GLUT_BITMAP_HELVETICA_18
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    }
}
// **********************************************************************
//
// **********************************************************************
void ImprimeNumeracaoDosVertices(Poligono &P)
{
    for(int i=0;i<P.getNVertices();i++)
    {
        Ponto aux;
        aux = P.getVertice(i);
        char msg[10];
        sprintf(msg,"%d",i);
        printString(msg,aux.x, aux.y);
    }
}

void ImprimeNroDoPoligono(Poligono P,int n)

{
    char msg[10];
    sprintf(msg,"%d",n);
    Ponto Soma, A;
    for (int i=0;i<P.getNVertices();i++)
    {
        A = P.getVertice(i);
        Soma = Soma + A;
    }
    double div = 1.0/P.getNVertices();
    Soma = Soma * div;
    printString(msg,Soma.x, Soma.y);
}

// **********************************************************************
//
// **********************************************************************
void GeraPontos(int qtd)
{
    time_t t;
    Ponto Escala;
    Escala = (Max - Min) * (1.0/1000.0);
    srand((unsigned) time(&t));
    for (int i = 0;i<qtd; i++)
    {
        float x = rand() % 1000;
        float y = rand() % 1000;
        x = x * Escala.x + Min.x;
        y = y * Escala.y + Min.y;
        Pontos.insereVertice(Ponto(x,y));
    }
}

/**
* Applies the convex inclusion algorithm to find the current polygon where the moving point is.
* @param callsToProdVetorial the number of calls to the ProdVetorial method.
* @return the index of the polygon where the moving point is currently in.
*/
int findCurrentPolygonConvexAlgorithm(int& callsToProdVetorial)
{
    int NcallsToProdVet = 0; // calls to calculation method
    int i = 0; // polygon index

    for (i = 0; i < Voro.getNPoligonos(); i++) // for each polygon
    {
        Envelope enve = Voro.getEnvelope(i);
        if (enve.pontoEstaDentro(movingPoint)) // if the point is in the envelope
        {
            Poligono poligono = Voro.getPoligono(i);

            int crossedEdgeIdx;
            int auxCont;
            bool pointInPolygon = poligono.pontoDentroPoligno(movingPoint, auxCont, crossedEdgeIdx);
            NcallsToProdVet+=auxCont;
            if (pointInPolygon) // if the point is in the polygon
            {
                // we have found the polygon
                callsToProdVetorial = NcallsToProdVet;
                //cout << "Poligno atual (CONVEX ALGORITHM DEBUG) (i): " << i << "\n" << endl;
                return i;
            }
        }
    }
    callsToProdVetorial = NcallsToProdVet;
    return -1;
}

float movementStepX, movementStepY;
// **********************************************************************
//
// **********************************************************************
void init()
{
    srand(0);
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    Voro.LePoligonos("ListaDePoligonos-V2.txt");
    Voro.obtemLimites(Min,Max);
    Min.imprime("Minimo:", "\n");
    Max.imprime("Maximo:", "\n");

    // initialize the moving point
    glColor3b(255, 0, 0);
    movingPoint.x = (Min.x+Max.x)/2 + 2;
    movingPoint.y = (Min.y+Max.y)/2 + 2;
    movingPoint.z = (Min.z+Max.z)/2;

    CoresDosPoligonos = new int[Voro.getNPoligonos()];

    for (int i=0; i<Voro.getNPoligonos(); i++)
        CoresDosPoligonos[i] = (i*2)%colorsSize;//rand()%80;

    // Ajusta a largura da janela l�gica
    // em fun��o do tamanho dos pol�gonos
    Ponto Largura;
    Largura = Max - Min;

    Min = Min - Largura * 0.1;
    Max = Max + Largura * 0.1;

    // initialize current polygon where the moving point is at
    int counter;
    currentPolygonIdx = findCurrentPolygonConvexAlgorithm(counter);

    // each step of the point will measure 0.5% of the lenght of the diagram
    movementStepX = (Max.x-Min.x) * 0.005;
    movementStepY = (Max.y-Min.y) * 0.005;

    Voro.obtemLimites(voroMin, voroMax);
}

double nFrames=0;
double TempoTotal=0;
// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0/30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        //angulo+=0.05;
        glutPostRedisplay();
    }
    if (TempoTotal > 50.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela (Viewport/Janela de exibi��o)
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela (Window/Janela de sele��o/Universo)
    glOrtho(Min.x,Max.x,
            Min.y,Max.y,
            0,1);

    Voro.obtemLimites(voroMin, voroMax);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
//
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;

    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical
        glVertex2f(Meio.x,Min.y);
        glVertex2f(Meio.x,Max.y);
    glEnd();
}
// **********************************************************************
//
// **********************************************************************
void DesenhaLinha(Ponto P1, Ponto P2)
{
    glBegin(GL_LINES);
        glVertex3f(P1.x,P1.y,P1.z);
        glVertex3f(P2.x,P2.y,P2.z);
    glEnd();
}
// **********************************************************************
void InterseptaArestas(Poligono P)
{
    /*
    Ponto P1, P2;
    for (int i=0; i < P.getNVertices();i++)
    {
        P.getAresta(i, P1, P2);
        //if(PassaPelaFaixa(i,F))
        if (HaInterseccao(PontoClicado,Esq, P1, P2))
            P.desenhaAresta(i);
    }*/

}

void drawPoint(Ponto p, int size)
{
    glPointSize(size);
    glBegin(GL_POINTS);
        glVertex3f(p.x, p.y, p.z);
    glEnd();
}

/**
* Applies the concave polygon inclusion algorithm to find out if a point is inside a polygon.
* @param counter a pointer to the variable where the information on how many times the calculation method has been called should be stored.
* @param currentPol a pointer to the variable where the index of the polygon calculated by this method should be stored
* @return the color of the polygon where the point is.
*/
string concavePolygonInclusion(int& counter, int& currentPolIdx)
{
    int currentPol = -1;          // index of the polygon
    int callsToHaInterseccao = 0; // calls to calculation function

    for (int i=0; i<Voro.getNPoligonos(); i++) // for each polygon
    {
        if (Voro.getEnvelope(i).pontoEstaDentro(movingPoint)) { // if the moving point is in its envelope
            int intersectionCont = 0;
            Poligono poli = Voro.getPoligono(i);
            for(int j=0; j<poli.getNVertices(); j++) // iterate over the edges of the polygon, counting how many edges are intercepted by the horizontal line
            {
                Ponto P1, P2;
                poli.getAresta(j, P1, P2);
                callsToHaInterseccao++;
                if (HaInterseccao(Ponto(voroMin.x, movingPoint.y), movingPoint, P1, P2)) intersectionCont++;
            }
            if (intersectionCont % 2 != 0) // if number of intersections is odd, the point is in the polygon
            {
                currentPol = i;
                break;
            }
        }
    }
	counter = callsToHaInterseccao;
	currentPolIdx = currentPol;
    if (currentPol < 0) return "Out of bounds";
    else                return colorNames[(currentPol*2)%colorsSize]; // multiplied by two because that's the criteria for picking the polygon colors during initialization
}

/**
* Applies the convex polygon inclusion algorithm to find out if a point is inside a polygon.
* @param counter a pointer to the variable where the information on how many times the calculation method has been called should be stored.
* @param currentPol a pointer to the variable where the index of the polygon calculated by this method should be stored
* @return the color of the polygon where the point is.
*/
string convexPolygonInclusion(int& counter, int& currentPolIdx)
{
    int NcallsToProdVet;
    int polygonIdx = findCurrentPolygonConvexAlgorithm(NcallsToProdVet);
    counter = NcallsToProdVet;
    currentPolIdx = polygonIdx;
    if (polygonIdx < 0) return "Out of bounds";
    else                return colorNames[(polygonIdx*2)%colorsSize];
}

/**
* Applies the Voronoi neighbor convex polygon inclusion algorithm, which consists in storing during initialization,
* for each polygon edge, what polygon is on the other side of the edge. When an object crosses that line, it must be
* in the polygon on the other side of that edge.
* @param counter a pointer to the variable where the information on how many times the calculation method has been called should be stored.
* @param crossedEdgeIdx the index of the edge that's been crossed, calculated by the caller method when it found out the moving point had left the current polygon
* @param currentPol a pointer to the variable where the index of the polygon calculated by this method should be stored
* @return the color of the polygon where the point is.
*/
string convexVoronoiNeighborInclusion(int& counter, int crossedEdgeIdx, int& currentPolIdx)
{
    Poligono prevPol = Voro.getPoligono(currentPolygonIdx);
    currentPolygonIdx = prevPol.getNeighborPolygonIdx(crossedEdgeIdx);
    counter = 0;
    currentPolIdx = currentPolygonIdx;
    if (currentPolygonIdx < 0) return "Out of bounds";
    else                       return colorNames[(currentPolygonIdx*2)%colorsSize]; // multiplied by two because that's the criteria for picking the polygon colors during initialization
}

// **********************************************************************
//  void display( void )
// **********************************************************************
void display( void )
{
	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da area OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]
    DesenhaEixos();

    glRotatef(angulo, 0,0,1);
    glLineWidth(2);


    Poligono P;
    bool shouldPrintPolIdx = Voro.getNPoligonos() < 30;
    for (int i=0; i<Voro.getNPoligonos(); i++)
    {
        P = Voro.getPoligono(i);
        #if ENABLE_DEBUG_FEATURES
            Envelope e = Voro.getEnvelope(i);
            if (e.pontoEstaDentro(movingPoint)) glColor3f(1,1,1);
            else defineCor(CoresDosPoligonos[i]);
        #else
            defineCor(CoresDosPoligonos[i]);
        #endif
        P.pintaPoligono();
        if (shouldPrintPolIdx) ImprimeNroDoPoligono(P, i);
    }
    glColor3f(0,0,0);
    for (int i=0; i<Voro.getNPoligonos(); i++)
    {
        P = Voro.getPoligono(i);
        P.desenhaPoligono();
    }
    drawPoint(movingPoint, 7);

    #if ENABLE_DEBUG_FEATURES
        glColor3f(1,0,0);
        DesenhaLinha(Ponto(voroMin.x, movingPoint.y), movingPoint);
    #endif

    glutSwapBuffers();
}

// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n�mero de segundos e informa quanto frames
// se passaram neste per�odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}

/**
* Calculates and displays the current polygon where the moving point is in using three different algorithms.
*/
void calculateInclusion()
{
    cout << "\n-----------------------------------------------------------------------------------------------------------------------" << endl;

	// check if we have left the current polygon
	Poligono currentPolygon = Voro.getPoligono(currentPolygonIdx);
	int callsToProdVet=0, crossedEdgeIdx=0;
	bool stillInCurrent = currentPolygon.pontoDentroPoligno(movingPoint, callsToProdVet, crossedEdgeIdx);
	printf("Called ProdVetorial %d times to find out whether or not the point is still in the current polygon.\n", callsToProdVet);
	if (stillInCurrent)
	{
		cout << "We're still in the same polygon. No further actions will be taken." << endl;
	}
    else
    {
        cout << "We have left the polygon. Calling calculation algorithms..." << endl;
        int concaveAlgoCounter, convexAlgoCounter, voroNeighborAlgoCounter;
        int concaveAlgoPolIdx, convexAlgoPolIdx, voroNeighborAlgoPolIdx;
        string concaveAlgoResColor = concavePolygonInclusion(concaveAlgoCounter, concaveAlgoPolIdx);
        string convexAlgoResColor = convexPolygonInclusion(convexAlgoCounter, convexAlgoPolIdx);
        string voroAlgoResColor = convexVoronoiNeighborInclusion(voroNeighborAlgoCounter, crossedEdgeIdx, voroNeighborAlgoPolIdx);
        printf("\tCONCAVE INCLUSION:          %-15s (polygon %d) (%d calls to HaInterseccao).\n",
                        concaveAlgoResColor.c_str(), concaveAlgoPolIdx,       concaveAlgoCounter);
        printf("\tCONVEX INCLUSION:           %-15s (polygon %d) (%d calls to ProdVetorial).\n",
                        convexAlgoResColor.c_str(),  convexAlgoPolIdx,        convexAlgoCounter);
        printf("\tVORONOI NEIGHBOR INCLUSION: %-15s (polygon %d) (%d calls to ProdVetorial).\n",
                        voroAlgoResColor.c_str(),    voroNeighborAlgoPolIdx,  voroNeighborAlgoCounter);
    }

	cout << "-----------------------------------------------------------------------------------------------------------------------" << endl;
}

// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{
	switch ( key )
	{
        case 'w': if (movingPoint.y + movementStepY < voroMax.y) movingPoint.y+=movementStepY; break;
        case 'a': if (movingPoint.x - movementStepX > voroMin.x) movingPoint.x-=movementStepX; break;
        case 's': if (movingPoint.y - movementStepY > voroMin.y) movingPoint.y-=movementStepY; break;
        case 'd': if (movingPoint.x + movementStepX < voroMax.x) movingPoint.x+=movementStepX; break;
		default:                                                                               break;
	}
	calculateInclusion();
}

// **********************************************************************
// Esta fun��o captura o clique do botao direito do mouse sobre a �rea de
// desenho e converte a coordenada para o sistema de refer�ncia definido
// na glOrtho (ver fun��o reshape)
// Este c�digo � baseado em http://hamala.se/forums/viewtopic.php?t=20
// **********************************************************************
void Mouse(int button,int state,int x,int y)
{
    GLint viewport[4];
    GLdouble modelview[16],projection[16];
    GLfloat wx=x,wy,wz;
    GLdouble ox=0.0,oy=0.0,oz=0.0;

    if(state!=GLUT_DOWN)
      return;
    if(button!=GLUT_LEFT_BUTTON)
     return;
    cout << "Botao da Esquerda! ";

    glGetIntegerv(GL_VIEWPORT,viewport);
    y=viewport[3]-y;
    wy=y;
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
    gluUnProject(wx,wy,wz,modelview,projection,viewport,&ox,&oy,&oz);
    PontoClicado = Ponto(ox,oy,oz);
    PontoClicado.imprime("- Ponto no universo: ", "\n");
    FoiClicado = true;
}


// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  (650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de titulo da janela.
    glutCreateWindow    ( "Inclusão de Ponto em Polígonos Côncavos com OpenGL" );

    // executa algumas inicializa��es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    glutMouseFunc(Mouse);

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
