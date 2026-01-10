#pragma once

class HttpUri;
class BuildSetup;

class PackageManager
{
public:
	void update(const BuildSetup& setup);

private:
	void download(const HttpUri& url, const std::string& filename);
};
