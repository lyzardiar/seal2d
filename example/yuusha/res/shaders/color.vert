#version 330

layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in vec4 vertex_color;

out vec4 frag_color;

void main() {
    gl_Position.xy = vertex_pos;
    gl_Position.z = 1.0;
    gl_Position.w = 1.0;
    
    frag_color = vertex_color;
}