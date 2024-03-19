#include "mesh.h"

#include <fstream>
#include <string>
#include <exception>

uint32_t mesh::verts_count()
{
    return num_verts;
}

uint32_t mesh::tris_count()
{
    return num_tris;
}

mesh::mesh(uint32_t verts_capacity, uint32_t tris_capacity)
{
    num_tris = tris_capacity * 3;
    num_verts = verts_capacity;

    triangles = new uint32_t[num_tris];
    vertices = new vector3[num_verts];

    uvs = new vector2[num_tris];
    vertex_normals = new vector3[num_tris];
}

mesh::mesh(std::string path)
{
    std::ifstream file;
    file.open(path);
    if (!file.is_open()) throw new std::exception("unable to open file");


    // prepass to count vertices and triangles
    uint16_t found_vertices = 0;
    uint16_t found_triangles = 0;
    uint16_t found_uvs = 0;
    uint16_t found_vnorms = 0;

    std::string line;
    while (getline(file, line))
    {
        if (line.length() < 2) continue;
        if (line[0] == '#') continue;
        if (line[0] == 'v' && line[1] == ' ')
        {
            found_vertices++;
        }
        if (line[0] == 'f' && line[1] == ' ')
        {
            found_triangles++;
        }
        if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            found_uvs++;
        }
        if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
        {
            found_vnorms++;
        }
    }

    num_tris = found_triangles * 3;
    num_verts = found_vertices;

    triangles = new uint32_t[num_tris];
    vertices = new vector3[num_verts];

    vector2* uvs_temp = new vector2[found_uvs];
    vector3* vns_temp = new vector3[found_vnorms];
    
    // process the file
    file.clear();
    file.seekg(0, std::ios::beg);

    int v_ind = 0;
    int vt_ind = 0;
    int vn_ind = 0;
    int f_ind = 0;
    std::string type;
    vector3 tmp3;
    vector2 tmp2;

    while (true)
    {
        file >> type;
        if (type == "v")
        {
            // read vertex position
            file >> tmp3.x;
            file >> tmp3.y;
            file >> tmp3.z;
            vertices[v_ind] = tmp3;
            v_ind++;
        }
        else if (type == "f")
        {
            // tri TODO
        }
        else if (type == "vn")
        {
            // read vertex normal
            file >> tmp3.x;
            file >> tmp3.y;
            file >> tmp3.z;
            vns_temp[vn_ind] = tmp3;
            vn_ind++;
        }
        else if (type == "vt")
        {
            // read vertex uv
            file >> tmp2.x;
            file >> tmp2.y;
            uvs_temp[vt_ind] = tmp2;
            vt_ind++;
        }
        // skip to next line
        file.ignore('\n');
    }
}

mesh::~mesh()
{
    if (vertices) delete[] vertices;
    if (triangles) delete[] triangles;

    if (uvs) delete[] uvs;
    if (vertex_normals) delete[] vertex_normals;
}

