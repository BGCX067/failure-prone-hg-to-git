#version 330 core

in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D tex;
uniform float time;

void main() {

     vec2 resolution = vec2(800.0, 600.0);
     vec2 halfres = resolution.xy/2.0;
     vec2 cPos = gl_FragCoord.xy;

    cPos.x -= 0.5*halfres.x*sin(time/2.0)+0.3*halfres.x*cos(time)+halfres.x;
    cPos.y -= 0.4*halfres.y*sin(time/5.0)+0.3*halfres.y*cos(time)+halfres.y;
    float cLength = length(cPos);

    vec2 uv = gl_FragCoord.xy/resolution.xy+(cPos/cLength)*sin(cLength/30.0-time*10.0)/25.0;
    vec3 col = texture2D(tex,uv).xyz*50.0/cLength;

    fragColor = vec4(col,1.0);

}
