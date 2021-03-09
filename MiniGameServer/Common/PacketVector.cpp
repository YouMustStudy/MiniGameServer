#include "PacketVector.h"
#include <iostream>

int PacketVector::DEFAULT_BUFFER_SIZE = 256;

PacketVector::PacketVector() : data(new char[DEFAULT_BUFFER_SIZE]), len(0), maxLen(DEFAULT_BUFFER_SIZE) { }
PacketVector::PacketVector(int size) : data(new char[size]), len(0), maxLen(size) { }
PacketVector::~PacketVector() { delete[] data; data = nullptr; len = 0; maxLen = 0; };

void PacketVector::EmplaceBack(void* src, size_t srcLen)
{
	size_t require_len = this->len + srcLen;
	if (require_len > maxLen) {
		char* new_data = new char[require_len];
		memcpy(new_data, data, this->len);
		delete[] data;
		data = new_data;
		maxLen = require_len;
	}
	memcpy(data + this->len, src, srcLen);
	this->len += srcLen;
}