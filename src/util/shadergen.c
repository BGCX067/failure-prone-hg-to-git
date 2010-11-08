#include "shadergen.h"
#include <string.h>
#include <stdio.h>

char* createVSGlobals(material m) {
    char* header = "#version 330 core\n\n";
    char* ret;
    char* phong = ""; 
    char* normalmap = "";
    char* envmap = "";
    char* refract = "";
    
    char* minimal = "uniform mat4 mvp;\n"
                    "uniform mat4 modelview;\n"
                    "layout(location = 0) in vec3 vertPos;\n"
                    "layout(location = 2) in vec3 vertNormal;\n\n";
    if(m.flags & PHONG) {
        //Posição e normal que devem ser interpoladas
        phong = "out vec3 position;\n"
                "out vec3 normal;\n"
                "uniform mat4 normalmatrix;\n";
    }

    if(m.flags & NORMAL_MAP) {
        normalmap = "attribute vec3 Tangent;\n"
                    "attribute vec3 Binormal;\n"
                    "uniform vec3 LightPosition;\n"
                    "varying vec3 lightDir;\n"
                    "varying vec3 position;\n";
    }
    
    if(m.flags & ENV_MAP) {
        envmap = "varying vec3 reflectDir;\n";
    }
    
    if(m.flags & REFRACT) {
        refract = "varying vec3 refractDir;\n"
                 "uniform float etaRatio;\n";
    }
    size_t retlen = strlen(header) + strlen(minimal) + strlen(phong) + strlen(normalmap) 
                    + strlen(envmap) + strlen(refract) + 1;
    ret = malloc(sizeof(char)*retlen);
    sprintf(ret, "%s%s%s%s%s%s", header, minimal, phong, normalmap, envmap, refract);

    return ret;
}

char* createVSFuncs(material m) {
    char *ret = "";
    return ret;
}

char* createVSMain(material m) {
    char* ret;
    char* phong = ""; 
    char* normalmap = "";
    char* tex = "";
    char* envmap = "";
    char* refract = "";
    char* beginmain = "void main() {\n";
    char* endmain = "}\n";

    char* minimal = "\tgl_Position = mvp*vec4(vertPos, 1.0);\n";
    
    //FIXME passar normal matrix
    if(m.flags & PHONG) {
        phong = "\tposition = (modelview*vec4(vertPos, 1.0)).xyz;\n"
                "\tnormal = normalize((normalmatrix*vec4(vertNormal, 1.0)).xyz);\n";
    }

    if(m.flags & NORMAL_MAP) {
        normalmap = "\tgl_Position = ftransform();\n"
                    //TODO: pegar a posição em coordenadas do mundo
                    "\tposition = gl_Vertex.xyz;\n"
                    "\tvec3 lightDirection = LightPosition - position.xyz;\n"
                    "\tvec3 normal = gl_NormalMatrix*gl_Normal;\n"
                    "\tvec3 tangent = gl_NormalMatrix*Tangent;\n"
                    "\tvec3 binormal = cross(normal, tangent);\n"
                    "\tlightDir.x = dot(lightDirection, tangent);\n"
                    "\tlightDir.y = dot(lightDirection, binormal);\n"
                    "\tlightDir.z = dot(lightDirection, normal);\n";
    }

    if(m.flags & TEX) {
        tex = "\tgl_TexCoord[0] = gl_MultiTexCoord0;\n";
    }

    if(m.flags & ENV_MAP) {
        envmap = "\tvec3 eyeDir = (gl_ModelViewMatrix*gl_Vertex).xyz;\n"
                 "\treflectDir = reflect(eyeDir, normal);\n";
    }
    
    if(m.flags & REFRACT) {
        refract = "\tvec3 eyeDir = (gl_ModelViewMatrix*gl_Vertex).xyz;\n"
                  "\trefractDir = refract(eyeDir, normal, etaRatio);\n";
    }
    size_t retlen = strlen(beginmain) + strlen(minimal) + strlen(phong) + 
                    strlen(normalmap) + strlen(tex) + strlen(envmap) + 
                    strlen(refract) + strlen(endmain) + 1;
    ret = malloc(sizeof(char)*retlen);
    sprintf(ret, "%s%s%s%s%s%s%s%s", beginmain, minimal, phong, normalmap, tex, envmap, 
                                   refract, endmain);

    return ret;
}

char* createFSGlobal(material m) {
    char* ret;
    char* phong = "";
    char* attenuation = "";
    char* spotlight = "";
    char* normalmap = "";
    char* tex = "";
    char* envmap = "";
    char* refract = "";

    char* header = "#version 330 core\n\n";

    char* fragColor = "out vec4 fragColor;\n"
                      "uniform mat4 mvp;\n";

    if(m.flags & PHONG) {
        phong = "in vec3 normal;\n"
                "in vec3 position;\n"
                "uniform vec3 LightPosition;\n"
                "uniform vec3 EyePosition;\n"
                "uniform vec4 Ka;\n"
                "uniform vec4 Kd;\n"
                "uniform vec4 Ks;\n"
                "uniform float shininess;\n"
                "uniform vec4 globalAmbient;\n"
                "uniform vec4 LightColor;\n";
    }

    if(m.flags & ATTENUATION) {
        attenuation = "uniform float Kc;\n"
                      "uniform float Kl;\n"
                      "uniform float Kq;\n";
    }

    if(m.flags & SPOTLIGHT) {
        spotlight = "uniform float cosOuterCone;\n"
                    "uniform float cosInnerCone;\n"
                    "uniform vec3 coneDir;\n";
    }

    if(m.flags & TEX) {
        tex = "uniform sampler2D texture;\n";
    }

    if(m.flags & NORMAL_MAP) {
        normalmap = "varying vec3 lightDir;\n"
                    "varying vec3 position;\n"
                    "uniform vec3 EyePosition;\n"
                    "uniform vec4 Ka;\n"
                    "uniform vec4 Kd;\n"
                    "uniform vec4 Ks;\n"
                    "uniform float shininess;\n"
                    "uniform vec4 globalAmbient;\n"
                    "uniform vec4 LightColor;\n"
                    "uniform sampler2D normalMap;\n";
    }

    if(m.flags & ENV_MAP) {
        envmap = "uniform samplerCube envmap;\n"
                 "varying vec3 reflectDir;\n";
    }

    if(m.flags & REFRACT) {
        refract = "uniform samplerCube envmap;\n"
                  "varying vec3 refractDir;\n";
    }
    
    size_t retlen = strlen(header) + strlen(fragColor) + strlen(phong) + strlen(attenuation)
                    + strlen(spotlight) + strlen(normalmap) + strlen(tex) + strlen(envmap) 
                    + strlen(refract) + 1;
    ret = malloc(sizeof(char)*retlen);
    sprintf(ret, "%s%s%s%s%s%s%s%s%s", header, fragColor, phong, attenuation, spotlight, 
                                     normalmap, tex, envmap, refract);
    
    return ret;
}

char* createFSFuncs(material m) {
    char* ret = "";
    char* att = "";
    char* phong = "";
    char* spotlight = "";
    
    //cria a função de iluminação que recebe a normal
    //e a lightdir e retorna a cor
    if(m.flags & ATTENUATION) {
        att = "float attenuation(vec3 pos, vec3 lpos) {\n"
              "\tfloat d = distance(pos, lpos);\n"
              "\treturn 1.0/(Kc + Kl*d + Kq*d*d);\n"
              "}\n";
    }

    if(m.flags & SPOTLIGHT) {
        spotlight = "float spotlight(vec3 p, vec3 lightpos) {\n"
                    "\tvec3 v = normalize(p - lightpos);\n"
                    "\tvec4 lightDir = vec4(coneDir, 0.0);\n"
                    "\tlightDir = gl_ModelViewMatrix*lightDir;\n"
                    "\tlightDir = normalize(lightDir);\n"
                    "\tfloat cosDir = dot(v, lightDir.xyz);\n"
                    "\tfloat res = smoothstep(cosOuterCone, cosInnerCone, cosDir);\n"
                    "\treturn res;\n"
                    "}\n";

    }

    if(m.flags & PHONG) {
        char* beginphong = "vec4 phong(vec3 n, vec3 lightDir) {\n"
                "\tvec3 viewVec = normalize(EyePosition - position);\n"
                "\tvec3 halfVec = normalize(lightDir + viewVec);\n"
                "\tfloat diffCoef = max(dot(n, lightDir), 0.0);\n"
                "\tfloat specCoef = pow(max(dot(n, halfVec), 0.0), shininess);\n"
                "\tif (diffCoef <= 0.0)\n"
                "\t\tspecCoef = 0.0;\n"
                "\tvec4 ambient = globalAmbient*Ka;\n"
                "\tvec4 diffuse = Kd*LightColor*diffCoef;\n"
                "\tvec4 specular = Ks*LightColor*specCoef;\n";
        char* color = "";
        char* spotmult = "";
        char* endphong = "\tcolor.w = 1.0;\n"
                         "\treturn color;\n"
                         "}\n";
        if(m.flags & ATTENUATION) {
            color = "\tvec4 color = ambient + (attenuation(position, LightPosition)*(diffuse + specular));\n";
        } else {
            color = "\tvec4 color = (ambient + diffuse) + specular;\n";
        }
        if(m.flags & SPOTLIGHT) {
            spotmult = "\tvec4 lightPos = vec4(LightPosition, 1.0);\n"
                       "\tlightPos = gl_ModelViewMatrix*lightPos;\n"
                       "\tcolor = color*spotlight(position, lightPos.xyz);\n";
        }

        size_t phonglen = strlen(beginphong) + strlen(color) + strlen(spotmult) + strlen(endphong) + 1;
        phong = malloc(sizeof(char)*phonglen);
        sprintf(phong, "%s%s%s%s", beginphong, color, spotmult, endphong);
    }

    size_t retlen = strlen(phong) + strlen(att) + strlen(spotlight) + 1;
    ret = malloc(sizeof(char)*retlen);
    sprintf(ret, "%s%s%s", att, spotlight, phong);

    return ret;
}

char* createFSMainBody(material m) {
    char* ret;
    char* phong = ""; 
    char* tex = "";
    char* normalmap = "";
    char* envmap = "";
    char* refract = "";
    char* beginmain = "void main() {\n";
    
    if(m.flags & PHONG) {
        phong = "\tvec3 N = normalize(normal);\n"
                "\tvec4 lightPos = vec4(LightPosition, 1.0);\n"
                "\tlightPos = mvp*lightPos;\n"
                "\tvec3 lightVec = normalize(lightPos.xyz - position);\n"
                "\tvec4 phongColor = phong(N, lightVec);\n";
    }

    if(m.flags & TEX) {
        tex = "\tvec4 texColor = texture2D(texture, gl_TexCoord[0].xy);\n";
    }

    if(m.flags & NORMAL_MAP) {
        normalmap = "\tvec3 lightVec = normalize(lightDir);\n"
                    "\tvec3 N = normalize((texture2D(normalMap, gl_TexCoord[0].xy).xyz*2.0) - 1.0);\n";
                    "\tvec4 phongColor = phong(N, lightVec);\n";
    }

    if(m.flags & ENV_MAP) {
        envmap = "\tvec4 envcolor = textureCube(envmap, reflectDir);\n";
    }
    
    if(m.flags & REFRACT) {
        envmap = "\tvec4 envcolor = textureCube(envmap, refractDir);\n";
    }
    
    
    size_t retlen = strlen(beginmain) + strlen(phong) + strlen(normalmap) 
                    + strlen(tex) + strlen(envmap) + strlen(refract) + 1;
    ret = malloc(sizeof(char)*retlen);
    sprintf(ret, "%s%s%s%s%s%s", beginmain, phong, normalmap, tex, envmap, refract);

    return ret;
}

char* createFSMainFragColor(material m) {
    char* ret;
    char* phong = ""; 
    char* tex = "";
    char* texphong = "";
    char* envmap = "";
    char* refract = "";
   
    if((m.flags & PHONG) && (m.flags & ENV_MAP) && (m.flags & TEX)) {
        envmap = "\tfragColor =  mix( (texColor*phongColor), envcolor, 0.3);\n"
                 "}\n";
    } else if((m.flags & PHONG) && (m.flags & REFRACT) && (m.flags & TEX)) {
        refract = "\tfragColor = mix(texColor*phongColor, envcolor, 0.3);\n"
                 "}\n";
    } else if(((m.flags & PHONG)|| (m.flags & NORMAL_MAP)) && (m.flags &TEX)) {
        texphong = "\tfragColor = phongColor*texColor;\n"
                   "}\n";
    } else if((m.flags & PHONG) || (m.flags & NORMAL_MAP)) {
        phong = "\tfragColor = phongColor;\n"
                "}\n";
    } else if(m.flags & TEX) {
        tex = "\tfragColor = texColor;\n"
              "}\n";
    } else if (m.flags & ENV_MAP) {
        envmap = "\tfragColor = vec4(envcolor, 1.0);\n"
                 "}\n";
    } 

    size_t retlen = strlen(texphong) + strlen(phong) + strlen(tex) 
                    + strlen(envmap) + strlen(refract) + 1;
    ret = malloc(sizeof(char)*retlen);
    sprintf(ret, "%s%s%s%s%s", texphong, phong, tex, envmap, refract);
    
    return ret;
}

char* createFSMain(material m) {
    char* ret;
    char* main = "";
    char* fragcolor = "";

    main = createFSMainBody(m);
    fragcolor = createFSMainFragColor(m); 
    
    size_t retlen = strlen(main) + strlen(fragcolor) + 1;
    ret = malloc(sizeof(char)*retlen);
    sprintf(ret, "%s%s", main, fragcolor);

    return ret;
}

//TODO passar ponteiro pra ponteiro é uma solução ruim
void shadergen(material m, char** vertShader, char** fragShader) {
    //create vert shader
    char* vsglobal = createVSGlobals(m);
    char* vsfunc = createVSFuncs(m);
    char* vsmain = createVSMain(m);
    int vslength = strlen(vsglobal) + strlen(vsfunc) + strlen(vsmain);
    *vertShader = (char*) malloc(sizeof(char)*vslength);
    sprintf(*vertShader, "%s%s%s", vsglobal, vsfunc, vsmain);
    
    //create frag shader
    char* fsglobal = createFSGlobal(m);
    char* fsfunc = createFSFuncs(m);
    char* fsmain = createFSMain(m);
    int fslength = strlen(fsglobal) + strlen(fsfunc) + strlen(fsmain);
    *fragShader = (char*) malloc(sizeof(char)*fslength);
    sprintf(*fragShader, "%s%s%s", fsglobal, fsfunc, fsmain);
}
