#version 410

out vec4 outColor;

void main() {
  float i = 1.0 - gl_FragCoord.z*1.5;

  outColor = vec4(i*0.56, i*0.35, i*0.14, 1); // cor bronze
}