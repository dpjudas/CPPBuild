
#include "Precomp.h"
#include "Base64.h"
#include "IOData/DataBuffer.h"
#include <cstring>

/*
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m',
	'n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9','+','/'
*/

static unsigned char cl_char_to_base64[256] =
{
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0, 62,  0,  0,  0, 63,
	52, 53, 54, 55, 56, 57, 58, 59,  60, 61,  0,  0,  0,  0,  0,  0,
	 0,  0,  1,  2,  3,  4,  5,  6,   7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22,  23, 24, 25,  0,  0,  0,  0,  0,
	 0, 26, 27, 28, 29, 30, 31, 32,  33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,  49, 50, 51,  0,  0,  0,  0,  0,

	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0
};

class Base64Decoder_Impl
{
public:
	std::shared_ptr<DataBuffer> result = DataBuffer::create(0);

	unsigned char chunk[4] = {};
	size_t chunk_filled = 0;

	static void decode(unsigned char* output, const unsigned char* input, size_t size_input)
	{
		int i, o;
		for (i = 0, o = 0; i < size_input; i += 4, o += 3)
		{
			unsigned int v1 = cl_char_to_base64[input[i + 0]];
			unsigned int v2 = cl_char_to_base64[input[i + 1]];
			unsigned int v3 = cl_char_to_base64[input[i + 2]];
			unsigned int v4 = cl_char_to_base64[input[i + 3]];
			unsigned int value = (v1 << 18) + (v2 << 12) + (v3 << 6) + v4;

			output[o + 0] = (value >> 16) & 255;
			output[o + 1] = (value >> 8) & 255;
			output[o + 2] = value & 255;
		}
	}

	void reset()
	{
		result->setSize(0);
		chunk_filled = 0;
	}

	void feed(const void* _data, size_t size, bool append_result)
	{
		size_t pos = 0;
		const unsigned char* data = (const unsigned char*)_data;

		if (!append_result)
			result->setSize(0);

		// Handle any left-over data from last encode:

		if (chunk_filled > 0)
		{
			size_t needed = 4 - chunk_filled;
			if (size >= needed)
			{
				// Finish left-over data block:

				memcpy(chunk + chunk_filled, data, needed);
				size_t out_pos = result->size();
				result->setSize(out_pos + 3);
				decode((unsigned char*)result->data() + out_pos, chunk, 4);
				pos += needed;
				chunk_filled = 0;

				// Shorten result if we got an end of base64 data marker:

				if (chunk[2] == '=')
					result->setSize(result->size() - 2);
				else if (chunk[3] == '=')
					result->setSize(result->size() - 1);
			}
			else
			{
				memcpy(chunk + chunk_filled, data, size);
				chunk_filled += size;
				return;
			}
		}

		// Base64 encode what's available to us now:

		size_t blocks = (size - pos) / 4;
		size_t out_pos = result->size();
		result->setSize(out_pos + blocks * 3);
		decode((unsigned char*)result->data() + out_pos, data + pos, blocks * 4);
		pos += blocks * 4;

		// Shorten result if we got an end of base64 data marker:

		if (blocks > 0)
		{
			if (data[pos - 2] == '=')
				result->setSize(result->size() - 2);
			else if (data[pos - 1] == '=')
				result->setSize(result->size() - 1);
		}

		// Save data for last incomplete block:

		size_t leftover = size - pos;
		if (leftover > 4)
			throw std::runtime_error("Base64 decoder is broken!");
		chunk_filled = leftover;
		memcpy(chunk, data + pos, leftover);
	}
};

std::string Base64::encode(std::shared_ptr<DataBuffer> data)
{
	return encode(data->data(), data->size());
}

std::string Base64::decodeText(const void* data, size_t size)
{
	std::shared_ptr<DataBuffer> result = decode(data, size);
	return std::string(result->data(), result->size());
}

std::string Base64::decodeText(const std::string& data)
{
	return decodeText(data.data(), data.size());
}

std::shared_ptr<DataBuffer> Base64::decode(const void* data, size_t size)
{
	Base64Decoder_Impl decoder;
	decoder.feed(data, size, false);
	return decoder.result;
}

std::shared_ptr<DataBuffer> Base64::decode(const std::string& data)
{
	return decode(data.data(), data.size());
}

// ------------------------

static unsigned char cl_base64char[64] =
{
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m',
	'n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9','+','/'
};

class Base64Encoder_Impl
{
public:
	std::shared_ptr<DataBuffer> result = DataBuffer::create(0);

	unsigned char chunk[3] = {};
	size_t chunk_filled = 0;

	static void encode(unsigned char* output, const unsigned char* input, size_t size_input)
	{
		for (size_t i = 0, o = 0; i < size_input; i += 3, o += 4)
		{
			unsigned int v1 = input[i + 0];
			unsigned int v2 = input[i + 1];
			unsigned int v3 = input[i + 2];
			unsigned int value = (v1 << 16) + (v2 << 8) + v3;

			output[o + 0] = cl_base64char[(value >> 18) & 63];
			output[o + 1] = cl_base64char[(value >> 12) & 63];
			output[o + 2] = cl_base64char[(value >> 6) & 63];
			output[o + 3] = cl_base64char[value & 63];
		}
	}

	void reset()
	{
		result->setSize(0);
		chunk_filled = 0;
	}

	void feed(const void* _data, size_t size, bool append_result)
	{
		size_t pos = 0;
		const unsigned char* data = (const unsigned char*)_data;

		if (!append_result)
			result->setSize(0);

		// Handle any left-over data from last encode:

		if (chunk_filled > 0)
		{
			size_t needed = 3 - chunk_filled;
			if (size >= needed)
			{
				memcpy(chunk + chunk_filled, data, needed);
				size_t out_pos = result->size();
				result->setSize(out_pos + 4);
				encode((unsigned char*)result->data() + out_pos, chunk, 3);
				pos += needed;
				chunk_filled = 0;
			}
			else
			{
				memcpy(chunk + chunk_filled, data, size);
				chunk_filled += size;
				return;
			}
		}

		// Base64 encode what's available to us now:

		size_t blocks = (size - pos) / 3;
		size_t out_pos = result->size();
		result->setSize(out_pos + blocks * 4);
		encode((unsigned char*)result->data() + out_pos, data + pos, blocks * 3);
		pos += blocks * 3;

		// Save data for last incomplete block:

		size_t leftover = size - pos;
		if (leftover > 3)
			throw std::runtime_error("Base64 encoder is broken!");
		chunk_filled = leftover;
		memcpy(chunk, data + pos, leftover);
	}

	void finalize(bool append_result)
	{
		if (!append_result)
			result->setSize(0);
		if (chunk_filled == 0)
			return;

		// Allocate memory for last block:

		size_t pos = result->size();
		result->setSize(pos + 4);
		unsigned char* output = (unsigned char*)result->data() + pos;
		unsigned char* input = chunk;
		size_t size = chunk_filled;

		// Base64 last block:

		memset(input + size, 0, 3 - size);

		unsigned int v1 = input[0];
		unsigned int v2 = input[1];
		unsigned int v3 = input[2];
		unsigned int value = (v1 << 16) + (v2 << 8) + v3;

		output[0] = cl_base64char[(value >> 18) & 63];
		output[1] = cl_base64char[(value >> 12) & 63];
		output[2] = cl_base64char[(value >> 6) & 63];
		output[3] = cl_base64char[value & 63];

		if (chunk_filled == 1)
		{
			output[2] = '=';
			output[3] = '=';
		}
		else if (chunk_filled == 2)
		{
			output[3] = '=';
		}
	}
};

std::string Base64::encode(const void* data, size_t size)
{
	Base64Encoder_Impl encoder;
	encoder.feed(data, size, false);
	encoder.finalize(true);
	return std::string(encoder.result->data(), encoder.result->size());
}

std::string Base64::encode(const std::string& data)
{
	return encode(data.data(), data.size());
}
