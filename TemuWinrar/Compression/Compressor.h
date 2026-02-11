#pragma once

class Compressor {
	virtual ~Compressor() = default;
	virtual const char* name() const = 0;
	//compress
	//decompress
};