//
//  Poligono.hpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 18/08/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#ifndef Poligono_hpp
#define Poligono_hpp

#include <iostream>
using namespace std;


#ifdef WIN32
#include <windows.h>
#include <glut.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Ponto.h"
#include <vector>

class Poligono
{
    vector <Ponto> Vertices;
    vector <int>   neighbors;
    Ponto Min, Max;
public:
    Poligono();
    Ponto getVertice(int);
    unsigned long getNVertices();
    void insereVertice(Ponto);
    void insereVertice(Ponto p, int pos);
    void desenhaPoligono();
    void desenhaVertices();
    void pintaPoligono();
    void imprime();
    void atualizaLimites();
    void obtemLimites(Ponto &Min, Ponto &Max);
    void LePoligono(const char *nome);
    void desenhaAresta(int n);
    void getAresta(int i, Ponto &P1, Ponto &P2);
    bool ponto_A_Esq_Aresta(Ponto ponto, Ponto arestaP1, Ponto arestaP2);
    bool ponto_Dentro_Poligno(Ponto ponto, int* cont, int& crossedEdgeIdx);
    void addNeighborPolygon(int neighborPolygonIdx);
    int getNeighborPolygonIdx(int pos);
    int getNNeighbors();
};

#endif
