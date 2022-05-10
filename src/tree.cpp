#include "tree.hpp"
#define M_PI 3.14159265

using namespace cgp;


mesh create_cylinder_mesh(float radius, float height)
{
    mesh m; 
    // To do: fill this mesh ...
    // ...
    // To add a position: 
    //   m.position.push_back(vec3{x,y,z})
    // Or in pre-allocating the buffer:
    //   m.position.resize(maximalSize);
    //   m.position[index] = vec3{x,y,z}; (with 0<= index < maximalSize)
    // 
    // Similar with the triangle connectivity:
    //  m.connectivity.push_back(uint3{index_1, index_2, index_3});
    int nbp = 50;
    m.position.resize(nbp);
    for (int i = 0; i < nbp / 2; i++) {
        float x = radius * std::cos(2 * 3.14f*i / (nbp / 2.f));
        float y = radius * std::sin(2 * 3.14f*i / (nbp / 2.f));
        vec3 point = vec3( x, y, 0);
        m.position[2*i]=point;
        vec3 point2 = vec3( x, y, height);
        m.position[2 * i + 1] = point2;
        
    }
    std::cout << m.position<<std::endl; 
    for (int i = 0; i < nbp/2; i++) {
        uint3 triangle = { (2*i)%nbp,(2*(i+1))%nbp,(2*(i+1)+1)%nbp};
        m.connectivity.push_back(triangle);
        uint3 triangle2 = { (2 * i) % nbp,(2 * (i + 1) + 1) % nbp,(2*i+1)% nbp};
        m.connectivity.push_back(triangle2);
    }

    // Need to call fill_empty_field() before returning the mesh 
    //  this function fill all empty buffer with default values (ex. normals, colors, etc).
    m.fill_empty_field();

    return m;
}

mesh create_cone_mesh(float radius, float height, float z_offset)
{
    mesh m; 
    int nbp = 50;
    m.position.resize(nbp + 2);
    for (int i = 0; i < nbp; i++) {
        float x = radius * std::cos(2 * 3.14f * i / (nbp / 2.f));
        float y = radius * std::sin(2 * 3.14f * i / (nbp / 2.f));
        vec3 point = vec3(x, y, z_offset);
        m.position[i] = point;
    }
    m.position[nbp] = vec3(0, 0, z_offset + height);
    m.position[nbp+1] = vec3(0, 0, z_offset);
    for (int i = 0; i < nbp; i++) {
        uint3 triangle = { i%nbp,(i + 1)%nbp,nbp };
        m.connectivity.push_back(triangle);
        uint3 triangle2 = { i % nbp,nbp + 1,(i + 1) % nbp };
        m.connectivity.push_back(triangle2);
    }

    m.fill_empty_field();
    return m;
}


mesh create_tree()
{
    float h = 0.7f; // trunk height
    float r = 0.1f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_cylinder_mesh(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });


    // Create a green foliage from 3 cones
    mesh foliage = create_cone_mesh(4 * r, 6 * r, 0.0f);      // base-cone
    foliage.push_back(create_cone_mesh(4 * r, 6 * r, 2 * r));   // middle-cone
    foliage.push_back(create_cone_mesh(4 * r, 6 * r, 4 * r));   // top-cone
    foliage.position += vec3(0, 0, h);                 // place foliage at the top of the trunk
    foliage.color.fill({ 0.4f, 0.6f, 0.3f });

    // The tree is composted of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}