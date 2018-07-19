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

uniform float u_alpha;
uniform vec3 u_colour;
uniform vec4 u_tint;
uniform sampler2D u_texture;

void main(void) { 
	fragColor = TEXTURE2D(u_texture, v_texCoord);

	// if (fragColor.a != 0.0) {
		float prevA = fragColor.a;
		fragColor = (u_tint - fragColor)*u_tint.a + fragColor;
		fragColor.a = prevA;
		// fragColor = mix(fragColor, vec4(u_tint.r*255.0, u_tint.g*255.0, u_tint.b*255.0, 255.0), u_tint.a/255.0); //@cleanup Better mix mode?

		fragColor.rgb /= fragColor.a;
		fragColor.a -= 1.0-u_alpha;
	// }

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
