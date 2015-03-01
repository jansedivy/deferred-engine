#version 330

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D specularTexture;

uniform vec3 lightColor;
uniform vec3 lightDirection;
uniform vec3 lightPosition;
uniform mat4 invProjection;
uniform float lightRadius;

uniform vec3 camera;

out vec4 fragColor;

vec3 decode(vec4 enc) {
  return enc.xyz * 2.0 - 1.0;
}

void main() {
  vec2 uv = gl_FragCoord.xy / vec2(1280.0, 720.0);

  vec3 normalColor = normalize(decode(texture(normalTexture, uv)));
  vec3 diffuseColor = texture(diffuseTexture, uv).xyz;
  float specularMap = texture(specularTexture, uv).x;
  float depth = texture(depthTexture, uv).x;

  vec4 view_pos = vec4(uv*2.0-1.0, depth*2.0 - 1.0, 1.0);
  view_pos = invProjection * view_pos;
  view_pos /= view_pos.w;

  vec3 light_dir = lightPosition - view_pos.xyz;
  float dist = length(light_dir);
  float size = lightRadius;
  float attenuation = 1.0 - pow( clamp(dist/size, 0.0, 1.0), 2.0);
  light_dir = normalize(light_dir);

  float n_dot_l = clamp(dot(light_dir, normalColor), 0.0, 1.0);
  vec3 diffuse = lightColor * n_dot_l;

  vec3 directionToEye = normalize(camera - view_pos.xyz);
  vec3 reflectDirection = normalize(reflect(light_dir, normalColor));

  float specularPower = 16.0;
  float specularIntensity = 4.0;

  vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);
  float specularFactor = dot(directionToEye, reflectDirection);
  specularFactor = pow(specularFactor, specularPower);

  if (specularFactor > 0) {
    specularColor = vec4(lightColor, 1.0) * specularIntensity * specularFactor * specularMap * n_dot_l;
  }

  fragColor = attenuation * (specularColor + vec4(diffuse, 1.0)) * vec4(diffuseColor, 1.0);
}
