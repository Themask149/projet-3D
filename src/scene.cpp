#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"


using namespace cgp;

void opengl_uniform(GLuint shader, environment_custom const& scene_environment) { //permet d'utiliser rotat dans les shaders
	opengl_uniform(shader, cgp::scene_environment_basic_camera_spherical_coords(scene_environment));
	cgp::opengl_uniform(shader, "rotat", scene_environment.rotat);
}


void scene_structure::initialize()
{
	// Basic set-up
	// ***************************************** //

	GLuint const shader_deformation = opengl_load_shader("shaders/deformation/vert.glsl", "shaders/deformation/frag.glsl");
	GLuint const shader_terrain = opengl_load_shader("shaders/terrain/vert.glsl", "shaders/terrain/frag.glsl");

	global_frame.initialize(mesh_primitive_frame(), "Frame");
	environment.camera.axis= camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ 15.0f,6.0f,16.0f }, { 0,2,16 });

	int N_terrain_samples = 300;
	float terrain_length = 300;
	mesh const terrain_mesh = create_terrain_mesh(N_terrain_samples, terrain_length);
	terrain.initialize(terrain_mesh, "terrain");
	terrain.shading.color = { 0.6f,0.85f,0.5f };
	terrain.shading.phong.specular = 0.0f; // non-specular terrain material
	terrain.shading.phong.ambient = 0.2f;
	GLuint const terrain_text = opengl_load_texture_image("textures/sand.jpg", GL_REPEAT, GL_REPEAT);
	GLuint const herb_text = opengl_load_texture_image("textures/texture_grass.jpg", GL_REPEAT, GL_REPEAT);
	terrain.texture_1 = terrain_text;
	terrain.texture = herb_text;
	terrain.shader = shader_terrain;

	ocean_mesh = create_ocean_mesh(N_terrain_samples, terrain_length);
	ocean.initialize(ocean_mesh, "ocean");
	GLuint const ocean_text = opengl_load_texture_image("textures/water.jpg", GL_REPEAT, GL_REPEAT);
	ocean.texture = ocean_text;
	ocean.shader = shader_deformation;

	/*
	mesh const tree_mesh = create_tree();
	tree.initialize(tree_mesh, "tree");
	tree_position = generate_positions_on_terrain(50,terrain_length);
	GLuint const terrain_text = opengl_load_texture_image("texture_grass.jpg", GL_REPEAT, GL_REPEAT);
	terrain.texture = terrain_text;
	*/
	mesh quad_mesh= mesh_primitive_quadrangle({ -0.5f,0,0 }, { 0.5f,0,0 }, { 0.5f,0,1 }, { -0.5f,0,1 });
	quad.initialize(quad_mesh, "billboard");
	quad_position = generate_positions_on_terrain(300, terrain_length);
	quad.texture = opengl_load_texture_image("textures/grass.png");
	quad.shading.phong = { 0.4f,0.6f,0,1 };
	quad.shading.phong.ambient -= 0.2;


	//skybox
	skybox.initialize("textures/skybox/");

	sun.initialize(mesh_primitive_sphere(5.f, { 0,0,0 }));
	GLuint const text_sun = opengl_load_texture_image("textures/sun.jpg", GL_REPEAT, GL_REPEAT);
	sun.texture = text_sun;
	sun.shading.phong.ambient = 1;
	

	// clock tower
	mesh_drawable cube_base;
	mesh_drawable cube_2;
	mesh_drawable cylinder;
	mesh_drawable aiguille;
	mesh_drawable toit;

	cube_base.initialize(mesh_primitive_cube(), "Cube base"); cube_base.transform.scaling = 2.f;
	cube_2.initialize(mesh_primitive_cube(), "Cube 2"); cube_2.transform.scaling = 2.f;
	cylinder.initialize(mesh_primitive_cylinder(0.6f, { -0.1f,0,0 }, { 0,0,0 }, 10, 40, true), "Cylinder clock");
	aiguille.initialize(mesh_primitive_cylinder(0.02f, { 0,0,0 }, { 0,0,0.55f }, 10, 20, true), "Aiguille");
	toit.initialize(mesh_primitive_cone(1.8f, 1.8f, { 0,0,0 }, {0, 0, 1}, true), "Toit");

	GLuint const text_tuile = opengl_load_texture_image("textures/tuile.jpg", GL_REPEAT, GL_REPEAT);
	toit.texture = text_tuile;
	GLuint const text_brique = opengl_load_texture_image("textures/brique.jpg", GL_REPEAT, GL_REPEAT);
	cube_base.texture = text_brique;
	cube_2.texture = text_brique;
	clock_tower.add(cube_base);
	clock_tower.add(cube_2, "Cube base", { 0,0,2.f });
	cylinder.shading.phong.diffuse -= 0.2f;
	clock_tower.add(cylinder, "Cube 2", { 1.1f,0,0 });
	aiguille.shading.color = { 0.f,0.f,0.f };
	clock_tower.add(aiguille, "Cylinder clock", { 0.f,0,0 });
	clock_tower.add(toit, "Cube 2", { 0,0,1.f });


	mesh mesh_boat = mesh_load_file_obj("assets/boat.obj");
	boat.initialize(mesh_boat,"boat");
	boat.transform.scaling = 0.02f;
	boat.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 3.14159265358979/2);
	boat.transform.translation = { -2,-13,5.1f };
}


void scene_structure::display()
{
	
	draw(skybox, environment); //doit etre draw en tout premier
	timer.update();
	float t = timer.t;

	// Basic elements of the scene
	//environment.light = environment.camera.position();
	environment.light = { 150 * cos(3 * environment.rotat),0,150 * sin(3 * environment.rotat) }; //coordonn�es du soleil
	if (gui.display_frame)
		draw(global_frame, environment);

	//timer.update();
	clock_tower["Aiguille"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, -3 * environment.rotat);
	clock_tower.update_local_to_global_coordinates();
	clock_tower["Cube base"].transform.translation = vec3{2,2,14};
	
	environment.update(t);
	draw(terrain, environment);
	draw(ocean, environment);
	draw(clock_tower, environment);
	sun.transform.translation = { 150 * cos(3* environment.rotat),0,150 * sin(3* environment.rotat) };
	draw(sun, environment);
	draw(boat, environment);
	/*
	for (int i = 0; i < 5; i++) {
		tree.transform.translation = tree_position[i];
		draw(tree, environment);

	}
	*/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);
	vec3 const front = normalize(environment.camera.front() * vec3 { 1, 1, 0 }); // front-vector of the camera without z-component
	vec3 const right = environment.camera.right();
	rotation_transform R = rotation_transform::between_vector({ 1,0,0 }, { 0,1,0 }, right, front);
	for (int i = 0; i < 300; i++) {
		quad.transform.translation = quad_position[i];
		// Re-orient the grass shape to always face the camera direction

		// Rotation such that R*{1,0,0} = right-direction, R*{0,1,0} = front-direction

		quad.transform.rotation = R;
		draw(quad, environment);

	}
	glDepthMask(true);
	glDisable(GL_BLEND);
	

	if (gui.display_wireframe) {
		draw_wireframe(terrain, environment);
		/*draw_wireframe(tree, environment);*/
	}


}





void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	ImGui::SliderFloat("Hour", &environment.rotat, 0, 1.1f);

}


