#pragma once

#include <string>

class HttpUri
{
public:
	HttpUri() = default;
	HttpUri(const std::string& uri);

	static HttpUri combine(const HttpUri& base, const std::string& relativeUri);

	std::string scheme;
	std::string username;
	std::string password;
	std::string host;
	int port = 0;
	std::string path;
	std::string query;
	std::string fragment;

	std::string toString() const;
};

class HttpRequest
{
public:
	HttpRequest(std::string type, HttpUri url) : type(type), url(url) {}

	std::string type; // POST, GET, etc.
	HttpUri url;
	std::map<std::string, std::string> headers;
	std::vector<uint8_t> data;
};

class HttpResponse
{
public:
	int statusCode = 0;
	std::string statusText;
	std::map<std::string, std::string> headers;
	std::vector<uint8_t> data;

	std::string getHeader(std::string name);
};

class HttpClient
{
public:
	static HttpResponse send(const HttpRequest& request);
};
