#include "Renderer.h"
#include "RandomSampler.h"
#include "RayIntersection.h"
#include "MonteCarloGI.h"
#include "Time/FrameTime.h"
#include <tchar.h>


#define MIN_SAMPLE_COUNT 8
#define MAX_SAMPLE_COUNT 16
#define MIN_VARIANCE 0.0001
#define MAX_VARIANCE 0.001
#define GI_SAMPLE 10
#define GI_BOUNCE_COUNT 1
#include <iostream>
#define THREADCOUNT 10

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

DWORD WINAPI startRenderer(LPVOID i_data)
{
	Renderer::startRendering(THREADCOUNT);
	return 0;
}

void BeginRender()
{
	/*HANDLE WINAPI CreateThread(
		_In_opt_  LPSECURITY_ATTRIBUTES  lpThreadAttributes,
		_In_      SIZE_T                 dwStackSize,
		_In_      LPTHREAD_START_ROUTINE lpStartAddress,
		_In_opt_  LPVOID                 lpParameter,
		_In_      DWORD                  dwCreationFlags,
		_Out_opt_ LPDWORD                lpThreadId
		);*/

	HANDLE mainThread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(startRenderer), nullptr, 0, nullptr);
	//WaitForSingleObject(mainThread, INFINITE);
	std::cout << "Hold On";
}

void Renderer::startRendering(size_t i_threadCount)
{
	threadCount = i_threadCount;
	mThreadHandle = ThreadHandle(threadCount);
	imageWidth = renderImage.GetWidth();
	imageHeight = renderImage.GetHeight();
	noOfRowsToRenderPerThread = imageHeight / threadCount;
	renderingImage = renderImage.GetPixels();
	zBufferImage = renderImage.GetZBuffer();
	sampleCountImage = renderImage.GetSampleCount();
	operationCountImage = renderImage.GetOperationCountImage();

	myEngine::Timing::Clock *clock = myEngine::Timing::Clock::createAndStart();

	/*for (int i = 0; i < renderImage.GetHeight(); ++i)
	{
		for (int j = 0; j < renderImage.GetWidth(); ++j)
		{
			calculatePixelColor(rootNode,lights,j, i);
		}
	}*/

	

	int *threadVal = new int[threadCount];
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
	
	std::cout << "Wait Val"<<std::endl << WaitForMultipleObjects(threadCount, mThreadHandle.thread, TRUE, INFINITE) << std::endl;;
	std::cout << "Wait finished";
	
	if (WaitForMultipleObjects(threadCount + 1, mThreadHandle.thread, TRUE, INFINITE))
	{
		mThreadHandle.destroyThread();
	}

	TCHAR* mutexName = __T("WritingMutex");
	HANDLE mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
	CloseHandle(mutexHandle);


	renderImage.SaveImage("RayCasted.ppm");
	renderImage.ComputeZBufferImage();
	renderImage.SaveZImage("RayCastWithZ.ppm");
	renderImage.ComputeSampleCountImage();
	renderImage.SaveSampleCountImage("SampleCountImage.ppm");
	clock->updateDeltaTime();
	double time = clock->getdeltaTime();
	printf("Time to render ray casting  %f", clock->getdeltaTime());
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
	/*Node threadScopeRootNodeCopy = rootNode;
	LightList threadScopeLightLIst = lights;*/
	for (int j = 0; j < noOfRowsToRenderPerThread; j = j + 2)
	{
		for (int widthOffset = 0; widthOffset < imageWidth; widthOffset = widthOffset + 2)
		{
			calculatePixelColor(/*threadScopeRootNodeCopy*/ rootNode, /*threadScopeLightLIst*/lights, widthOffset, heightImageIndex);
			calculatePixelColor(/*threadScopeRootNodeCopy*/rootNode, /*threadScopeLightLIst*/ lights, widthOffset + 1, heightImageIndex);
			calculatePixelColor(/*threadScopeRootNodeCopy*/rootNode, /*threadScopeLightLIst*/lights, widthOffset, heightImageIndex + 1);
			calculatePixelColor(/*threadScopeRootNodeCopy*/rootNode, /*threadScopeLightLIst*/lights, widthOffset + 1, heightImageIndex + 1);
		}
		heightImageIndex += 2;
	}
	return 0;
}


void Renderer::calculatePixelColor(Node &i_rootNode, LightList &i_lightList, int offsetAlongWidth, int offsetAlongHeight)
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
			if (TraceRay(&i_rootNode, sampleRay, hitInfo))
			{
				finalColor = hitInfo.node->GetMaterial()->Shade(sampleRay, hitInfo, i_lightList, 7);
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

	TCHAR* mutexName = __T("WritingMutex");
	HANDLE mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);
	WaitForSingleObject(mutexHandle, INFINITE);
	renderingImage[pixel] = tempColor; 
	operationCountImage[pixel] = Color(1.0f,0.0f,0.0f) * static_cast<float>(hitInfo.operationCount/BIGFLOAT);
	zBufferImage[pixel] = depth;
	sampleCountImage[pixel] = sampleCount;
	ReleaseMutex(mutexHandle);
	sampler.resetSampler();
	//delete mGI;
	//delete sampler;	
}