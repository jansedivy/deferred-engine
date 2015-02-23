#version 330

in vec2 outUv;

uniform sampler2D uSampler;

out vec4 fragColor;

void main() {
  fragColor = texture(uSampler, outUv);
}
