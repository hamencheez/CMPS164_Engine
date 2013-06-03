#version 150

//These variables are constant for all vertices
uniform mat4 M; //modelview matrix
uniform mat4 P; //projection matrix
uniform mat3 M_n; //normal matrix
uniform float time; //time variable

//input variables from host
in vec3 pos; //vertex position
in vec3 norm; //vertex normal

//variables to be passed to the fragment shader
out vec4 frag_color;

void main() {
    gl_Position = P * (M * vec4(pos, 1.0));
    
    //determine vertex color based on position and time
    //vec4 color = vec4(time * pos.x, (1.0 - time) * pos.y , time * pos.z, 1.0);
	vec4 color = vec4(pos.x*sin(pos.z+time*2.0), pos.y*cos(pos.x+time*2.0) , pos.z*sin(pos.y+time*2.0), 1.0);
    frag_color = vec4(norm.x,norm.y,norm.z,1.0);
}
