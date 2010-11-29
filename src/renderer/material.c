#include "material.h"
#include "renderer.h"

void bindMaterial(material* m, light* l ){

	if (!m || !l)
		return 0;

	
	setShaderConstant4f(m->shaderid, "Kd", m->kd );
	setShaderConstant4f(m->shaderid, "Ka", m->ka );
	setShaderConstant4f(m->shaderid, "Ks", m->ks );
	setShaderConstant1f(m->shaderid, "shininess", m->shininess);

	setShaderConstant3f(m->shaderid, "LightPosition", l->pos);
	setShaderConstant4f(m->shaderid, "LightColor", l->color);

	float ambient[4];
	ambient[0] = 0.1;
	ambient[1] = 0.1;
	ambient[2] = 0.1;
	ambient[3] = 0.1;
	setShaderConstant4f(m->shaderid, "globalAmbient", ambient);

}
