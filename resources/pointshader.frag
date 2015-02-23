#version 330

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2D depthTexture;

uniform vec3 lightColor;
uniform vec3 lightDirection;
uniform vec3 lightPosition;
uniform mat4 invProjection;
uniform float lightRadius;

out vec4 fragColor;

vec3 decode(vec4 enc) {
  return enc.xyz * 2.0 - 1.0;
}

void main() {
  vec2 uv = gl_FragCoord.xy / vec2(1280.0, 720.0);

  vec3 normalColor = decode(texture(normalTexture, uv));
  vec3 diffuseColor = texture(diffuseTexture, uv).xyz;
  vec3 positionColor = texture(positionTexture, uv).xyz;
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

  fragColor = attenuation * vec4(diffuse, 1.0) * vec4(diffuseColor, 1.0);
}
