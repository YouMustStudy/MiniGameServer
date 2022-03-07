#pragma once
#include <WS2tcpip.h>

/**
@brief EXPENDED class for WSAOVERLAPPED structure for IOCP.
@author Gurnwoo Kim
*/
class OverEx
{
private:
	WSAOVERLAPPED over;		///< WSAOVERLAPPED structure.
	WSABUF        wsabuf;	///< WSABUF structure.
	int           ev_type;	///< Event type for iocp. can be defined for its own goal.
public:
	char* packet;	        ///< Inner recv buffer.

	~OverEx();
	/**
	*@brief Basic Constructor.
	*/
	OverEx();

	/**
	*@brief Set event type.
	*@param[in] ev set event.
	*/
	OverEx( int ev );

	/**
	*@brief Set event type and allocate inner buffer. usually for recv data.
	*@param[in] ev set event.
	*@param[in] buf_len inner buffer size.
	*/
	OverEx( int ev, size_t buf_size );

	/**
	*@brief Set event type and copy buff to inner buffer. usually for send data.
	*@param[in] ev set event.
	*@param[in] buff want to send data pointer.
	*/
	OverEx( int ev, void* buff );

	/**
	*@brief Set event type and copy buff to inner buffer. usually for send data.
	*@param[in] ev set event.
	*@param[in] buff want to send data pointer.
	*@param[in] buf_size buff data size.
	*/
	OverEx( int ev, void* buff, size_t buf_size );

	/**
	*@brief return wsabuf pointer.
	*/
	WSABUF* Buffer() { return &wsabuf; }

	/**
	*@brief return buffer pointer.
	*/
	char* Data() { return packet; }

	/**
	*@brief return overlapped structure pointer.
	*/
	WSAOVERLAPPED* Overlapped() { return &over; }

	/**
	*@brief return event type.
	*/
	int EventType() { return ev_type; }

	/**
	*@brief reset for recv.
	*/
	void Reset();

	/**
	*@brief init for socket communication.
	@param[in] buf_size inner buffer size.
	*/
	void Init( size_t buf_size );

	/**
	*@brief set event.
	*@param[in] ev eventType
	*/
	void SetEvent( int ev );
};