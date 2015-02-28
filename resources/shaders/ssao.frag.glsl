#version 330

#define CAP_MIN_DISTANCE 0.0001
#define CAP_MAX_DISTANCE 0.005

uniform sampler2D uDepth;
uniform sampler2D uNormal;
uniform sampler2D noiseTexture;
uniform sampler2D diffuseTexture;
uniform float noiseScale;
uniform float uRadius;

uniform mat4 invProjection;

uniform vec3 kernel[128];
uniform int uKernelSize;

uniform mat4 uPMatrix;

in vec2 pos;

out vec4 fragColor;

float linearDepth(vec2 coord) {
  float depth = texture(uDepth, coord).r;

  return depth;
}

vec4 getViewPos(vec2 texCoord) {
  // Calculate out of the fragment in screen space the view space position.

  float x = texCoord.s * 2.0 - 1.0;
  float y = texCoord.t * 2.0 - 1.0;

  // Assume we have a normal depth range between 0.0 and 1.0
  float z = linearDepth(texCoord) * 2.0 - 1.0;

  vec4 posProj = vec4(x, y, z, 1.0);

  vec4 posView = invProjection * posProj;

  posView /= posView.w;

  return posView;
}

void main(void)
{
  vec2 uv = pos * 0.5 + 0.5;

  vec4 posView = getViewPos(uv);
  vec3 normalView = normalize(texture(uNormal, uv).xyz * 2.0 - 1.0);

  vec3 randomVector = normalize(texture(noiseTexture, uv * noiseScale).xyz * 2.0 - 1.0);
  vec3 tangentView = normalize(randomVector - dot(randomVector, normalView) * normalView);
  vec3 bitangentView = cross(normalView, tangentView);
  mat3 tbn = mat3(tangentView, bitangentView, normalView);

  float occlusion = 0.0;
  for (int i = 0; i < uKernelSize; ++i) {
    vec3 sampleVectorView = tbn * kernel[i];

    vec4 samplePointView = posView + uRadius * vec4(sampleVectorView, 0.0);

    vec4 samplePointNDC = uPMatrix * samplePointView;

    samplePointNDC /= samplePointNDC.w;

    vec2 samplePointTexCoord = samplePointNDC.xy * 0.5 + 0.5;
    float zSceneNDC = linearDepth(samplePointTexCoord) * 2.0 - 1.0;

    float rangeCheck = smoothstep(0.0, 1.0, uRadius / abs(samplePointNDC.z - zSceneNDC));
    occlusion += rangeCheck * step(zSceneNDC, samplePointNDC.z);
  }

  occlusion = 1.0 - (occlusion / (float(uKernelSize) - 1.0));
  /* occlusion = pow(occlusion, 2.0); */

  fragColor = vec4(texture(diffuseTexture, uv).xyz * occlusion, 1.0);
}
