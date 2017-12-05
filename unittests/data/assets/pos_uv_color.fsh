#version 150

in vec2 uv;
in vec3 col;

uniform sampler2D xruSampler0;
out vec4 frag_colour;

void main()
{
	vec4 rgba = texture(xruSampler0, uv);
	frag_colour = mix(rgba, vec4(col, 1.0), .5);
}
