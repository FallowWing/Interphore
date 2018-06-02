precision highp float;

#if __VERSION__ >= 300
in vec2 rt_v_texcoord;
out vec4 fragColor;
#define TEXTURE2D texture
#elif __VERSION__ >= 100
varying vec2 rt_v_texcoord;
vec4 fragColor;
#define TEXTURE2D texture2D
#endif

uniform sampler2D rt_u_texture;

uniform vec4 rt_u_tint;
uniform bool rt_u_bleed;

void main(void) { 
	vec4 textureFrag = TEXTURE2D(rt_u_texture, rt_v_texcoord);

	fragColor = (rt_u_tint - textureFrag)*rt_u_tint.a + textureFrag;
	if (rt_u_bleed == false) fragColor.a = textureFrag.a;

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
