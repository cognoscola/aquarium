#version 410

/** helps  visualize triangles without any textures or colours */
layout(triangles) in;
layout(line_strip, max_vertices = 4) out;

void main() {

    for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
         gl_Position = gl_in[i].gl_Position;
         EmitVertex();
    }
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();

  }