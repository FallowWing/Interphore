#if __VERSION__ >= 300
in vec2 p_a_position;
in vec2 p_a_texcoord;
out vec2 v_texCoord;
#elif __VERSION__ >= 100
attribute vec2 p_a_position;
attribute vec2 p_a_texcoord;
varying vec2 v_texCoord;
#endif
// out var for particle colour?

uniform mat3 p_u_matrix;
uniform vec2 p_u_offset;
// uniform vec4 color;

void main() {
	float scale = 10.0f;
	v_texCoord = p_a_texcoord;
	// ParticleColor = color;
	gl_Position = vec4((p_u_matrix * vec3(p_a_position + p_u_offset, 1)).xy, 0, 1);
}
