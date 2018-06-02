precision highp float;

#if __VERSION__ >= 300
in vec2 tm_v_texCoord;
out vec4 fragColor;
#define TEXTURE2D texture
#elif __VERSION__ >= 100
vec4 fragColor;
varying vec2 tm_v_texCoord;
#define TEXTURE2D texture2D
#endif

uniform sampler2D tm_u_tilesetTexture;
uniform sampler2D tm_u_tilemapTexture;
uniform ivec2 tm_u_tilesetSize;
uniform ivec2 tm_u_tilemapSize;
uniform ivec2 tm_u_resultSize;
uniform ivec2 tm_u_pixelRatio;

void main(void) {
	tm_u_tilesetTexture;
	tm_u_tilemapTexture;
	tm_u_tilesetSize;
	tm_u_tilemapSize;
	tm_u_resultSize;
	tm_u_pixelRatio;

	vec2 dataTex;
	dataTex.x = tm_v_texCoord.x / float(tm_u_resultSize.x);
	dataTex.y = tm_v_texCoord.y / float(tm_u_resultSize.y);
	// realTex *= vec2(1, -1); // Flip because default.frag expects all textures to be upside-down
	
	ivec2 tilesetSizeInTiles = tm_u_tilesetSize / tm_u_pixelRatio;

	vec4 dataFrag = TEXTURE2D(tm_u_tilemapTexture, dataTex);
	int tileIndex = 0;
	tileIndex += int(dataFrag.r*256.0);
	tileIndex += 256 * int(dataFrag.g*256.0);
	//tileIndex += (256*2) * int(dataFrag.b*256.0);
	//tileIndex += (256*3) * int(dataFrag.a*256.0);
	tileIndex -= 1;
	if (tileIndex == -1) discard;
	ivec2 tilePos = ivec2(mod(float(tileIndex), float(tilesetSizeInTiles.x)), tileIndex / tilesetSizeInTiles.x);
	vec2 uv = vec2(tilePos) / vec2(tm_u_tilesetSize);
	
	// fragColor = vec4(float(tilePos.x)/10.0, float(tilePos.y)/10.0, float(tileIndex)/100.0, 1);
	// fragColor = vec4(float(tm_u_tilesetSize.y)/100.0, 0.0, 0.0, 1);
	// return;

	// int x = tm_u_tilesetSize.x + tm_u_tilemapSize.x + tm_u_resultSize.x + tm_u_pixelRatio.x;
	// fragColor = vec4(float(tm_u_tilesetSize.x)/10000.0, 0.0, float(x)*0.000001, 1.0);
	// return;

	vec2 offset = vec2(
		fract(dataTex.x * float(tm_u_tilemapSize.x)) / float(tm_u_tilesetSize.x),
		fract(dataTex.y * float(tm_u_tilemapSize.y)) / float(tm_u_tilesetSize.y)
	);
	
	//fragColor = vec4(offset.x*10.0, offset.y*10.0, 0.0, 1);
	//return;

	uv += offset;
	uv *= vec2(tm_u_pixelRatio);
	fragColor = TEXTURE2D(tm_u_tilesetTexture, uv);

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
