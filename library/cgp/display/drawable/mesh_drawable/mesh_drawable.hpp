#pragma once

#include <map>
#include <string>
#include "cgp/display/opengl/opengl.hpp"
#include "cgp/shape/mesh/mesh.hpp"
#include "cgp/display/drawable/shading_parameters/shading_parameters.hpp"

namespace cgp
{
	struct mesh_drawable
	{
		mesh_drawable();
		mesh_drawable& initialize(mesh const& data_to_send, std::string const& object_name = "unset_name", GLuint shader = default_shader, GLuint texture = default_texture);
		mesh_drawable& clear();

		mesh_drawable& update_position(buffer<vec3> const& new_position);
		mesh_drawable& update_normal(buffer<vec3> const& new_normal);
		mesh_drawable& update_color(buffer<vec3> const& new_color);
		mesh_drawable& update_uv(buffer<vec2> const& new_uv);


		// Stores VBO ID in GPU_elements_id
		std::map<std::string, GLuint> vbo;
		GLuint vao;

		GLuint number_triangles;
		GLuint shader;
		GLuint texture;
		GLuint texture_1 = 0;

		// Uniform
		affine_rts transform;
		vec3 anisotropic_scale;
		shading_parameters_phong shading;
		mat4 model_matrix() const;

		std::string name;
		

		static GLuint default_shader;
		static GLuint default_texture;



	};

	template <typename SCENE_ENVIRONMENT>
	void draw(mesh_drawable const& drawable, SCENE_ENVIRONMENT const& environment);

	template <typename SCENE_ENVIRONMENT>
	void draw_wireframe(mesh_drawable const& drawable, SCENE_ENVIRONMENT const& environment, vec3 const& color={0,0,1});


	// Additional possibilities to split the drawing calls between sub-functions of CGP
	//  This allows to add custom command between the drawing process in spliting the shader setup/uniform send/draw call without modifying the environment.
	//  The full draw call (similar to draw(mesh_drawable)) is then
	//     draw_split::set_shader(mesh_drawable.shader);
	//     draw_split::set_texture(mesh_drawable.texture);
	//     draw_split::send_uniform(mesh_drawable);
	//       ... add any custom uniform here
	//     opengl_uniform(mesh_drawable.shader, environment);
	//     draw_split::draw_call(mesh_drawable);
	namespace draw_split {
		void set_shader(GLuint shaderID);
		// Bind a basic 2D texture at GL_TEXTURE0 and send the corresponding 0-value as uniform to "image_texture" variable
		void set_texture(GLuint textureID, GLuint shaderID); 
		// Send basic uniforms of mesh drawable (shading and Model matrix)
		void send_uniform(mesh_drawable const& drawable);
		// Call the display of the triangles
		void draw_call(mesh_drawable const& drawable);
	}
}


namespace cgp
{
	template <typename SCENE_ENVIRONMENT>
	void draw(mesh_drawable const& drawable, SCENE_ENVIRONMENT const& environment)
	{
		if (drawable.number_triangles == 0) return;

		// Setup shader
		assert_cgp(drawable.shader!=0, "Try to draw mesh_drawable without shader [name:"+ drawable.name+"]");
		assert_cgp(drawable.texture!=0, "Try to draw mesh_drawable without texture [name:"+ drawable.name+"]");
		glUseProgram(drawable.shader); opengl_check;

		// Send uniforms for this shader
		opengl_uniform(drawable.shader, environment);
		opengl_uniform(drawable.shader, drawable.shading);
		opengl_uniform(drawable.shader, "model", drawable.model_matrix());

		// Set texture
		glActiveTexture(GL_TEXTURE0); opengl_check;
		glBindTexture(GL_TEXTURE_2D, drawable.texture); opengl_check;
		opengl_uniform(drawable.shader, "image_texture", 0);  opengl_check;


		glActiveTexture(GL_TEXTURE1); opengl_check;
		glBindTexture(GL_TEXTURE_2D, drawable.texture_1); opengl_check;
		opengl_uniform(drawable.shader, "image_texture_1", 1);  opengl_check;
		
		// Call draw function
		assert_cgp(drawable.number_triangles>0, "Try to draw mesh_drawable with 0 triangles [name:"+ drawable.name+"]"); opengl_check;
		glBindVertexArray(drawable.vao);   opengl_check;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.vbo.at("index")); opengl_check;
		glDrawElements(GL_TRIANGLES, GLsizei(drawable.number_triangles*3), GL_UNSIGNED_INT, nullptr); opengl_check;

		// Clean buffers
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	template <typename SCENE_ENVIRONMENT>
	void draw_wireframe(mesh_drawable const& drawable, SCENE_ENVIRONMENT const& scene, vec3 const& color)
	{
		mesh_drawable wireframe = drawable;
		wireframe.shading.phong = {1.0f,0.0f,0.0f,64.0f};
		wireframe.shading.color = color;
		wireframe.shading.use_texture = false;
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0, 1.0);
		draw(wireframe, scene);
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}



}