#include "Renderer.h"
#include "RandomSampler.h"
#include "RayIntersection.h"
#include "MonteCarloGI.h"



#define MIN_SAMPLE_COUNT 8
#define MAX_SAMPLE_COUNT 16
#define MIN_VARIANCE 0.0001
#define MAX_VARIANCE 0.001
#define GI_SAMPLE 10
#define GI_BOUNCE_COUNT 1
#include <iostream>


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


size_t Renderer::threadCount = 0;
ThreadHandle Renderer::mThreadHandle;
Color24* Renderer::renderingImage = nullptr;
float* Renderer::zBufferImage = nullptr;
uchar* Renderer::sampleCountImage = nullptr;
Color24* Renderer::operationCountImage = nullptr;
size_t Renderer::noOfRowsToRenderPerThread;
int Renderer::imageWidth;
int Renderer::imageHeight;


void Renderer::startRendering(size_t i_threadCount)
{
	threadCount = i_threadCount;
	mThreadHandle = ThreadHandle(threadCount);
	imageWidth = renderImage.GetWidth();
	imageHeight = renderImage.GetHeight();
	noOfRowsToRenderPerThread = imageHeight / threadCount;
	//mPixelOffset = new Pixel[threadCount];
	renderingImage = renderImage.GetPixels();
	zBufferImage = renderImage.GetZBuffer();
	sampleCountImage = renderImage.GetSampleCount();
	operationCountImage = renderImage.GetOperationCountImage();

	for (int i = 0; i < renderImage.GetHeight(); ++i)
	{
		for (int j = 0; j < renderImage.GetWidth(); ++j)
		{
			calculatePixelColor(j, i);
		}
	}

	/*int *threadVal = new int[threadCount];
	for (size_t i = 0; i < threadCount; i++)
	{
		
		threadVal[i] = i;
		std::cout << "\nPassing Value to thread" << threadVal[i];
		mThreadHandle.thread[i] = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(renderPixel), &threadVal[i], CREATE_SUSPENDED, nullptr);
	}

	for (size_t i = 0; i < threadCount;i++)
	{
		ResumeThread(mThreadHandle.thread[i]);
	}

	std::cout << std::endl << WaitForMultipleObjects(threadCount + 1, mThreadHandle.thread, TRUE, INFINITE) << std::endl;;
	if (WaitForMultipleObjects(threadCount + 1, mThreadHandle.thread, TRUE, INFINITE))
	{
		
		mThreadHandle.destroyThread();
	}
*/
}

Renderer::Renderer()
{
}

DWORD Renderer::renderPixel(LPVOID threadData)
{
	/* Doing Z-fractal
	*****
		*
	   *
	  *
	 *
	*****
	*/
	int heightImageIndex = *reinterpret_cast<int*>(threadData) * noOfRowsToRenderPerThread;
	//std::cout <<"\nHeight Offset"<< heightImageIndex;
	//int widthOffset = 0;
	for (int j = 0; j < noOfRowsToRenderPerThread; j = j + 2)
	{
		for (int widthOffset = 0; widthOffset < imageWidth; widthOffset = widthOffset + 2)
		{
			calculatePixelColor(widthOffset, heightImageIndex);
			calculatePixelColor(widthOffset + 1, heightImageIndex);
			calculatePixelColor(widthOffset, heightImageIndex + 1);
			calculatePixelColor(widthOffset + 1, heightImageIndex + 1);
		}
		heightImageIndex += 2;
	}
	return 0;
}


void Renderer::calculatePixelColor(int offsetAlongWidth, int offsetAlongHeight)
{
	HitInfo hitInfo;
	Color noHitPixelColor = { 0,0,0 };
	Color finalColor = { 0,0,0 };
	RandomSampler sampler = RandomSampler(MIN_SAMPLE_COUNT, MAX_SAMPLE_COUNT, MIN_VARIANCE, MAX_VARIANCE);
	/*MonteCarloGI *mGI = new MonteCarloGI();*/
	while (sampler.needMoreSamples())
	{
		sampler.generateSamples(offsetAlongWidth, offsetAlongHeight);
		for (int k = 0; k < sampler.getSampleBucketSize(); ++k)
		{
			hitInfo.Init();
			Ray sampleRay = sampler.getSampleRay(k);
			if (TraceRay(&rootNode, sampleRay, hitInfo))
			{
				finalColor = hitInfo.node->GetMaterial()->Shade(sampleRay, hitInfo, lights, 7);
				//finalColor += mGI->indirectIlluminate(hitInfo, lights, GI_BOUNCE_COUNT, GI_SAMPLE);
				sampler.setSampleColor(k, finalColor);
				sampler.setIsSampleHit(k, true);
			}
			else
			{				
				sampler.setSampleColor(k, background.Sample(sampleRay.dir)/*noHitPixelColor*/);
			}
			sampler.setHitInfo(k, hitInfo);
		}
	}

	Color tempColor = sampler.getAveragedSampleListColor();
	float depth = sampler.getAveragedDepth();
	int sampleCount = sampler.getSampleBucketSize();
	int pixel = offsetAlongHeight * imageWidth + offsetAlongWidth;
	renderingImage[pixel] = tempColor; 
	operationCountImage[pixel] = Color(1.0f,0.0f,0.0f) * static_cast<float>(hitInfo.operationCount/BIGFLOAT);
	zBufferImage[pixel] = depth;
	sampleCountImage[pixel] = sampleCount;
	sampler.resetSampler();
	//delete mGI;
	//delete sampler;	
}