//
//  DiagramaVoronoi.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 23/08/23.
//  Copyright © 2023 Márcio Sarroglia Pinho. All rights reserved.
//

#include "DiagramaVoronoi.h"

#include <stdexcept>

ifstream input;            // ofstream arq;

Voronoi::Voronoi()
{

}
Poligono Voronoi::LeUmPoligono()
{
    Poligono P;
    unsigned int qtdVertices;
    input >> qtdVertices;  // arq << qtdVertices
    for (int i=0; i< qtdVertices; i++)
    {
        double x,y;
        // Le um ponto
        input >> x >> y;
        Ponto(x, y).imprime();
        if(!input)
        {
            cout << "Fim inesperado da linha." << endl;
            break;
        }
        P.insereVertice(Ponto(x,y));
    }
    cout << "Poligono lido com sucesso!" << endl;
    return P;
}

void Voronoi::LePoligonos(const char *nome)
{
    input.open(nome, ios::in); //arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    string S;

    input >> qtdDePoligonos;
    cout << "qtdDePoligonos:" << qtdDePoligonos << endl;
    Ponto A, B;
    Diagrama[0] = LeUmPoligono();
    Diagrama[0].obtemLimites(Min, Max);    // obtem o envelope do poligono

	Envelope e;
    e.GeraEnvelope(Min, Max);
    envelopes[0] = e; // store polygon's envelope

    for (int i=1; i< qtdDePoligonos; i++)
    {
        Diagrama[i] = LeUmPoligono();
        Diagrama[i].obtemLimites(A, B);    // obtem o envelope do poligono

        Envelope e;
        e.GeraEnvelope(A, B);
        envelopes[i] = e; // store polygon's envelope

        Min = ObtemMinimo(A, Min);
        Max = ObtemMaximo(B, Max);
    }

    // load neighbors
    cout << "LOADING THE NEIGHBORS OF THE EDGES" << endl;
    for (int p=0; p<qtdDePoligonos; p++) // for each polygon
    {
        Poligono& pol = this->getPoligono(p);
        cout << "Poligono: " << p << endl;
        for (int e=0; e<pol.getNVertices(); e++) // for each edge
        {
            Ponto ep1, ep2;
            pol.getAresta(e, ep1, ep2);
            // find the edge's neighbor index and initialize it
            int neighborIdx = this->findNeighbor(p, ep1, ep2);
            //if (neighborIdx < 0) throw runtime_error("Could not find neighbor of some edge of polygon " + p);
            pol.addNeighborPolygon(neighborIdx);
            cout << "Neighbor of edge " << e << " : " << pol.getNeighborPolygonIdx(e) << endl;
        }
    }

    cout << "Lista de Poligonos lida com sucesso! Criados " << qtdDePoligonos << "poligonos." << endl;
}

int Voronoi::findNeighbor(int polygonIdx, Ponto edgePoint1, Ponto edgePoint2)
{
    //cout << "Finding neighbor of polygon " << polygonIdx << endl;
    //cout << "     (" << edgePoint1.x << "," << edgePoint1.y << ") - (" << edgePoint2.x << "," << edgePoint2.y << ")" << endl;
    for (int p=0; p<qtdDePoligonos; p++) // for each polygon
    {
        if (p != polygonIdx) // investigate the polygons that are not the current to find the other that shares the same edge
        {
            //cout << "Investigating edges of polygon " << p << endl;
            Poligono pol = Diagrama[p];
            for (int e=0; e<pol.getNVertices(); e++) // for each edge
            {
                Ponto ep1, ep2;
                pol.getAresta(e, ep1, ep2);
                //cout << "     (" << ep1.x << "," << ep1.y << ") - (" << ep2.x << "," << ep2.y << ")" << endl;
                if ( (edgePoint1 == ep1 && edgePoint2 == ep2) || (edgePoint2 == ep1 && edgePoint1 == ep2) )
                    return p;
            }
        }
    }
    return -1;
}

Poligono& Voronoi::getPoligono(int i)
{
    if (i >= qtdDePoligonos)
    {
        cout << "Nro de Poligono Inexistente" << endl;
        return Diagrama[0];
    }
    return Diagrama[i];
}
unsigned int Voronoi::getNPoligonos()
{
    return qtdDePoligonos;
}
void Voronoi::obtemLimites(Ponto &min, Ponto &max)
{
    min = this->Min;
    max = this->Max;
}

Envelope Voronoi::getEnvelope(int i)
{
    if (i >= qtdDePoligonos || i < 0) {
        cout << "Nro de envelope Inexistente" << endl;
        return envelopes[0];
    }
    return envelopes[i];
}
