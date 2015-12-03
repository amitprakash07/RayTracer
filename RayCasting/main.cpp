#include "xmlload.h"
#include "viewport.h"
//#include "main.h"
//#include "main.h"
#include "Time/FrameTime.h"
#include "Renderer.h"

//#define LOAD_FILE "CornellBox.xml"
//#define LOAD_FILE "Box.xml"
//#define LOAD_FILE "DepthOfField.xml"
#define LOAD_FILE "SoftShadowsGlossySurface.xml"
//#define LOAD_FILE "SoftShadowsGlossySurface_simple.xml"
//#define LOAD_FILE "white_teapot_GI.xml"
//#define LOAD_FILE "CornellBoxForGI.xml"
#define THREADCOUNT 8
#include <ctime>

extern RenderImage renderImage;
//#define LOAD_FILE "XMLFile.xml"
//bool TraceRay(Node *i_node, Ray &ray, HitInfo &hInfo, int hitside = HIT_FRONT);

//Ray pixelRay;
int main(void)
{
	srand(time(nullptr));	
	myEngine::Timing::Clock *clock = myEngine::Timing::Clock::createAndStart();
	int temp = LoadScene(LOAD_FILE);

	Renderer::startRendering(THREADCOUNT);
	renderImage.SaveImage("RayCasted.ppm");
	renderImage.ComputeZBufferImage();
	renderImage.SaveZImage("RayCastWithZ.ppm");
	renderImage.ComputeSampleCountImage();
	renderImage.SaveSampleCountImage("SampleCountImage.ppm");
	clock->updateDeltaTime();
	double time = clock->getdeltaTime();
	printf("Time to render ray casting  %f", clock->getdeltaTime());
	ShowViewport();
	
	
}
		












