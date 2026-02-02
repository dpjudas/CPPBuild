
#include "Precomp.h"
#include "HttpClient.h"
#include "HttpConnection.h"
#include <regex>

HttpUri::HttpUri(const std::string& uri)
{
	size_t pos, pos2, pos3, pos4;

	// Extract scheme
	pos = uri.find(':');
	if (pos != std::string::npos)
	{
		scheme = uri.substr(0, pos);
		for (auto& c : scheme)
		{
			if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
		}
	}
	else
	{
		pos = 0;
	}

	// Extract authority
	if (pos + 2 < uri.length() && uri[pos + 1] == '/' && uri[pos + 2] == '/')
	{
		pos += 3;
		pos2 = uri.find('/', pos);
		if (pos2 == std::string::npos) pos2 = uri.length();

		std::string authority = uri.substr(pos, pos2 - pos);

		// Extract userinfo
		pos3 = authority.find('@');
		if (pos3 != std::string::npos)
		{
			std::string userinfo = authority.substr(0, pos3);
			pos4 = userinfo.find(':');
			if (pos4 != std::string::npos)
			{
				username = userinfo.substr(0, pos4);
				password = userinfo.substr(pos4 + 1);
			}
			else
			{
				username = userinfo;
			}
			pos3++;
		}
		else
		{
			pos3 = 0;
		}

		// Extract host and port
		pos4 = authority.find(':', pos3);
		if (pos4 != std::string::npos)
		{
			host = authority.substr(pos3, pos4 - pos3);
			port = std::atoi(authority.substr(pos4 + 1).c_str());
		}
		else
		{
			host = authority.substr(pos3);
		}

		pos = pos2;
	}

	// Extract path, query and fragment
	pos2 = uri.find_first_of("?#", pos);
	if (pos2 == std::string::npos)
	{
		path = uri.substr(pos);
	}
	else if (uri[pos2] == '?')
	{
		path = uri.substr(pos, pos2 - pos);
		pos3 = uri.find('#', pos2 + 1);
		if (pos3 == std::string::npos)
		{
			query = uri.substr(pos2 + 1);
		}
		else
		{
			pos2++;
			query = uri.substr(pos2, pos3 - pos2);
			fragment = uri.substr(pos3 + 1);
		}
	}
	else
	{
		path = uri.substr(pos, pos2 - pos);
		fragment = uri.substr(pos2 + 1);
	}
}

HttpUri HttpUri::combine(const HttpUri& base, const std::string& relativeUri)
{
	HttpUri result = base;
	HttpUri relative(relativeUri);

	if (!relative.scheme.empty())
	{
		result.scheme = relative.scheme;
		result.port = 0;
	}

	if (!relative.host.empty())
	{
		result.host = relative.host;
		result.port = relative.port;
		result.username = relative.username;
		result.password = relative.password;
	}

	if (!relative.path.empty() && relative.path.front() == '/')
	{
		result.path = relative.path;
	}
	else
	{
		if (result.path.empty() || result.path.back() != '/') result.path.push_back('/');
		result.path += relative.path;
	}

	result.query = relative.query;
	result.fragment = relative.fragment;

	return result;
}

std::string HttpUri::toString() const
{
	std::string schemepart;
	if (!scheme.empty())
		schemepart = scheme + ":";

	std::string authority;
	if (!host.empty())
	{
		std::string userinfo;
		if (!username.empty() && !password.empty())
			userinfo = username + ":" + password + "@";
		else if (!username.empty())
			userinfo = username + "@";

		std::string portinfo;
		if (port != 0) portinfo = ":" + std::to_string(port);

		authority = "//" + userinfo + host + portinfo;
	}

	std::string pathpart;
	if (path.empty() || path.front() != '/')
		pathpart = "/" + path;
	else
		pathpart = path;

	if (!query.empty() && query.front() != '?')
		pathpart += "?" + query;
	else if (!query.empty())
		pathpart += query;

	if (!fragment.empty() && fragment.front() != '#')
		pathpart += "#" + fragment;
	else if (!fragment.empty())
		pathpart += fragment;

	return schemepart + authority + pathpart;
}

/////////////////////////////////////////////////////////////////////////////

std::string HttpResponse::getHeader(std::string name)
{
	for (char& c : name) { if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a'; }

	for (auto& it : headers)
	{
		std::string key = it.first;
		for (char& c : key) { if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a'; }
		if (name == key)
			return it.second;
	}
	return {};
}

/////////////////////////////////////////////////////////////////////////////

HttpResponse HttpClient::send(const HttpRequest& request)
{
	std::string abspath = request.url.path;
	if (abspath.empty()) abspath = "/";
	if (!request.url.query.empty()) abspath += "?" + request.url.query;

	std::string requestheader = request.type + " " + abspath + " HTTP/1.1\r\n";

	// Required Host header
	if (request.url.port != 0)
		requestheader += "Host: " + request.url.host + ":" + std::to_string(request.url.port) + "\r\n";
	else
		requestheader += "Host: " + request.url.host + "\r\n";

	// Required Content-Length header
	requestheader += "Content-Length: " + std::to_string(request.data.size()) + "\r\n";

	// Request headers
	for (auto& it : request.headers)
	{
		requestheader += it.first + ": " + it.second + "\r\n";
	}
	requestheader += "\r\n";

	// Send the request
	auto connection = HttpConnection::create();
	connection->connect(request.url);
	connection->writeAll(requestheader.data(), requestheader.size());
	if (!request.data.empty())
		connection->writeAll(request.data.data(), request.data.size());

	HttpResponse response;

	// Read the response header
	std::string statusline = connection->readLine();

	try
	{
		std::regex status_regex(R"(^(HTTP/[\d.]+)\s+(\d{3})\s+(.+)$)", std::regex::icase);
		std::smatch matches;
		if (std::regex_match(statusline, matches, status_regex) && matches.size() == 4)
		{
			// std::string version = matches[1];
			response.statusCode = std::atoi(matches[2].str().c_str());
			response.statusText = matches[3];
		}
	}
	catch (...)
	{
	}

	while (true)
	{
		std::string line = connection->readLine();
		if (line.empty())
			break;
		size_t pos = line.find(':');
		if (pos != std::string::npos)
		{
			std::string name = line.substr(0, pos);

			pos = line.find_first_not_of(' ', pos + 1);
			if (pos == std::string::npos) pos = line.length();
			std::string value = line.substr(pos);

			response.headers[name] = value;
		}
	}

	// Read the response body
	auto it = response.headers.find("Transfer-Encoding");
	if (it != response.headers.end() && it->second == "chunked")
	{
		while (true)
		{
			std::string line = connection->readLine();
			int size = 0;
			for (char c : line)
			{
				size <<= 4;
				if (c >= '0' && c <= '9') size += c - '0';
				else if (c >= 'A' && c <= 'F') size += c - 'A' + 10;
				else if (c >= 'a' && c <= 'f') size += c - 'a' + 10;
			}
			size_t start = response.data.size();
			response.data.resize(response.data.size() + size);
			connection->readAll(response.data.data() + start, size);
			connection->readLine();
			if (size == 0)
				break;
		}
	}
	else
	{
		it = response.headers.find("Content-Length");
		if (it != response.headers.end())
		{
			size_t length = std::atol(it->second.c_str());
			response.data.resize(length);
			connection->readAll(response.data.data(), response.data.size());
		}
	}

	return response;
}
