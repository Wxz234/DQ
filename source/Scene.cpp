#include "DQ/Scene.h"

namespace DQ
{
	class Scene::Impl
	{
	public:

	};

	Scene::SharedPtr Scene::create(DQ::Device::SharedPtr& pDevice)
	{
		return SharedPtr(new Scene(pDevice));
	}

	Scene::Scene(DQ::Device::SharedPtr& pDevice)
	{
		mpImpl = new Impl;
		mpDevice = pDevice;
	}

	Scene::~Scene()
	{
		delete mpImpl;
	}

	void Scene::loadModel(std::string_view modelPath) 
	{
		Mesh mesh;

	}

	void Scene::_preBake()
	{

	}
}