#include "DQ/Scene.h"
namespace DQ
{
	class Scene : public IScene
	{
	public:
		~Scene()
		{

		}

		void LoadModel(const char* filePath)
		{

		}

		void Update()
		{

		}
	};

	void InitScene(IScene** ppScene)
	{
		if (ppScene)
		{
			*ppScene = new Scene;
		}
	}

	void RemoveScene(IScene* pScene)
	{
		auto ptr = dynamic_cast<Scene*>(pScene);
		delete ptr;
	}
}