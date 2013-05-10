#include "FoundationPch.h"
#include "TUID.h"

#include "Platform/Exception.h"
#include "Foundation/Endian.h"

#include <time.h>
#include <sstream>
#include <algorithm>
#include <cctype>

#if HELIUM_OS_WIN
# include <iphlpapi.h>
#endif

using namespace Helium;

#pragma comment ( lib, "iphlpapi.lib" )

const tuid TUID::Null = 0x0;

static uint64_t s_CachedMacBits64 = 0; ///< cached 48-bit MAC address, centered in uint64_t

bool isAlpha( int c )
{
    return ( std::isalpha( c ) != 0 );
}

template< class T >
void ToString( tuid id, std::basic_string< T >& str )
{
    std::basic_ostringstream< T > sstr;
    sstr << TUID::HexFormat << id;
    str = sstr.str();
}

template< class T >
const T* GetPrefix()
{
	return NULL;
}

template<>
const char* GetPrefix()
{
	return "0x";
}

template<>
const wchar_t* GetPrefix()
{
	return L"0x";
}

template< class T >
bool FromString( const std::basic_string< T >& str, tuid& id )
{
    std::basic_stringstream< T > stream;
    typename std::basic_string< T >::const_iterator alphaIt = std::find_if( str.begin(), str.end(), isAlpha );
    if ( alphaIt != str.end() )
    {
        if ( str.length() >= 2 && ( str[1] == 'x' || str[1] == 'X' ) )
        {
            stream << str;
            stream >> std::hex >> id;
        }
        else
        {
            // this is icky, pretend that they entered a hex tuid without the prefix
            std::basic_string< T > prefixedStr = std::basic_string< T >( GetPrefix<T>() ) + str;

            stream << prefixedStr;
            stream >> std::hex >> id;
        }
    }
    else
    {
        // this is also icky, but it might actually be a decimal TUID
        stream << str;
        stream >> id;
    }

    return ( !stream.fail() && stream.eof() );
}

void TUID::ToString( std::string& id ) const
{
	::ToString< char >( m_ID, id );
}

bool TUID::FromString( const std::string& str )
{
	return ::FromString< char >( str, m_ID );
}

void TUID::ToString( std::wstring& id ) const
{
	::ToString< wchar_t >( m_ID, id );
}

bool TUID::FromString( const std::wstring& str )
{
	return ::FromString< wchar_t >( str, m_ID );
}

tuid TUID::Generate()
{
    tuid uid;
    Generate( uid );
    return uid;
}

void TUID::Generate( TUID& uid )
{
    Generate( uid.m_ID );
}

void TUID::Generate( tuid& uid )
{
    uid = (tuid)TUID::Null;

    // fetches the MAC address (if it has not already been set)
    if ( s_CachedMacBits64 == 0 )
    {
#if HELIUM_OS_WIN
        IP_ADAPTER_INFO adapterInfo[16];
        DWORD bufLength = sizeof( adapterInfo );
        DWORD status = GetAdaptersInfo( adapterInfo, &bufLength );
        if ( status != ERROR_SUCCESS )
        {
            throw Helium::Exception( TXT( "Could not get network adapter info to seed TUID generation." ) );
        }

        // cache the appropriate bits
        uint64_t bits = 0;
        uint64_t tempByte = 0;
        for ( int32_t address_byte = 5; address_byte >= 0; --address_byte )
        {
            tempByte = adapterInfo[0].Address[ address_byte ];
            bits |= tempByte << ( 8 * address_byte );
        }

        s_CachedMacBits64 |= (bits << 8); // shift left 8 to center
#else
        HELIUM_ASSERT( false );
#endif
    }

    uid |= s_CachedMacBits64;

    uint64_t timeBits = 0;

    // get the clock ticks
#if HELIUM_OS_WIN
    LARGE_INTEGER ticks;
    BOOL result = QueryPerformanceCounter( &ticks );
    if ( !result )
    {
        throw Helium::Exception( TXT( "Could not obtain performance counter ticks to generate TUID." ) );
    }
    timeBits = ticks.LowPart;
    timeBits = timeBits << 32; // shift left
#else
    HELIUM_ASSERT( false );
#endif

    // get the system time
    time_t systemTime;
    time( &systemTime );
    timeBits |= systemTime; // concat with clock ticks

    uid ^= timeBits;
}