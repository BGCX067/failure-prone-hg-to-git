#version 330

layout(location = 0) in vec3 inpos;

//eyepos
//uniform vec3 eyepos;		// The camera's current position

const vec3 v3LightPos = vec3(0.0, 1.0, 0.0);		// The direction vector to the light source

const vec3 v3InvWavelength = vec3(1.0/pow(0.950, 4.0), 1.0/pow(0.270, 4.0), 1.0/pow(0.475, 4.0));

const float fInnerRadius = 100;		// The inner (planetary) radius
const float fOuterRadius = fInnerRadius + fInnerRadius*1.25;		// The outer (atmosphere) radius

vec3 atmosOffset = vec3(0.0, fInnerRadius, 0.0);

const float kr = 0.0025;
const float km = 0.0010;
const float sun = 100.0;

const float fKrESun = kr*sun;			// Kr * ESun
const float fKmESun = km*sun;			// Km * ESun
const float fKr4PI = kr*4.0*3.1415;			// Kr * 4 * PI
const float fKm4PI = km*4.0*3.1415;			// Km * 4 * PI

const float fScaleDepth = 0.25;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)

const int nSamples = 10;
const float fSamples = 10;

uniform mat4 modelview;
uniform mat4 mvp;

out vec3 v3Direction;
out vec4 color;
out vec4 secondaryColor;

float scale(float fCos)
{
	float x = 1.0 - fCos;
    //WTF?
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main(void)
{
    vec3 v3CameraPos = atmosOffset;
    float fCameraHeight = v3CameraPos.y;//length(v3CameraPos);

    float fScale = 1.0/(fOuterRadius - fInnerRadius);
    float fScaleOverScaleDepth = fScale/fScaleDepth;
	// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 v3Pos = inpos.xyz;
	vec3 v3Ray = v3Pos - v3CameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = v3CameraPos;
	float fHeight = length(v3Start);
	float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
	float fStartAngle = dot(v3Ray, v3Start) / fHeight;
	float fStartOffset = fDepth*scale(fStartAngle);

	// Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	float fSampleLength = fFar/fSamples;
	float fScaledLength = fSampleLength*fScale;
	vec3 v3SampleRay = v3Ray*fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	for(int i=0; i<nSamples; i++)
	{   
        //Altura não deveria ser v3SamplePoint.y?
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fLightAngle = dot(v3LightPos, v3SamplePoint)/fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth*(scale(fLightAngle) - scale(fCameraAngle)));
		vec3 v3Attenuate = exp(-fScatter*(v3InvWavelength*fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate*(fDepth*fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
    secondaryColor.rgb  =  v3FrontColor*fKmESun;
    color.rgb = v3FrontColor*(v3InvWavelength*fKrESun);

	//gl_FrontSecondaryColor.rgb = v3FrontColor * fKmESun;
	//gl_FrontColor.rgb = v3FrontColor*(v3InvWavelength*fKrESun);
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
	v3Direction = v3CameraPos - v3Pos;

	gl_Position = mvp*vec4(inpos, 1.0);
}
