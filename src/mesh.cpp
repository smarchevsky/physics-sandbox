#include "mesh.h"
//#include "meshdata.h"

//#include <assimp/Importer.hpp>
//#include <assimp/postprocess.h>
//#include <assimp/scene.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
//#include <hashtable.h>
#include <iostream>
#include <string>

//using namespace std;

Mesh::Mesh(MeshData &&mData) {
    auto vertexArray = mData.m_vertexArray;
    auto indexArray = mData.indices;
    auto type = mData.m_type;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexArray.size() * sizeof(GLfloat),
                 vertexArray.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArray.size() * sizeof(GLuint),
                 indexArray.data(), GL_STATIC_DRAW);
    numOfVertices = indexArray.size();
    switch (type) {
    case MeshType::V: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                              (GLvoid *)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

    } break;

    case MeshType::VT: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                              (GLvoid *)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                              (GLvoid *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    } break;

    case MeshType::VN: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                              (GLvoid *)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                              (GLvoid *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    } break;

    case MeshType::VTN: {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (GLvoid *)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (GLvoid *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (GLvoid *)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    } break;
    default:
        throw("Bad mesh type");
        break;
    }

    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so afterwards we
    // can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void Mesh::draw() {
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    glDrawElements(GL_TRIANGLES, numOfVertices, GL_UNSIGNED_INT, 0);
}

Mesh::~Mesh() {
    // check for cached data
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

std::vector<scalar> makeSingleArray(const std::vector<scalar> &positions, const std::vector<scalar> &normals, const std::vector<scalar> &texcoords,
                                    const MeshType type) {
    std::vector<scalar> array;
    std::string error(std::string(__FUNCTION__) + " Arrays does not match: ");

    switch (type) {
    case MeshType::V:
        array = positions;
        break;

    case MeshType::VN:
        if (positions.size() != normals.size())
            throw(error + "VN");
        array.reserve(positions.size() + normals.size());
        for (int i = 0; i < positions.size(); ++i) {
            array.push_back(positions[i]);
            array.push_back(normals[i]);
        }
        break;

    case MeshType::VT:
        if (positions.size() != texcoords.size())
            throw(error + "VT");
        array.reserve(positions.size() + texcoords.size());
        for (int i = 0; i < positions.size(); ++i) {
            array.push_back(positions[i]);
            array.push_back(texcoords[i]);
        }
        break;

    case MeshType::VTN:
        if (positions.size() != normals.size() || positions.size() != texcoords.size() / 2 * 3)
            throw(error + "VTN");
        array.reserve(positions.size() + normals.size() + texcoords.size());
        for (int i = 0; i < positions.size(); ++i) {
            array.push_back(positions[i]);
            array.push_back(texcoords[i]);
            array.push_back(normals[i]);
        }

        break;
    //case MeshType::VTNTB:
    //break;
    default:
        throw("Bad mesh type");
    }

    return array;
}

MeshData MeshData::generateCube(MeshType type) {

    std::vector<float> positions{
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};
    std::vector<float> normals{
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,

        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,

        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f};
    std::vector<float> texcoords = {
        // positions          // normals           // texture coords
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f};

    MeshData result;
    result.m_vertexArray = makeSingleArray(positions, normals, texcoords, type);
    result.indices.reserve(36);
    result.m_type = type;
    for (int i = 0; i < 36; i++)
        result.indices[i] = i;

    return result;
}

MeshData MeshData::generateGeosphere(MeshType type) {
    std::vector<uint> indices{
        2, 1, 0, 3, 2, 0, 4, 3, 0, 5, 4, 0, 1, 5, 0, 11, 6, 7, 11, 7,
        8, 11, 8, 9, 11, 9, 10, 11, 10, 6, 1, 2, 6, 2, 3, 7, 3, 4, 8, 4,
        5, 9, 5, 1, 10, 2, 7, 6, 3, 8, 7, 4, 9, 8, 5, 10, 9, 1, 6, 10};
    std::vector<scalar> positions{
        0.000f, 0.000f, 1.000f,
        0.894f, 0.000f, 0.447f,
        0.276f, 0.851f, 0.447f,
        -0.724f, 0.526f, 0.447f,
        -0.724f, -0.526f, 0.447f,
        0.276f, -0.851f, 0.447f,
        0.724f, 0.526f, -0.447f,
        -0.276f, 0.851f, -0.447f,
        -0.894f, 0.000f, -0.447f,
        -0.276f, -0.851f, -0.447f,
        0.724f, -0.526f, -0.447f,
        0.000f, 0.000f, -1.000f};

    MeshData result;
    std::vector<scalar> texcoords;
    if (type == VT || type == VTN) {
        texcoords.reserve(positions.size());
        std::fill(texcoords.begin(), texcoords.end(), 0.f);
    }
    result.m_vertexArray = makeSingleArray(positions, positions, texcoords, type);
    result.indices.reserve(36);
    for (int i = 0; i < 36; i++)
        result.indices[i] = i;

    return result;
}