#pragma once

class MSIGenerator
{
public:
	static std::shared_ptr<MSIGenerator> create();

	virtual ~MSIGenerator() = default;
	virtual void generate() = 0;
};
