#if __VERSION__ >= 300
in vec2 rt_a_position;
in vec2 rt_a_texcoord;
out vec2 rt_v_texcoord;
#elif __VERSION__ >= 100
attribute vec2 rt_a_position;
attribute vec2 rt_a_texcoord;
varying vec2 rt_v_texcoord;
#endif

uniform mat3 rt_u_matrix;

void main(void) {
	rt_v_texcoord = rt_a_texcoord;
	gl_Position = vec4((rt_u_matrix * vec3(rt_a_position, 1)).xy, 0, 1);
}
