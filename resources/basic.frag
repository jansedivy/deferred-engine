#version 330

in vec4 vPosition;
in vec3 transformedNormals;
in vec2 texturePosition;

uniform vec3 camera;
uniform sampler2D uSampler;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 positionOut;

void main() {
  vec3 diff = texture(uSampler, texturePosition).xyz;
  /* vec3 surfaceToCamera = normalize(camera - vPosition.xyz); */
  vec3 normal = normalize(transformedNormals);
  /* vec3 normalToLight = normalize(vec3(-10.202742, 22.519585, 4.284930) - vPosition.xyz); */
  /* float surface = min(dot(normal, normalToLight), 1.0); */
  /* float specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-normalToLight, normal))), 10.0); */

  // fragColor = vec4(diff * surface + vec3(1.0, 0.6, 0.6) * specularCoefficient, 1.0);
  fragColor = vec4(diff, 1.0);
  normalOut = vec4(normal, 1.0);
  positionOut = vPosition;
}
