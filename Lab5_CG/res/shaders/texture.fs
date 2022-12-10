//this var comes from the vertex shader
varying vec2 v_coord;
varying vec3 v_wPos;
varying vec3 v_wNormal;

//the texture passed from the application
uniform sampler2D color_texture;

void main()
{
	//read the pixel RGBA from the texture at the position v_coord
	vec4 color = texture2D( color_texture, v_coord );
	
	//here we set up the normal as a color to see them as a debug

	//set the ouput color por the pixel
	gl_FragColor = vec4( color, 1.0 ) * 1.0;
}


