#include "triangle_mesh.h"
#include <vector>
#include <glad/glad.h>

TriangleMesh::TriangleMesh() {

    // x y z r g b
    // X轴：从左(-1.0)到右(+1.0)
    //
    // Y轴：从下(-1.0)到上(+1.0)
    //
    // Z轴：从远(-1.0)到近(+1.0)
    
    std::vector<float> data = {
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    
    vertex_count = 3;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    size_t bufferSize = data.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, data.data(), 
            GL_STATIC_DRAW);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, 
            GL_FALSE, 24, (void*)0);
    glEnableVertexAttribArray(0);

    //color
    glVertexAttribPointer(1, 3, GL_FLOAT, 
            GL_FALSE, 24, (void*)12);
    glEnableVertexAttribArray(1);
}

void TriangleMesh::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

TriangleMesh::~TriangleMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
