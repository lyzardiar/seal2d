#version 330

in vec4 frag_color;
in vec2 frag_uv;

out vec4 color;

uniform sampler2D sampler;

void main() {
    vec4 texture_color = texture(sampler, frag_uv);
    color = texture_color;
}