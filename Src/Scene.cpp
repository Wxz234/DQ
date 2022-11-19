#include "DQ/Scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

		bool HasRenderableObject() const
		{
			return mHasRenderable;
		}

		void LoadModel(const char* filePath)
		{
			mHasRenderable = true;
		}

		std::shared_ptr<std::vector<MeshData>> pMeshData;
		bool mHasRenderable = false;
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