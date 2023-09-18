//
//  Poligono.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 18/08/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//
#include <iostream>
#include <fstream>
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

bool Poligono::ponto_A_Esq_Aresta(Ponto ponto, Ponto A1, Ponto A2)
{
    Ponto vetorAresta = A2 - A1;
    Ponto vetorPonto = ponto - A1;

    Ponto p;
    ProdVetorial(vetorAresta, vetorPonto, p);

    return (p.z < 0);//retorna verdadeiro se o ponto estiver a esquerda da aresta
}

bool Poligono::ponto_Dentro_Poligno(Ponto ponto, int* cont, Ponto& crossedEdgePoint1, Ponto& crossedEdgePoint2)
{
//    int contador =0;
//
//    // Verifica se o ponto está do mesmo lado de todas as outras arestas
//    for (int i = 0; i < Vertices.size()-1; i++)
//    {
//        bool ladoAtual;
//
//        if(i+2 <= Vertices.size())
//        {
//            Ponto A, B;
//            getAresta(i, A, B);
//            contador ++;
//            ladoAtual = ponto_A_Esq_Aresta(ponto, A,B);
//
//            if (ladoAtual== false)
//                return false;
//
//        }
//
//    }
//
//        *cont = contador;
//
//        // O ponto está do mesmo lado de todas as arestas.
//        return true;



    int numVertices = Vertices.size();
    int contador = 0; // Conta o número de arestas que o ponto está à esquerda
    for (int i = 0; i < numVertices; i++)
    {
        Ponto A1 = Vertices[i];
        Ponto A2 = Vertices[(i + 1) % numVertices]; // Próximo vértice (faz loop no último vértice)
        contador ++;
        if (!ponto_A_Esq_Aresta(ponto, A1, A2))
        {
            crossedEdgePoint1 = A1;
            crossedEdgePoint2 = A2;
            *cont = contador;
            return false;//se tiver algum ponto do lado contrario, entao nao esta nele
        }
    }
    *cont = contador;
    return true;

}

