#version 330

// From vertex shader
in vec2 texcoord;
uniform vec2 frame;
uniform vec2 scale;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float invulnerable_timer;
uniform float M_PI;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2((texcoord.x+frame.x)/scale.x, (texcoord.y+frame.y)/scale.y));
	if (invulnerable_timer > 0) {
		color *= vec4(0.3 + abs(cos(M_PI - M_PI * invulnerable_timer / 3000.f)) / 0.5, 0, 0, 1);
	}
}
