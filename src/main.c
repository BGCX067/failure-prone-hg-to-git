#include <stdio.h>

#include "math/vec3.h"

int main(){
    vec3 a = {1, 2, 4};
    vec3 b = {0, -9, 1};
    
    vec3 res;
    addvec(a, b, res);
    printf("res: %f, %f, %f\n", res[0], res[1], res[2]);
    float resDot = dot(a, b);
    printf("dot: %f\n", resDot);
    normalize(res);
    printf("normalized res: %f, %f, %f\n", res[0], res[1], res[2]);
    cross(a, b, res);
    printf("cross: %f, %f, %f\n", res[0], res[1], res[2]);
    return 0;
}
