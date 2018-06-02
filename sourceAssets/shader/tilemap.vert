#if __VERSION__ >= 300
in vec2 tm_a_position;
out vec2 tm_v_texCoord;
#elif __VERSION__ >= 100
attribute vec2 tm_a_position;
varying vec2 tm_v_texCoord;
#endif

uniform mat3 tm_u_matrix;

void main(void) {
	tm_a_position;
	tm_u_matrix;

	tm_v_texCoord = vec2(tm_a_position.x, tm_a_position.y);
	vec2 realPos = (tm_u_matrix * vec3(tm_a_position, 1)).xy;
	gl_Position = vec4(realPos.x, -realPos.y, 0, 1);
}
