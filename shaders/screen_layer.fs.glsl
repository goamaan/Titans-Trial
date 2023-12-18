#version 330

// From vertex shader
in vec2 texcoord;
uniform float time;
uniform float screen_darken_factor;
uniform bool pause;


// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// Output color
layout(location = 0) out  vec4 color;

vec4 pause_effect(vec4 in_color)
{
	if (pause)
		in_color += vec4(0, 0, 0, 0.6);
	return in_color;
}

vec4 fade_color(vec4 in_color)
{
	if (screen_darken_factor > 0)
		in_color += vec4(0, 0, 0, 0.9 * (screen_darken_factor));
	return in_color;
}

void main()
{
    vec4 in_color = texture(sampler0, texcoord) * vec4(1,1,1,0);
    color = pause_effect(in_color);
    color = fade_color(color);
}
