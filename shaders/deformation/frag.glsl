#version 330 core

in struct fragment_data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;

	vec3 eye;
} fragment;

layout(location=0) out vec4 FragColor;

uniform sampler2D image_texture;
uniform sampler2D image_texture_1;

uniform float rotat;

uniform vec3 color = vec3(1.0, 1.0, 1.0); // Unifor color of the object
uniform float alpha = 1.0f; // alpha coefficient
uniform float Ka = 0.4; // Ambient coefficient
uniform float Kd = 0.6; // Diffuse coefficient
uniform float Ks = 0.6f;// Specular coefficient
uniform float specular_exp = 50.0; // Specular exponent
uniform bool use_texture = true;
uniform bool texture_inverse_y = false;


void main()
{
	vec3 N = normalize(fragment.normal);
	if (gl_FrontFacing == false) {
		N = -N;
	}
	vec3 lsun = { 150 * cos(3* rotat),0,150 * sin(3* rotat) };
	vec3 L = normalize(lsun-fragment.position);

	float diffuse = max(dot(N,L),0.0);
	float specular = 0.0;
	if(diffuse>0.0){
		vec3 R = reflect(-L,N);
		vec3 V = normalize(fragment.eye-fragment.position);
		specular = pow( max(dot(R,V),0.0), specular_exp );
	if(sin(3* rotat)<=0.1f) specular=10*sin(3* rotat)*specular;
	}


	vec2 uv_image = vec2(fragment.uv.x, 1.0-fragment.uv.y);
	if(texture_inverse_y) {
		uv_image.y = 1.0-uv_image.y;
	}
	vec4 color_image_texture = texture(image_texture, uv_image);
	if(use_texture==false) {
		color_image_texture=vec4(1.0,1.0,1.0,1.0);
	}
	vec3 color_object  = fragment.color * color * color_image_texture.rgb;
	float modif;
	if(sin(3*rotat)<0.2) modif = 0.2;
	else modif=sin(3*rotat);
	vec3 color_shading = (Ka + Kd * diffuse * modif) * color_object + Ks * specular * vec3(1.0, 1.0, 1.0);
	



	FragColor = vec4(color_shading, alpha * color_image_texture.a);
}
