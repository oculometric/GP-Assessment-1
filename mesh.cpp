#include "mesh.h"

#include <iostream>
#include <fstream>
#include <string>
#include <exception>

struct FaceCornerInfo { uint32_t vert; uint32_t uv; uint32_t vn; };

FaceCornerInfo splitFaceCorner(std::string str)
{
    FaceCornerInfo fci = { 0,0,0 };
    size_t first_break_ind = str.find('/');
    if (first_break_ind == std::string::npos) return fci;
    fci.vert = stoi(str.substr(0, first_break_ind)) - 1;
    size_t second_break_ind = str.find('/', first_break_ind + 1);
    if (second_break_ind != first_break_ind + 1) // FIXME: -1 needed here too?
        fci.uv = stoi(str.substr(first_break_ind + 1, second_break_ind - first_break_ind)) - 1;
    fci.vn = stoi(str.substr(second_break_ind + 1, str.find('/', second_break_ind+1) - second_break_ind)) - 1;

    return fci;
}

uint32_t Mesh::vertsCount()
{
    return num_verts;
}

uint32_t Mesh::trisCount()
{
    return num_tris;
}

Mesh::Mesh(uint32_t verts_capacity, uint32_t tris_capacity)
{
    num_tris = tris_capacity * 3;
    num_verts = verts_capacity;

    triangles = new uint32_t[num_tris];
    vertices = new Vector3[num_verts];

    uvs = new Vector2[num_tris];
    vertex_normals = new Vector3[num_tris];
    
    material = NULL;
}

Mesh::Mesh(std::string path)
{
    material = NULL;

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

    triangles = new uint32_t[num_tris + 3];
    vertices = new Vector3[num_verts];
    uvs = new Vector2[num_tris];
    vertex_normals = new Vector3[num_tris];

    Vector2* uvs_temp = NULL;
    Vector3* vns_temp = NULL;

    if (found_uvs != 0)
        uvs_temp = new Vector2[found_uvs];
    if (found_vnorms != 0)
        vns_temp = new Vector3[found_vnorms];
    
    // process the file, excluding triangles
    file.clear();
    file.seekg(0, std::ios::beg);

    int v_ind = 0;
    int vt_ind = 0;
    int vn_ind = 0;
    int f_ind = 0;
    std::string type;
    Vector3 tmp3;
    Vector2 tmp2;

    while (!file.eof())
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
            if (vn_ind >= found_vnorms) std::cout << "shit";
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
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // iterate the file again, processing triangles now. TODO: support non-tri faces
    std::string v_inds;
    FaceCornerInfo fci;
    file.clear();
    file.seekg(0, std::ios::beg);
    while (!file.eof())
    {
        file >> type;
        if (type == "f")
        {
            if (f_ind >= num_tris)
                break;
            // read triangle
            
            // first vert
            file >> v_inds;
            fci = splitFaceCorner(v_inds);

            if (fci.vert < found_vertices)
                triangles[f_ind] = fci.vert;
            else
                triangles[f_ind] = 0;
            if (fci.uv < found_uvs && found_uvs > 0)
                uvs[f_ind] = uvs_temp[fci.uv];
            if (fci.vn < found_vnorms && found_vnorms > 0)
                vertex_normals[f_ind] = vns_temp[fci.vn];
            f_ind++;

            // second vert
            file >> v_inds;
            fci = splitFaceCorner(v_inds);

            if (fci.vert < found_vertices)
                triangles[f_ind] = fci.vert;
            else
                triangles[f_ind] = 0;
            if (fci.uv < found_uvs && found_uvs > 0)
                uvs[f_ind] = uvs_temp[fci.uv];
            if (fci.vn < found_vnorms && found_vnorms > 0)
                vertex_normals[f_ind] = vns_temp[fci.vn];
            f_ind++;

            // third vert
            file >> v_inds;
            fci = splitFaceCorner(v_inds);

            if (fci.vert < found_vertices)
                triangles[f_ind] = fci.vert;
            else
                triangles[f_ind] = 0;
            if (fci.uv < found_uvs && found_uvs > 0)
                uvs[f_ind] = uvs_temp[fci.uv];
            if (fci.vn < found_vnorms && found_vnorms > 0)
                vertex_normals[f_ind] = vns_temp[fci.vn];
            f_ind++;

            // recompute normal and fill in blanks
            Vector3 v01 = vertices[triangles[f_ind - 2]] - vertices[triangles[f_ind - 3]];
            Vector3 v02 = vertices[triangles[f_ind - 1]] - vertices[triangles[f_ind - 3]];
            Vector3 normal = norm(v01 % v02);
            if (mag(vertex_normals[f_ind - 3]) < 0.01f) vertex_normals[f_ind - 3] = normal;
            if (mag(vertex_normals[f_ind - 2]) < 0.01f) vertex_normals[f_ind - 2] = normal;
            if (mag(vertex_normals[f_ind - 1]) < 0.01f) vertex_normals[f_ind - 1] = normal;
        }
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // clean up
    file.close();

    if (found_uvs > 0) delete[] uvs_temp;
    if (found_vnorms > 0) delete[] vns_temp;

    bounds_min = Vector3{ 0,0,0 };
    bounds_max = Vector3{ 0,0,0 };
    for (size_t i = 0; i < num_verts; i++)
    {
        Vector3 vert = vertices[i];

        if (vert.x < bounds_min.x) bounds_min.x = vert.x;
        else if (vert.x > bounds_max.x) bounds_max.x = vert.x;

        if (vert.y < bounds_min.y) bounds_min.y = vert.y;
        else if (vert.y > bounds_max.y) bounds_max.y = vert.y;

        if (vert.z < bounds_min.z) bounds_min.z = vert.z;
        else if (vert.z > bounds_max.z) bounds_max.z = vert.z;
    }

    std::cout << "loaded object " << path << std::endl;
    std::cout << "    tris: " << found_triangles << std::endl;
    std::cout << "    verts: " << found_vertices << std::endl;
    std::cout << "    uvs: " << found_uvs << std::endl;
    std::cout << "    vnorms: " << found_vnorms << std::endl;
    std::cout << "    bounds: " << bounds_min << " : " << bounds_max << std::endl;
}

Mesh::~Mesh()
{
    if (vertices) delete[] vertices;
    if (triangles) delete[] triangles;

    if (uvs) delete[] uvs;
    if (vertex_normals) delete[] vertex_normals;
}

