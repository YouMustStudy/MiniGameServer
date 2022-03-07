#pragma once

/**
@brief Vector for BYTE.
@author Gurnwoo Kim
*/

class PacketVector {
	static int DEFAULT_BUFFER_SIZE;

public:
	char*  data{ nullptr }; ///< data buffer.
	size_t len;             ///< data size.
	size_t maxLen;          ///< data max size.

	/**
	*@brief default Constructor.
	*/
	PacketVector();

	/**
	*@brief default Constructor.
	*@param[in] size initialize buffer size.
	*/
	PacketVector(int size);

	/**
	*@brief default Deconstructor.
	*/
	~PacketVector();

	/**
	*@brief insert data to vector.
	*@param[in] src pointer to insert data.
	*@param[in] len length of data.
	*/
	void EmplaceBack(void* src, size_t len);

	/**
	*@brief clear vector.
	*@details there's no change in data place. just clear saved length.
	*/
	void Clear() { len = 0; };
};