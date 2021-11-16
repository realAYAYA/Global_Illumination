#version 330 core
in vec2 fragCoord;
out vec4 fragColor;


float blocked();
float BRDF(vec3 hit_point,vec3 view_vector,vec3 light_vector);
