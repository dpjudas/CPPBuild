
#include "Main.h"
#include <iostream>
#include <string>
#include <emscripten/val.h>

using JSValue = emscripten::val;

int main()
{
	std::cout << "Hello World!" << std::endl;

	std::string imgTag = "img";
	std::string attrName = "src";
	std::string src = "reticulated-giraffe.webp";

	JSValue body = JSValue::global("document")["body"];
	JSValue img = JSValue::global("document").call<JSValue>("createElement", imgTag);
	img.call<void>("setAttribute", attrName, src);
	body.call<void>("appendChild", img);

	return 0;
}
