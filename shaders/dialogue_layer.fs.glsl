#version 330

// From vertex shader
in vec2 texcoord;
uniform bool show_dialogue_screen;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

vec4 dialogue_effect(vec4 in_color)
{
	if (show_dialogue_screen)
		in_color += vec4(0, 0, 0, 0.6);
	return in_color;
}

void main()
{
    vec4 in_color = texture(sampler0, texcoord) * vec4(1,1,1,0);
	color = dialogue_effect(in_color);
}