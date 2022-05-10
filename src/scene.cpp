#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"

using namespace cgp;

void scene_structure::initialize()
{
	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");
	environment.camera.axis = camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ 15.0f,6.0f,6.0f }, { 0,0,0 });

	int N_terrain_samples = 200;
	float terrain_length = 100;
	mesh const terrain_mesh = create_terrain_mesh(N_terrain_samples, terrain_length);
	terrain.initialize(terrain_mesh, "terrain");
	terrain.shading.color = { 0.6f,0.85f,0.5f };
	terrain.shading.phong.specular = 0.0f; // non-specular terrain material

	/*
	mesh const tree_mesh = create_tree();
	tree.initialize(tree_mesh, "tree");
	tree_position = generate_positions_on_terrain(50,terrain_length);
	GLuint const terrain_text = opengl_load_texture_image("texture_grass.jpg", GL_REPEAT, GL_REPEAT);
	terrain.texture = terrain_text;

	mesh quad_mesh= mesh_primitive_quadrangle({ -0.5f,0,0 }, { 0.5f,0,0 }, { 0.5f,0,1 }, { -0.5f,0,1 });
	quad.initialize(quad_mesh, "billboard");
	quad_position = generate_positions_on_terrain(100, terrain_length);
	quad.texture = opengl_load_texture_image("grass.png");
	quad.shading.phong = { 0.4f,0.6f,0,1 };

	*/

}



void scene_structure::display()
{


	// Basic elements of the scene
	environment.light = environment.camera.position();
	if (gui.display_frame)
		draw(global_frame, environment);

	draw(terrain, environment);
	/*
	for (int i = 0; i < 5; i++) {
		tree.transform.translation = tree_position[i];
		draw(tree, environment);

	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);
	vec3 const front = normalize(environment.camera.front() * vec3 { 1, 1, 0 }); // front-vector of the camera without z-component
	vec3 const right = environment.camera.right();
	rotation_transform R = rotation_transform::between_vector({ 1,0,0 }, { 0,1,0 }, right, front);
	for (int i = 0; i < 100; i++) {
		quad.transform.translation = quad_position[i];
		// Re-orient the grass shape to always face the camera direction

		// Rotation such that R*{1,0,0} = right-direction, R*{0,1,0} = front-direction
		
		quad.transform.rotation = R;
		draw(quad, environment);

	}
	glDepthMask(true);
	glDisable(GL_BLEND);
	*/
	if (gui.display_wireframe) {
		draw_wireframe(terrain, environment);
		/*draw_wireframe(tree, environment);*/
	}


}





void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}


