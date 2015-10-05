#include "xmlload.h"
#include "viewport.h"
#include "main.h"
#include "Time/FrameTime.h"
#include "RayIntersection.h"
#include "LightColor.h"

#define LOAD_FILE "../Assets/CornellBoxScene.xml"

extern Camera camera;
bool TraceRay(Node *i_node, Ray &ray, HitInfo &hInfo, int hitside = HIT_FRONT);

#ifdef WIN32
#define COMPARE(a,b) (_stricmp(a,b)==0)
#else
#define COMPARE(a,b) (strcasecmp(a,b)==0)
#endif

//#define __ZBUFFER

Ray pixelRay;
int main(void)
{
	myEngine::Timing::Clock *clock = myEngine::Timing::Clock::createAndStart();
	int temp = LoadScene(LOAD_FILE);
	Point3 cameraRight = (camera.dir ^ camera.up).GetNormalized();
	double aspectRatio = static_cast<double>(camera.imgWidth) / static_cast<double>(camera.imgHeight);
	float camera_l = 1 / (tan((camera.fov / 2) * (M_PI/ 180)));
	
	Point3 Sx = cameraRight;
	Point3 Sy = (-1.0f) * camera.up;
	
	Point3 pixel;
	Point3 k = camera.pos + camera_l* camera.dir;// -cameraRight + camera.up;

	HitInfo hitInfo;
	

	//Color24 hitPixelColor = { 255,255,255 };
	Color noHitPixelColor = { 0,0,0 };
	Color24* temp_image = renderImage.GetPixels();

	float* temp_zBuffer = renderImage.GetZBuffer();

	
	for (int i = 0;i < camera.imgHeight; i++)
	{
		//printf("%d\n",i);
		for (int j = 0; j < camera.imgWidth; j++)
		{
			hitInfo.Init();
			float flipped_i = camera.imgHeight - i - 1;
			pixel = k + (((2.0f*aspectRatio * (j + 0.5f)) / camera.imgWidth) - aspectRatio)*Sx + ((((flipped_i + 0.5) * 2) / camera.imgHeight) - 1)* Sy;
			pixelRay.p = camera.pos;
			pixelRay.dir = (pixel - camera.pos).GetNormalized();
			
			if (TraceRay(&rootNode, pixelRay, hitInfo))
			{
#ifdef  RELEASE_DEBUG
				temp_image[i*camera.imgWidth + j] = normalColor(hitInfo);
#else
				temp_image[i*camera.imgWidth + j] = hitInfo.node->GetMaterial()->Shade(pixelRay, hitInfo, lights, 7);
#endif
				temp_zBuffer[i*camera.imgWidth + j] = hitInfo.z;
			}
			else
			{

				temp_image[i*camera.imgWidth + j] = noHitPixelColor;
				temp_zBuffer[i*camera.imgWidth + j] = hitInfo.z;
			}
		}
	}

	renderImage.SaveImage("RayCasted.ppm");
	renderImage.ComputeZBufferImage();
	renderImage.SaveZImage("RayCastWithZ.ppm");
	
	clock->updateDeltaTime();
	double time = clock->getdeltaTime();
	printf("Time to render ray casting  %f", clock->getdeltaTime());
	printf("Time to render zBuffer Image  %f", clock->getdeltaTime());

	ShowViewport();
	
}













