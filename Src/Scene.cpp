#include "DQ/Scene.h"
namespace DQ
{
	class Scene : public IScene
	{
	public:
		Scene()
		{

		}

		~Scene()
		{
			
		}

		bool HasData() const
		{
			return mHasData;
		}

		void LoadModel(const char* filePath)
		{
			mHasData = true;
		}

		std::shared_ptr<std::vector<MeshData>> pMeshData;
		bool mHasData = false;
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