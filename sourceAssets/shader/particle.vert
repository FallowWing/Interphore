#if __VERSION__ >= 300
in vec2 a_position;
in vec2 a_texcoord;
out vec2 v_texCoord;
#elif __VERSION__ >= 100
attribute vec2 a_position;
attribute vec2 a_texcoord;
varying vec2 v_texCoord;
#endif
// out var for particle colour?

uniform mat3 u_matrix;
uniform vec2 offset;
// uniform vec4 color;

void main() {
	float scale = 10.0f;
	v_texCoord = a_texcoord;
	// ParticleColor = color;
	gl_Position = vec4((u_matrix * vec3(a_position + offset, 1)).xy, 0, 1);
}
