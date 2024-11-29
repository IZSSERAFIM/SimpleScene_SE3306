#version 330 core

out vec4 FragColor;

in float Height;
in vec2 TexCoords;
in vec4 Color;

uniform sampler2D gTextureHeight0;
uniform sampler2D gTextureHeight1;
uniform sampler2D gTextureHeight2;
uniform sampler2D gTextureHeight3;

uniform float gHeight0 = 1.0;
uniform float gHeight1 = 2.0;
uniform float gHeight2 = 3.02;
uniform float gHeight3 = 3.99;


vec4 CalcTexColor(){
	vec4 TexColor;
	if (Height < gHeight0){
		TexColor = texture(gTextureHeight0, TexCoords);
	}
	else if (Height < gHeight1){
		vec4 Color0 = texture(gTextureHeight0, TexCoords);
		vec4 Color1 = texture(gTextureHeight1, TexCoords);
		float Delta = gHeight1 - gHeight0;
		float Factor = (Height - gHeight0) / Delta;
		TexColor = mix(Color0, Color1, Factor);
	}
	else if (Height < gHeight2){
		vec4 Color0 = texture(gTextureHeight1, TexCoords);
		vec4 Color1 = texture(gTextureHeight2, TexCoords);
		float Delta = gHeight2 - gHeight1;
		float Factor = (Height - gHeight1) / Delta;
		TexColor = mix(Color0, Color1, Factor);
	}
	else if (Height < gHeight3){
		vec4 Color0 = texture(gTextureHeight2, TexCoords);
		vec4 Color1 = texture(gTextureHeight3, TexCoords);
		float Delta = gHeight3 - gHeight2;
		float Factor = (Height - gHeight2) / Delta;
		TexColor = mix(Color0, Color1, Factor);
	}
	else{
		TexColor = texture(gTextureHeight3, TexCoords);
	}
	return TexColor;
}

void main()
{
    vec4 TexColor = CalcTexColor();
    FragColor = Color * TexColor;
}