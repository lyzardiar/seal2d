#version 330

in vec2 vertex_pos;

void main() {
    gl_position.xy = vertex_pos;
    gl_position.z = 0.0;
    gl_position.w = 1.0;
}