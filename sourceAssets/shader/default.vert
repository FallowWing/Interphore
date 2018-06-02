#if __VERSION__ >= 300
in vec2 a_position;
in vec2 a_texcoord;
out vec2 v_texCoord;
#elif __VERSION__ >= 100
attribute vec2 a_position;
attribute vec2 a_texcoord;
varying vec2 v_texCoord;
#endif

uniform mat3 u_matrix;
uniform mat3 u_uv;

void main(void) {
	v_texCoord = vec4((u_uv * vec3(a_texcoord, 1)).xy, 0, 1).xy;
	gl_Position = vec4((u_matrix * vec3(a_position, 1)).xy, 0, 1);
}

