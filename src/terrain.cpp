
#include "terrain.hpp"


using namespace cgp;

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_height(float x, float y,float terrain_length)
{
    vec2 p_0[2] = { { 0,0},{4,4} };
    float h_0[2] = {13.0f,8.f};
    float sigma_0[2] = {15.f,25.f};
    float z = 0.f;
    float const noise = noise_perlin({ x/terrain_length +0.5f,y/terrain_length+0.5f }, 4,0.35f,3.f);
    for (int i = 0; i < 2; i++) {
        z += h_0[i]*std::exp(- std::pow(norm(vec2(x, y) - p_0[i]) / sigma_0[i],2));
    }
    return z*noise;
}

mesh create_terrain_mesh(int N, float terrain_length)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.uv.resize(N * N);

    // Fill terrain geometry
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku/(N-1.0f);
            float v = kv/(N-1.0f);

            // Compute the real coordinates (x,y) of the terrain 
            float x = (u - 0.5f) * terrain_length;
            float y = (v - 0.5f) * terrain_length;

            // Compute the surface height function at the given sampled coordinate
            float z = evaluate_terrain_height(x,y,terrain_length);

            // Store vertex coordinates
            terrain.position[kv+N*ku] = {x,y,z};
            terrain.uv[kv + N * ku] = { ku,kv };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(int ku=0; ku<N-1; ++ku)
    {
        for(int kv=0; kv<N-1; ++kv)
        {
            unsigned int idx = kv + N*ku; // current vertex offset

            uint3 triangle_1 = {idx, idx+1+N, idx+1};
            uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
	terrain.fill_empty_field(); 

    return terrain;
}

std::vector<cgp::vec3> generate_positions_on_terrain(int N, float terrain_length) 
{
    std::vector<cgp::vec3> pos;
    pos.resize(N);
    for (int i = 0; i < N; i++) {
        float x = rand_interval(-terrain_length/2, terrain_length/2);
        float y = rand_interval(-terrain_length / 2, terrain_length / 2);
        pos[i] = vec3(x, y, evaluate_terrain_height(x, y,terrain_length));
    }
    return pos;

}

