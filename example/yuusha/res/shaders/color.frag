#version 330


in vec4 frag_color;
out vec4 color;

void main() {
//    color = vec4(1.0, 1.0, 0.0, 1.0);
        color.r = frag_color.r;
        color.g = frag_color.g;
        color.b = frag_color.b;
        color.a = frag_color.a;
}