//
//  Poligono.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 18/08/20.
//  Improvements by Ricardo Süffert and Vinícius Turani (added documented functions implementing the algorithms for point inclusion in concave/convex polygons)
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;

#include "Poligono.h"
#include "Ponto.h"

Poligono::Poligono()
{

}

void Poligono::insereVertice(Ponto p)
{
    Vertices.push_back(p);
}

void Poligono::insereVertice(Ponto p, int pos)
{
    if ((pos < 0) || (pos>Vertices.size()))
    {
        cout << "Metodo " << __FUNCTION__ << ". Posicao Invalida. Vertice nao inserido." << endl;
        return;
    }
    Vertices.insert(Vertices.begin()+pos, p);
}

Ponto Poligono::getVertice(int i)
{
    return Vertices[i];
}

void Poligono::pintaPoligono()
{
    glBegin(GL_POLYGON);
    for (int i=0; i<Vertices.size(); i++)
        glVertex3f(Vertices[i].x,Vertices[i].y,Vertices[i].z);
    glEnd();
}

void Poligono::desenhaPoligono()
{
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<Vertices.size(); i++)
        glVertex3f(Vertices[i].x,Vertices[i].y,Vertices[i].z);
    glEnd();
}
void Poligono::desenhaVertices()
{
    glBegin(GL_POINTS);
    for (int i=0; i<Vertices.size(); i++)
        glVertex3f(Vertices[i].x,Vertices[i].y,Vertices[i].z);
    glEnd();
}
void Poligono::imprime()
{
    for (int i=0; i<Vertices.size(); i++)
        Vertices[i].imprime();
}
unsigned long Poligono::getNVertices()
{
    return Vertices.size();
}

void Poligono::obtemLimites(Ponto &Min, Ponto &Max)
{
    Max = Min = Vertices[0];

    for (int i=0; i<Vertices.size(); i++)
    {
        Min = ObtemMinimo (Vertices[i], Min);
        Max = ObtemMaximo (Vertices[i], Max);
    }
}

// **********************************************************************
//
// **********************************************************************
void Poligono::LePoligono(const char *nome)
{
    ifstream input;            // ofstream arq;
    input.open(nome, ios::in); //arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << nome << "...";
    string S;
    //int nLinha = 0;
    unsigned int qtdVertices;

    input >> qtdVertices;  // arq << qtdVertices

    for (int i=0; i< qtdVertices; i++)
    {
        double x,y;
        // Le cada elemento da linha
        input >> x >> y; // arq << x  << " " << y << endl
        if(!input)
            break;
        //nLinha++;
        insereVertice(Ponto(x,y));
    }
    cout << "Poligono lido com sucesso!" << endl;
    input.close();
}

void Poligono::getAresta(int n, Ponto &P1, Ponto &P2)
{
    P1 = Vertices[n];
    int n1 = (n+1) % Vertices.size();
    P2 = Vertices[n1];
}

void Poligono::desenhaAresta(int n)
{
    glBegin(GL_LINES);
        glVertex3f(Vertices[n].x,Vertices[n].y,Vertices[n].z);
        int n1 = (n+1) % Vertices.size();
        glVertex3f(Vertices[n1].x,Vertices[n1].y,Vertices[n1].z);
    glEnd();
}

/**
* Tells if a given point is on the left of a given edge of this polygon.
* @param ponto the point whose laterality is to be tested.
* @param A1 the first point that makes up the edge to be investigated.
* @param A2 the second point that makes up the edge to be investigated.
* @param return true if the given point is on the left of the given edge; false if not.
*/
bool Poligono::pontoAEsquerdaAresta(Ponto ponto, Ponto A1, Ponto A2)
{
    Ponto vetorAresta = A2 - A1;
    Ponto vetorPonto = ponto - A1;
    Ponto p;
    ProdVetorial(vetorAresta, vetorPonto, p);
    return p.z < 0; //retorna verdadeiro se o ponto estiver a esquerda da aresta
}

/**
* Tells whether or not a given point is inside of this polygon using the vectorial product (suited for convex polygons only).
* @param ponto the point to be tested whether or not it is in this polygon.
* @param cont the variable to store how many times we have invoked the function that calculates the vectorial product.
* @param crossedEdgeIdx the variable to store the index of the edge of this polygon that the moving polygon is on the right side, if it is not in the polygon.
* @return true if the moving point is in this polygon; false if not.
*/
bool Poligono::pontoDentroPoligno(Ponto ponto, int& cont, int& crossedEdgeIdx)
{
    int numVertices = Vertices.size();
    int contadorChamadasProdVet = 0;
    for (int i = 0; i < numVertices; i++)
    {
        Ponto A1 = Vertices[i];
        Ponto A2 = Vertices[(i + 1) % numVertices]; // Próximo vértice (faz loop no último vértice)
        contadorChamadasProdVet++;
        if (!pontoAEsquerdaAresta(ponto, A1, A2))
        {
            crossedEdgeIdx = i;
            cont = contadorChamadasProdVet;
            return false; //se tiver algum ponto do lado contrario, entao nao esta nele
        }
    }
    cont = contadorChamadasProdVet;
    return true;
}

/**
* Adds a neighbor to the list of neighbors of this polygon.
* @param neighborPolygonIdx the index of the neighbor polygon to be added.
*/
void Poligono::addNeighborPolygon(int neighborPolygonIdx)
{
    this->neighbors.push_back(neighborPolygonIdx);
}

/**
* Returns the requested neighbor of this polygon.
* @param edgeIdx the index of the edge whose neighbor is to be retrieved.
* @return the index of the neighbor of the given edge.
*/
int Poligono::getNeighborPolygonIdx(int edgeIdx)
{
    return this->neighbors.at(edgeIdx);
}

/**
* Tells how many neighbors we have stored for this polygon.
* @return how many neighbors we have stored for this polygon.
*/
int Poligono::getNNeighbors()
{
    return this->neighbors.size();
}
