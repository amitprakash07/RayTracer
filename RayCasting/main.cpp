#include "xmlload.h"
#include "viewport.h"
#include "main.h"
#include "Time/FrameTime.h"
#include "RayIntersection.h"
#include "RandomSampler.h"
#include <iostream>


//#define LOAD_FILE "XMLFile.xml"
extern Camera camera;
bool TraceRay(Node *i_node, Ray &ray, HitInfo &hInfo, int hitside = HIT_FRONT);

//Ray pixelRay;
int main(void)
{
	myEngine::Timing::Clock *clock = myEngine::Timing::Clock::createAndStart();
	int temp = LoadScene(LOAD_FILE);
	HitInfo hitInfo;
	Color noHitPixelColor = { 0,0,0 };
	Color24* temp_image = renderImage.GetPixels();
	float* temp_zBuffer = renderImage.GetZBuffer();
	uchar* temp_sampleBbuffer = renderImage.GetSampleCount();
	Sampler *sampler = new RandomSampler(MIN_SAMPLE_COUNT, MAX_SAMPLE_COUNT, MIN_VARIANCE, MAX_VARIANCE);
	SampleList tempSampleList;
	
	for (int i = 0;i < camera.imgHeight; i++)
	{
		for (int j = 0; j < camera.imgWidth; j++)
		{
			while (sampler->needMoreSamples())
			{
				sampler->generateSamples(j, i);
				for (int k = 0; k < sampler->getSampleBucketSize(); ++k)
				{
		 			hitInfo.Init();
					Ray sampleRay = sampler->getSampleRay(k);
					if (TraceRay(&rootNode, sampleRay, hitInfo))
					{
//#ifdef  RELEASE_DEBUG
//						temp_image[i*camera.imgWidth + j] = normalColor(hitInfo);
//#else
						sampler->setSampleColor(k,hitInfo.node->GetMaterial()->Shade(sampleRay, hitInfo, lights, 7));
						sampler->setIsSampleHit(k, true);
//#endif
					}
					else
					{
						sampler->setSampleColor(k,noHitPixelColor);
					}
					sampler->setHitInfo(k, hitInfo);
				}//for K
			}//while

			Color tempColor = sampler->getAveragedSampleListColor();
			float depth = sampler->getAveragedDepth();
			int sampleCount = sampler->getSampleBucketSize();
			//uchar aiwehi = 1;
#ifdef  _DEBUG || RELEASE_DEBUG
			std::cout << "Color of ("<<i<<","<<j<<") pixel = (" << tempColor.r << ", " << tempColor.g << ", " << tempColor.b << ")\n";
			//std::cout << "Checking uchar behavior " << aiwehi << std::endl;
			std::cout << "SampleCount for(" << i << "," << j << ") pixel = " << sampleCount << std::endl;
#endif
			temp_image[i*camera.imgWidth + j] = tempColor; /*hitInfo.node->GetMaterial()->Shade(pixelSamples[k].ray, hitInfo, lights, 7);*/
			temp_zBuffer[i*camera.imgWidth + j] = depth;
			temp_sampleBbuffer[i*camera.imgWidth + j] = sampleCount;
			sampler->resetSampler();
#ifdef RELEASE_DEBUG
			//std::cout << "SampleCount for this color is " << (int)randomSampling->getSampleBucketSize() << std::endl;
#endif
			
			depth = 0.0f;
		}//for for j
	}//for for i

	renderImage.SaveImage("RayCasted.ppm");
	renderImage.ComputeZBufferImage();
	renderImage.SaveZImage("RayCastWithZ.ppm");
	renderImage.ComputeSampleCountImage();
	renderImage.SaveSampleCountImage("SampleCountImage.ppm");
	
	clock->updateDeltaTime();
	double time = clock->getdeltaTime();
	printf("Time to render ray casting  %f", clock->getdeltaTime());
	printf("Time to render zBuffer Image  %f", clock->getdeltaTime());

	ShowViewport();
	
}
		












