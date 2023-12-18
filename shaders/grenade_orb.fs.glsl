#version 330

// From vertex shader
in vec2 texcoord;
uniform vec2 frame;
uniform vec2 scale;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2((texcoord.x+frame.x)/scale.x, (texcoord.y+frame.y)/scale.y));
}
