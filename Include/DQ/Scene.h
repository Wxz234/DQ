#pragma once
namespace DQ
{
	class IScene
	{
	public:
		virtual void LoadModel(const char* filePath) = 0;
	};

	void InitScene(IScene** ppScene);
	void RemoveScene(IScene* pScene);
}