#include <string>
#include <filesystem>
#include <cstdlib>
enum class ShaderType
{
	Vertex,
	Pixel
};

void CompileShader(const char* path, const char* entry, ShaderType type)
{
	std::filesystem::path myPath{ path };
	auto myExtension = myPath.extension();
	if (myExtension.string() != ".hlsl") {
		return;
	}

	std::string dxcPath = std::string(PROJECT_PATH) + "/ThirdParty/DirectXShaderCompiler/bin/x64/dxc.exe";
	std::string shaderPath = std::string(PROJECT_PATH) + "/Shaders/" + std::string(path);
	std::string entryName = entry;
	std::string targerName;
	if (type == ShaderType::Vertex)
	{
		targerName = "vs_6_6";
	}
	else if (type == ShaderType::Pixel)
	{
		targerName = "ps_6_6";
	}

	auto myPathStem = myPath.stem();
	std::string headerName = myPathStem.string() + ".h";
	std::string headrPath = std::string(PROJECT_PATH) + "/Include/DQ/Shaders/" + headerName;
	std::string str = dxcPath + " " + shaderPath + " /E " + entryName + " /T " + targerName + " /Fh " + headrPath + " /Vn " + myPathStem.string();
	system(str.c_str());
}

int main()
{
	CompileShader("gbuffer_vs.hlsl", "main", ShaderType::Vertex);
	CompileShader("gbuffer_ps.hlsl", "main", ShaderType::Pixel);
}