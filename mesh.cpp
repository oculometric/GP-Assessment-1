#include "mesh.h"

#include <fstream>
#include <string>
#include <exception>

struct face_corner_info { uint32_t vert; uint32_t uv; uint32_t vn; };

face_corner_info split_face_corner(std::string str)
{
    face_corner_info fci;
    size_t first_break_ind = str.find('/');
    fci.vert = stoi(str.substr(first_break_ind)) - 1;
    size_t second_break_ind = str.find('/', first_break_ind + 1);
    fci.uv = stoi(str.substr(first_break_ind + 1, second_break_ind - first_break_ind));
    fci.vn = stoi(str.substr(second_break_ind + 1, str.find('/', second_break_ind+1) - second_break_ind));

    return fci;
}

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
    
    // process the file, excluding triangles
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
        if (file.eof()) break;
    }

    // iterate the file again, processing triangles now. TODO: support non-tri faces
    std::string v_inds;
    face_corner_info fci;

    while (true)
    {
        file >> type;
        if (type == "f")
        {
            // read triangle
            
            // first vert
            file >> v_inds;
            fci = split_face_corner(v_inds);

            triangles[f_ind] = fci.vert;
            uvs[f_ind] = uvs_temp[fci.uv];
            vertex_normals[f_ind] = vns_temp[fci.vn];
            f_ind++;

            // second vert
            file >> v_inds;
            fci = split_face_corner(v_inds);

            triangles[f_ind] = fci.vert;
            uvs[f_ind] = uvs_temp[fci.uv];
            vertex_normals[f_ind] = vns_temp[fci.vn];
            f_ind++;

            // third vert
            file >> v_inds;
            fci = split_face_corner(v_inds);

            triangles[f_ind] = fci.vert;
            uvs[f_ind] = uvs_temp[fci.uv];
            vertex_normals[f_ind] = vns_temp[fci.vn];
            f_ind++;
        }
        file.ignore('\n');
        if (file.eof()) break;
    }

    // clean up
    file.close();

    delete[] uvs_temp;
    delete[] vns_temp;
}

mesh::~mesh()
{
    if (vertices) delete[] vertices;
    if (triangles) delete[] triangles;

    if (uvs) delete[] uvs;
    if (vertex_normals) delete[] vertex_normals;
}

