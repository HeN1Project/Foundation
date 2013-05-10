#pragma once

#include <iostream>
#include <iomanip>
#include <string>

#include <vector>
#include <set>
#include <map>
#if HELIUM_OS_WIN
# include <hash_map>
#endif

#include "Platform/Types.h"

#include "Foundation/API.h"
#include "Foundation/Endian.h"

typedef uint64_t tuid;
typedef std::set< tuid > S_tuid;
typedef std::vector< tuid > V_tuid;
typedef std::map< tuid, tuid > M_tuid;
typedef std::map< tuid, uint32_t > M_tuidu32;

#define TUID_HEX_FORMAT TXT( "0x%016I64X" )
#define TUID_INT_FORMAT TXT( "%I64u" )

namespace Helium
{
    class HELIUM_FOUNDATION_API TUID
    {
    protected:
        tuid m_ID;

    public:
        // The null ID
        static const tuid Null;

        // Generates a unique ID
        static tuid Generate();
        static void Generate( TUID& uid );
        static void Generate( tuid& uid );

        TUID(); // Null ID
        TUID( tuid id );
        TUID( const TUID &id );
        TUID( const tstring& id );

        TUID& operator=( const TUID &rhs );
        bool operator==( const TUID &rhs ) const;
        bool operator==( const tuid &rhs ) const;
        bool operator!=( const TUID &rhs ) const;
        bool operator!=( const tuid &rhs ) const;
        bool operator<( const TUID &rhs ) const;

        // Interop with tuid
        operator tuid() const;

		// String conversion
        void ToString( std::string& id ) const;
        bool FromString( const std::string& str );
        void ToString( std::wstring& id ) const;
        bool FromString( const std::wstring& str );

        // Resets the ID to be the null ID
        void Reset();

        static inline std::ostream& HexFormat( std::ostream& base )
        {
            return base << "0x" << std::setfill( '0' ) << std::setw(16) << std::right << std::hex << std::uppercase;
        }

        static inline std::wostream& HexFormat( std::wostream& base )
        {
            return base << L"0x" << std::setfill( L'0' ) << std::setw(16) << std::right << std::hex << std::uppercase;
        }

        friend HELIUM_FOUNDATION_API std::ostream& operator<<( std::ostream& stream, const TUID& id );
        friend HELIUM_FOUNDATION_API std::istream& operator>>( std::istream& stream, TUID& id );

        friend HELIUM_FOUNDATION_API std::wostream& operator<<( std::wostream& stream, const TUID& id );
        friend HELIUM_FOUNDATION_API std::wistream& operator>>( std::wistream& stream, TUID& id );
	};

    inline TUID::TUID()
        : m_ID( 0x0 )
    {

    }

    inline TUID::TUID( tuid id )
        : m_ID( id )
    {

    }

    inline TUID::TUID(const TUID &id)
        : m_ID( id.m_ID )
    {

    }

    inline TUID::TUID( const tstring& id )
    {
        FromString( id );
    }

    inline TUID& TUID::operator=(const TUID &rhs)
    {
        m_ID = rhs.m_ID;
        return *this;
    }

    inline bool TUID::operator==(const TUID &rhs) const
    {
        return m_ID == rhs.m_ID;
    }

    inline bool TUID::operator==( const tuid& rhs ) const
    {
        return m_ID == rhs;
    }

    inline bool TUID::operator!=(const TUID &rhs) const
    {
        return m_ID != rhs.m_ID;
    }

    inline bool TUID::operator!=( const tuid &rhs ) const
    {
        return m_ID != rhs;
    }

    inline bool TUID::operator<(const TUID &rhs) const
    {
        return m_ID < rhs.m_ID;
    }

    inline TUID::operator tuid() const
    {
        return m_ID;
    }

    inline void TUID::Reset()
    {
        m_ID = 0x0;
    }

    HELIUM_FOUNDATION_API inline std::ostream& operator<<(std::ostream& stream, const TUID& id)
    {
        std::string s;
        id.ToString(s);
        stream << s;
        return stream;
    }

    HELIUM_FOUNDATION_API inline std::istream& operator>>(std::istream& stream, TUID& id)
    {
        std::string s;
        stream >> s;
        id.FromString(s.c_str());
        return stream;
    }

    HELIUM_FOUNDATION_API inline std::wostream& operator<<(std::wostream& stream, const TUID& id)
    {
        std::wstring s;
        id.ToString(s);
        stream << s;
        return stream;
    }

    HELIUM_FOUNDATION_API inline std::wistream& operator>>(std::wistream& stream, TUID& id)
    {
        std::wstring s;
        stream >> s;
        id.FromString(s.c_str());
        return stream;
    }

#if HELIUM_OS_WIN
    // 
    // Hashing class for storing UIDs as keys to a hash_map.
    // 

    class TUIDHasher : public stdext::hash_compare< uint64_t >
    {
    public:
        size_t operator()( const TUID& tuid ) const
        {
            return stdext::hash_compare< uint64_t >::operator()( 0 );
        }

        bool operator()( const TUID& tuid1, const TUID& tuid2 ) const
        {
            return stdext::hash_compare< uint64_t >::operator()( tuid1, tuid2 );
        }
    };

    typedef stdext::hash_map< TUID, TUID, TUIDHasher > HM_TUID;
    typedef stdext::hash_map< TUID, uint32_t, TUIDHasher > HM_TUIDU32;
    typedef std::vector<TUID> V_TUID;
    typedef std::set<TUID> S_TUID;

    template<> inline void Swizzle<TUID>(TUID& val, bool swizzle)
    {
        // operator tuid() const will handle the conversion into the other swizzle func
        val = ConvertEndian(val, swizzle);
    }
#endif
}
