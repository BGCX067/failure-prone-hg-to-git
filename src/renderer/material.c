#include "material.h"
#include "renderer.h"

void bindMaterial(Material* m, Light* l ){

	if (!m || !l)
		return;

	
	setShaderConstant4f(m->shdr, "Kd", m->kd );
	setShaderConstant4f(m->shdr, "Ka", m->ka );
	setShaderConstant4f(m->shdr, "Ks", m->ks );
	setShaderConstant1f(m->shdr, "shininess", m->shininess);

	setShaderConstant3f(m->shdr, "LightPosition", l->pos);
	setShaderConstant4f(m->shdr, "LightColor", l->color);

	float ambient[4];
	ambient[0] = 0.1;
	ambient[1] = 0.1;
	ambient[2] = 0.1;
	ambient[3] = 0.1;
	setShaderConstant4f(m->shdr, "globalAmbient", ambient);

}
