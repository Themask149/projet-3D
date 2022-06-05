#version 330 core  // OpenGL 3.3 shader

// Fragment shader - this code is executed for every pixel/fragment that belongs to a displayed shape

// Inputs coming from tehe vertex shader
in struct fragment_data
{ 
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment
} fragment;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;

// Texture image
uniform sampler2D image_texture;
uniform sampler2D image_texture_1;

// Uniform values - must be send from the C++ code
uniform float rotat;

// Shape color
uniform vec3 color;   // Uniform color of the object
uniform float alpha;  // alpha coefficient

// Phong shading
uniform float Ka;     // Ambient coefficient
uniform float Kd;     // Diffuse coefficient
uniform float Ks;     // Specular coefficient
uniform float specular_exp; // Specular exponent

uniform mat4 view;       // View matrix (rigid transform) of the camera - to compute the camera position

void main()
{

	// Compute the position of the center of the camera
	mat3 O = transpose(mat3(view));                   // get the orientation matrix
	vec3 last_col = vec3(view*vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
	vec3 camera_position = -O*last_col;

	// Re-normalize the normals (interpolated on the triangle)
	vec3 N = normalize(fragment.normal);

	// Inverse the normal if it is viewed from its back
	//  (note: doesn't work on Mac)
	if (gl_FrontFacing == false) {  
		N = -N;
	}

	// Phong coefficient (diffuse, specular)
	// ************************* //

	// Unit direction toward the light
	vec3 lsun = { 150 * cos(3* rotat),0,150 * sin(3* rotat) };
	vec3 L = normalize(lsun-fragment.position);

	// Diffuse coefficient
	float diffuse = max(dot(N,L),0.0);

	// Specular coefficient
	float specular = 0.0;
	if(diffuse>0.0){
		vec3 R = reflect(-L,N); // symetric of light-direction with respect to the normal
		vec3 V = normalize(camera_position-fragment.position);
		specular = pow( max(dot(R,V),0.0), specular_exp );
	if(sin(3* rotat)<=0.1f) specular=10*sin(3* rotat)*specular;
	}

	// Texture
	// ************************* //
	
	// Current uv coordinates
	// by default inverse the v direction (avoids common image upside-down)
	vec2 uv_image = vec2(fragment.uv.x, 1.0-fragment.uv.y);

	// Get the current texture color
	vec4 color_image_texture = texture(image_texture, uv_image);
	vec4 color_image_texture2 = texture(image_texture_1, uv_image);
	float texture_alpha;


	// Compute Shading
	// ************************* //

	// Compute the base color of the object based on: vertex color, uniform color, and texture
	
	vec3 color_object;
	if (fragment.position.z<6.5f+cos(0.3*fragment.position.x)){
		color_object  = fragment.color * color * color_image_texture2.rgb;
		texture_alpha = color_image_texture2.a;
	}
	else{
		color_object  = fragment.color * color * color_image_texture.rgb;
		texture_alpha = color_image_texture.a;
	}
	// Compute the final shaded color using Phong model
	float modif;
	if(sin(3*rotat)<0.2) modif = 0.2;
	else modif=sin(3*rotat);
	vec3 color_shading = (Ka + Kd * diffuse * modif) * color_object + Ks * specular * vec3(1.0, 1.0, 1.0);
	
	// Output color, with the alpha component
	FragColor = vec4(color_shading, alpha * texture_alpha);
}