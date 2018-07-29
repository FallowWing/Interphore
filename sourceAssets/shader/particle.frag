precision mediump float;

#if __VERSION__ >= 300
in vec2 v_texCoord;
out vec4 fragColor;
#define TEXTURE2D texture
#elif __VERSION__ >= 100
varying vec2 v_texCoord;
vec4 fragColor;
#define TEXTURE2D texture2D
#endif

uniform sampler2D u_texture;

void main() {
	fragColor = TEXTURE2D(u_texture, v_texCoord);
#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}  
