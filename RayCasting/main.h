#ifndef __MAIN_H
#define __MAIN_H

#include "scene.h"
#include "objects.h"
#include "lights.h"

Node rootNode;
Camera camera;
RenderImage renderImage;
Sphere theSphere;
MaterialList materials;
LightList lights;
Plane thePlane;
ObjFileList objList;
TexturedColor background;
TexturedColor environment;
TextureList textureList;



#endif