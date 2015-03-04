#version 330

uniform sampler2D diffuseTexture;

in vec2 pos;

out vec4 fragColor;

void main(void)
{
  vec2 uv = pos * 0.5 + 0.5;

  vec3 diffuse = texture(diffuseTexture, uv).xyz;

  vec4 sum = vec4(0.0);
  for (int x=-4; x<=4; x++) {
    for (int y=-4; y<=4; y++) {
      vec2 offset = vec2(x, y) * 0.00005;
      sum += texture(diffuseTexture, offset + uv);
    }
  }

  fragColor = vec4(diffuse + sum.xyz / 81.0, 1.0);
}
