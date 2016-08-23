#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 8) out;

uniform vec2 pixel;
uniform float thickness;

// TODO: Colors...
in VS_OUT {
    vec4 vColor;
} gs_in[];
out vec4 fColor;
out float t;

void main()
{
    vec2 v = normalize( (gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy) / pixel );
    vec4 n = vec4(-v.y*pixel.x, v.x*pixel.y, 0, 0);

    float w = 0.5*thickness + 1;

//    fColor = vec4(gs_in[0].vColor.rgb, 0);
//    gl_Position = gl_in[0].gl_Position - (w+1)*n;
//    EmitVertex();
//    gl_Position = gl_in[1].gl_Position - (w+1)*n;
//    EmitVertex();

    fColor = gs_in[0].vColor;
    t = 0;
    gl_Position = gl_in[0].gl_Position - w*n;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position - w*n;
    EmitVertex();

    t = 1;
    gl_Position = gl_in[0].gl_Position + w*n;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + w*n;
    EmitVertex();

//    fColor = vec4(gs_in[0].vColor.rgb, 0);
//    gl_Position = gl_in[0].gl_Position + (w+1)*n;
//    EmitVertex();
//    gl_Position = gl_in[1].gl_Position + (w+1)*n;
//    EmitVertex();

    EndPrimitive();
}
