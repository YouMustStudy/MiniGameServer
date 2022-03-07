#include "OverEx.h"
#include "..\protocol.h"

OverEx::~OverEx() 
{
	if( packet != nullptr ) {
		delete[] packet;
		packet = nullptr;
	}
}

OverEx::OverEx() :
	ev_type( 0 ),
	packet( nullptr ),
	over(),
	wsabuf()
{
}

OverEx::OverEx( int ev ) :
	ev_type( ev ),
	packet( nullptr ),
	over(),
	wsabuf()
{
}

OverEx::OverEx( int ev, size_t buf_size ) :
	OverEx( ev )
{
	Init( buf_size );
}

OverEx::OverEx( int ev, void* buff ) :
	OverEx( ev )
{
	DEFAULT_PACKET* cdp = reinterpret_cast<DEFAULT_PACKET*>( buff );
	Init( cdp->size );
	memcpy( packet, buff, cdp->size );
}

OverEx::OverEx( int ev, void* buff, size_t buf_size ) :
	OverEx( ev )
{
	Init( buf_size );
	memcpy( packet, buff, buf_size );
}

void OverEx::Init( size_t buf_size )
{
	if( packet != nullptr ) {
		delete[] packet;
		packet = nullptr;
	}
	packet = new char[ buf_size ];
	memset( &over, 0, sizeof( WSAOVERLAPPED ) );
	wsabuf.buf = packet;
	wsabuf.len = static_cast<ULONG>( buf_size );
}

void OverEx::Reset()
{
	memset( &over, 0, sizeof( WSAOVERLAPPED ) );
}

void OverEx::SetEvent( int ev )
{
	ev_type = ev;
}