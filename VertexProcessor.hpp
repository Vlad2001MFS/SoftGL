#pragma once
#include "Math.hpp"
#include <vector>

struct Vertex {
    Vec4f pos;
    Color color;
};

void vpAddVertex(Vertex &&v);
void vpProcess();

const std::vector<Vertex> &vpGetVertices();