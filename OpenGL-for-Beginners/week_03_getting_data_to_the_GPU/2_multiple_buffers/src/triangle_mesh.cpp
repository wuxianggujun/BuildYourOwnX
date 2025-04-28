#include "triangle_mesh.h"
#include <glad/glad.h>

TriangleMesh::TriangleMesh()
{
    // x y z r g b
    // X轴：从左(-1.0)到右(+1.0)
    //
    // Y轴：从下(-1.0)到上(+1.0)
    //
    // Z轴：从远(-1.0)到近(+1.0)

    std::vector<float> positions = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
    };

    std::vector<int> colorIndices = {
        0, 1, 2
    };

    vertex_count = 3;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    VBOS.resize(2);

    // position
    glGenBuffers(1, &VBOS[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[0]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT,
                          GL_FALSE, 12, (void*)0);
    glEnableVertexAttribArray(0);

    //color
    glGenBuffers(1, &VBOS[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[1]);
    glBufferData(GL_ARRAY_BUFFER, colorIndices.size() * sizeof(int), colorIndices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribIPointer(1, 1, GL_INT, 4,
                           (void*)0);
    glEnableVertexAttribArray(1);
}

void TriangleMesh::draw()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

TriangleMesh::~TriangleMesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(VBOS.size(), VBOS.data());
}
