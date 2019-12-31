#pragma once

#include <vector>

class GetImageSize
{
public:
	GetImageSize();
	~GetImageSize();

private:

	int GetPNGWidthHeight(const char * path, unsigned int * punWidth, unsigned int * punHeight);

	int GetJPEGWidthHeight(const char * path, unsigned int * punWidth, unsigned int * punHeight);

	void GetPicWidthHeight(const char * path, unsigned int * punWidth, unsigned int * punHeight);

	std::vector < std::string> Get(std::wstring path);

};

GetImageSize::GetImageSize()
{
}

GetImageSize::~GetImageSize()
{
}