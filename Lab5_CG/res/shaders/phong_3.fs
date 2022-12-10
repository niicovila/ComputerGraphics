//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_wPos;
varying vec2 v_coord;



//here create uniforms for all the data we need here
uniform vec3 camera_position;
uniform vec3 ambient_light;


uniform vec3 light_position;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform vec3 material_ambient;


uniform float material_shininess;

uniform sampler2D color_texture; 
uniform sampler2D normal_texture;
uniform mat4 model;


void main()
{
	//here we set up the normal as a color to see them as a debug
	vec3 texture_normal = texture2D( normal_texture, v_coord ).xyz;

	texture_normal = (texture_normal - 0.5) / 0.5; // adapt the range [0, 1] to [-1, 1]
	vec3 wNormal = (model * vec4( texture_normal, 0.0)).xyz; // normal is affected by the model matrix

	//here write the computations for PHONG.
	vec3 N = normalize(wNormal);
	vec3 L = normalize(light_position - v_wPos); // Get a lighting direction vector from the light to the vertex.
	vec3 V = normalize(camera_position - v_wPos);
	vec3 R = reflect(-L, N);

	vec4 tex_color = texture2D( color_texture, v_coord );


	vec3 Kd = tex_color.xyz * material_diffuse;
	vec3 Ks = (tex_color.xyz*tex_color.w) * material_specular;
	vec3 Ka = tex_color.xyz*material_ambient;

	vec3 Ld = Kd * max(dot(N, L), 0.0) * light_diffuse;
	vec3 Ls =  Ks * pow(max(dot(R, V), 0.0), material_shininess) * light_specular;
	vec3 La =  Ka * ambient_light;

	vec3 color =  Ld + Ls + La;

	//set the ouput color por the pixel
	gl_FragColor = vec4( color, 1.0 ) * 1.0;
}
