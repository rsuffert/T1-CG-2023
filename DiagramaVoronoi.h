//
//  DiagramaVoronoi.hpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 23/08/23.
//  Copyright © 2023 Márcio Sarroglia Pinho. All rights reserved.
//

#ifndef DiagramaVoronoi_h
#define DiagramaVoronoi_h

#include <iostream>
#include <fstream>
using namespace std;

#include "Poligono.h"
#include "Envelope.h"

class Voronoi
{
    Poligono Diagrama[1000];
    Envelope envelopes[1000];
    unsigned int qtdDePoligonos;
    Ponto Min, Max;
    int findNeighbor(int polygonIdx, Ponto edgePoint1, Ponto edgePoint2);
public:
    Voronoi();
    Poligono LeUmPoligono();
    void LePoligonos(const char *nome);
    Poligono& getPoligono(int i);
    void obtemLimites(Ponto &min, Ponto &max);
    unsigned int getNPoligonos();
	Envelope getEnvelope(int i);
};

#endif /* DiagramaVoronoi_h */
