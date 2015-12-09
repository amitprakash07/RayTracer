#include "materials.h"
#include "RayIntersection.h"
#include "LightColor.h"
#include "SphereSampler.h"
#include "utils.h"
extern Node rootNode;
extern TexturedColor environment;

Point3 getRefractionVector(Point3 view, Point3 normal, float n1 = 1, float n2 = 1);
Point3 getReflectionVector(Point3 view, Point3 normal);
Point3 getPerturbedNormal(Point3 normal, Point3 hitPoint, float i_glossiness);

Color MtlBlinn::Shade(const Ray &ray, const HitInfo &hInfo, const LightList &lights, int bounceCount) const
{
	Color ambientComp, diffuseComp, specularComp, reflectiveComp, refractiveComp, reflectionTotal, refractionTotal, noColor;
	ambientComp = diffuseComp = specularComp = reflectiveComp = refractiveComp  = noColor = reflectionTotal = refractionTotal = Color(0.0f, 0.0f, 0.0f);
	Color fromReflection = Color(0.0f, 0.0f, 0.0f);
	Color fromRefraction = Color(0.0f, 0.0f, 0.0f);
	Point3 viewDirection = -ray.dir;
	float schlicksConstant, ratioOfRefraction;
	int hitside = HIT_FRONT;
	float n1 = 1;
	float n2 = ior;

	for (int i = 0; i < lights.size(); i++)
	{
		if (lights[i]->IsAmbient())
		{
			ambientComp += ambientComponent(lights[i], hInfo, diffuse.Sample(hInfo.uvw));
		}
		else
		{
			diffuseComp += diffuseComponent(lights[i], hInfo, diffuse.Sample(hInfo.uvw));
			specularComp += specularComponent(lights[i], viewDirection, hInfo, specular.Sample(hInfo.uvw), glossiness);

		}
	}
			/************************Refraction************************************************************/
			if(refraction.Sample(hInfo.uvw) != noColor && bounceCount > 0)
			{
				Ray refractionRay;
				HitInfo refractionRayHit;
				refractionRayHit.Init();
				refractionRay.p = hInfo.p;
				if (hInfo.front == HIT_FRONT)
				{
					refractionRay.dir = getRefractionVector(viewDirection, getPerturbedNormal(hInfo.N, hInfo.p, refractionGlossiness), n1, n2);
					//hitside = HIT_FRONT;
				}
				else
				{
					refractionRay.dir = getRefractionVector(viewDirection, getPerturbedNormal( -hInfo.N, hInfo.p, refractionGlossiness), n2, n1);
					//hitside = HIT_BACK;
				}
				
				if(TraceRay(&rootNode, refractionRay, refractionRayHit, hitside))
				{
					refractiveComp = refractionRayHit.node->GetMaterial()->Shade(refractionRay, refractionRayHit, lights, --bounceCount);
				}
				else
				{
					refractiveComp = environment.SampleEnvironment(refractionRay.dir);

				}				
			}



			/********************Schlick's Approximation - Fresnel Reflection***************************/
			/*schlicksConstant = pow(((n1 - n2) / (n1 + n2)), 2);
			ratioOfRefraction = schlicksConstant + (1 - schlicksConstant) * pow((1 - viewDirection.Dot(hInfo.N)), 5);
			reflectionTotal = ratioOfRefraction*refraction.Sample(hInfo.uvw);
			refractionTotal = (1 - ratioOfRefraction)*refraction.Sample(hInfo.uvw);*/
			///*******************************************************************************************/
			//refractiv    eComp *= refractionTotal; //It = (1-R) * KT'
			reflectionTotal += reflection.Sample(hInfo.uvw); //Doing outside in case refraction didn't occured at all

			/*********************************************************************************************/

			/**********************Reflection*************************************************************/
			if(reflectionTotal != noColor && bounceCount > 0)
			{
				Ray reflectionViewVector;
				reflectionViewVector.dir = getReflectionVector(viewDirection, getPerturbedNormal(hInfo.N, hInfo.p, reflectionGlossiness));
				reflectionViewVector.p = hInfo.p;
				HitInfo reflectionRayHit;
				reflectionRayHit.Init();
				if (TraceRay(&rootNode, reflectionViewVector, reflectionRayHit, HIT_FRONT))
				{
					bounceCount--;
					fromReflection = reflectionRayHit.node->GetMaterial()->Shade(reflectionViewVector, reflectionRayHit, lights, --bounceCount);
					reflectiveComp = reflectionTotal * fromReflection;
				}
				else
				{
					reflectiveComp = environment.SampleEnvironment(reflectionViewVector.dir);
				}
			}
			/****************************************************************************************************/
		

	

	return (ambientComp + diffuseComp + specularComp+ reflectiveComp + refractiveComp);
}


Point3 getRefractionVector(Point3 view, Point3 normal, float n1, float n2)
{
	Point3 Vnormal = normal;
	float cosThetaOne = view.Dot(normal)/view.Length();
	float sinThetaTwo = (n1 / n2) * sqrt(1 - pow(cosThetaOne, 2));
	float cosThetaTwo = sqrt(1 - pow(sinThetaTwo, 2));
	Point3 Vt = (view - (view.Dot(normal)*normal)).GetNormalized();
	
	/******************Total Internal Reflection **************************/
	//if (sinThetaTwo > 1 || sinThetaTwo < -1) //Checking total interanl reflection
	//	return(getReflectionVector(-view, normal)); //If happened returning a reflection vector
	/*************************End Total Internal Reflection***************/
	return ((cosThetaTwo * -1 * Vnormal) + (sinThetaTwo * -1 * Vt ));
}


Point3 getReflectionVector(Point3 view, Point3 normal)
{
	return((2 * normal * (view.Dot(normal))) - view); //Using Reflection Vector for formulae
}


Point3 getPerturbedNormal(Point3 normal, Point3 hitPoint, float i_glossiness)
{
	if (i_glossiness > 0.0f)
	{
		float radius = tan(i_glossiness*(M_PI / 180));
		SphereSampler sphereSampler = SphereSampler(1, 2, radius);
		sphereSampler.generateSamples();
		Point3 randomOffset = sphereSampler.getSample(getRandomNumber(0,2)).getOffset();
		return (normal + randomOffset).GetNormalized();
	}
	return normal;
}


