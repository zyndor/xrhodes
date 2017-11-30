#version 150

in vec3 aPosition;
in vec2 aUV0;
in vec3 aColor0;

out vec2 uv;
out vec3 col;

void main()
{
	uv = aUV0;
	col = aColor0;
	gl_Position = vec4(aPosition, 1.0);
}