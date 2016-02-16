#version 330

layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in vec2 vertex_uv;

out vec4 frag_color;
out vec2 frag_uv;

void main() {
    gl_Position.x = vertex_pos.x;
    gl_Position.y = vertex_pos.y;
    gl_Position.z = 1.0;
    gl_Position.w = 1.0;
    
    frag_color = vertex_color;
    frag_uv = vec2(vertex_uv.x, 1.0f - vertex_uv.y);
}