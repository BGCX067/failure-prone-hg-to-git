#version 330

const vec3 v3LightPos = vec3(0.0, 0.0, -1.0);
const float g = -0.999;
const float g2 = g*g;

in vec3 v3Direction;
in vec4 color;
in vec4 secondaryColor;

out vec4 fragColor;

void main (void)
{
	float fCos = dot(v3LightPos, v3Direction)/length(v3Direction);
	float fRayleighPhase = 0.75*(1.0 + fCos*fCos);
	float fMiePhase = 1.5*((1.0 - g2)/(2.0 + g2))*(1.0 + fCos*fCos)/pow(1.0 + g2 - 2.0*g*fCos, 1.5);
	fragColor = fRayleighPhase*color + fMiePhase*secondaryColor;
	fragColor.a = fragColor.b;
}

