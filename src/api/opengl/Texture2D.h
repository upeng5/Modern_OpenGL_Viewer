#pragma once

#include <string>

class Texture2D
{
public:
	Texture2D(const std::string& path);
	~Texture2D();

	void Bind(unsigned int slot = 0);
	void Unbind();

	unsigned int ID() const;
	int Width() const;
	int Height() const;
	int Channels() const;
private:
	unsigned int id_;
	int width_;
	int height_;
	int channels_;
};
