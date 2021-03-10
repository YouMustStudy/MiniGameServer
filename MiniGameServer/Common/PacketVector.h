#pragma once

/**
@brief Vector for BYTE.
@author Gurnwoo Kim
*/

class PacketVector {
	static int DEFAULT_BUFFER_SIZE;

public:
	char* data{nullptr} ;
	size_t len;
	size_t maxLen;
	PacketVector();
	PacketVector(int size);
	~PacketVector();

	/**
	@brief insert data to vector.
	@param src pointer to insert data.
	@param len length of data.
	*/
	void EmplaceBack(void* src, size_t len);

	/**
	@brief clear vector.
	@details there's no change in data place. just clear saved length.
	*/
	void Clear() { len = 0; };
};