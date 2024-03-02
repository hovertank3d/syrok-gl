#version 330 core

out vec4 FragColor;
in vec2 texPos;

uniform ivec2 screen_size; 
uniform sampler2D tex1;

float lumczt(vec3 rgb)
{
    // Algorithm from Chapter 10 of Graphics Shaders.
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    return dot(rgb, W);
}

vec3 syrok(vec3 p, int x, int y)
{
	float col = lumczt(p);
	int ucol = int(col*256.0);
	int level = ucol/16;
	int pattern = level < 10 ? x&y : x^y;
	
	pattern <<= 8 - (level + 1) / 2;
	//pattern &= 0xFF;
	return vec3(float(pattern)/255.0);
}

void main()
{
    vec3 col = texture(tex1, texPos).rgb;
    ivec2 iuv = ivec2(int(texPos.x * float(screen_size.x)),
	                  int(texPos.y * float(screen_size.y)));
    FragColor = vec4(syrok(col, iuv.x, iuv.y), 1.);
}
