#pragma once

#include "BuildSetup.h"

class MSIGenerator
{
public:
	static std::shared_ptr<MSIGenerator> create();

	virtual ~MSIGenerator() = default;
	virtual void generate(const std::string& binDir, const std::string& sourcePath, const BuildInstaller& installerDef) = 0;
};
