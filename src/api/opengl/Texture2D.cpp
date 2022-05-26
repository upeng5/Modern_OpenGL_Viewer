#include "Texture2D.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>

Texture2D::Texture2D(const std::string& path)
	: width_(0), height_(0), channels_(0)
{
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = stbi_load(path.c_str(), &width_, &height_, &channels_, 0);

	int format = GL_RGB;
	if (channels_ == 4) {
		stbi_set_flip_vertically_on_load(true);
		format = GL_RGBA;
	}
	else if (channels_ != 3) {
		std::cout << "ERROR::TEXTURE::INVALID_FORMAT\n" << "Only 3 and 4 channel formats are allowed." << std::endl;
	}

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR::TEXTURE::FAILED_TO_LOAD_IMAGE" << std::endl;
	}

	stbi_image_free(data);
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &id_);
}

void Texture2D::Bind(unsigned int slot)
{
	glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture2D::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture2D::ID() const
{
	return id_;
}

int Texture2D::Width() const
{
	return width_;
}

int Texture2D::Height() const
{
	return height_;
}

int Texture2D::Channels() const
{
	return channels_;
}