#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	Model(const std::string& path, bool gamma = false) : gamma_correction(gamma)
	{
		LoadModel(path);
	}
	void Draw(Shader& shader);
private:
	// model data
	bool gamma_correction;
	std::vector<Texture> textures_loaded_;
	std::vector<Mesh> meshes_;
	std::string directory_;

	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	unsigned int TextureFromFile(const char* path, const std::string& directory);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string type_name);
};
