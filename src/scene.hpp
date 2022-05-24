#pragma once

#include "cgp/cgp.hpp"
#include "deformation/deformation.hpp"

// The element of the GUI that are not already stored in other structures
struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
};

struct environment_custom:cgp::scene_environment_basic_camera_spherical_coords { float rotat; };
void opengl_uniform(GLuint shader, environment_custom const& scene_environment);

// The structure of the custom scene
struct scene_structure {

	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	cgp::mesh_drawable global_frame;          // The standard global frame
	scene_environment_with_shader_deformation environment; // Standard environment controler
	cgp::inputs_interaction_parameters inputs; // Storage for inputs status (mouse, keyboard, window dimension)

	gui_parameters gui;

	// Standard GUI element storage

	cgp::mesh ocean_mesh;

	cgp::mesh_drawable terrain;
	cgp::mesh_drawable ocean;
	cgp::mesh_drawable tronc;
	cgp::mesh_drawable cone;
	cgp::mesh_drawable tree;
	cgp::mesh_drawable quad;
	cgp::mesh_drawable sun;
	cgp::skybox_drawable skybox;

	std::vector<cgp::vec3> tree_position;
	std::vector<cgp::vec3> quad_position;

	cgp::hierarchy_mesh_drawable clock_tower;
	cgp::timer_basic timer;
	

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();  // Standard initialization to be called before the animation loop
	void display();     // The frame display to be called within the animation loop
	void display_gui(); // The display of the GUI, also called within the animation loop
	void wave_update(cgp::mesh& mesh_shape, cgp::mesh_drawable& drawable, float t);

};





