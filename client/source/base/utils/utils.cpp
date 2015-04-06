#include "pch.h"
#include "utils.h"

#if defined (WIN32)
#include <Rpc.h>
#elif defined (__ANDROID__)
#include <uuidlib/uuid.h>
#else
#include <uuid/uuid.h>
#endif





std::string Utils::generateUUID( )
{
    std::string s;

#ifdef WIN32
    UUID uuid;
    UuidCreate ( &uuid );

    unsigned char * str;
    UuidToStringA ( &uuid, &str );

    s = ( const char* ) str;

    RpcStringFreeA ( &str );
#else
    uuid_t uuid;
    uuid_generate_random ( uuid );
    char str[37];
    uuid_unparse ( uuid, str );

    s = str;
#endif

    return s;
}



const UChar * Utils::WCSToUString(const wchar_t * str)
{
    static UChar result[ 2048 ];
    
    int32_t length = 0;
    UErrorCode error = U_ZERO_ERROR;
    
    u_strFromWCS( result, 2048, &length, str, -1, &error );

    if( U_FAILURE( error ) )
        return NULL;
    
    return result;
}



const wchar_t * Utils::UTF8ToWCS(const char * str)
{
    static UChar valueUni[ 1024 ];
    int32_t length = 0;
    UErrorCode error = U_ZERO_ERROR;
    u_strFromUTF8( valueUni, 1024, &length, str, -1, &error );

    error = U_ZERO_ERROR;
    static wchar_t valueW[ 1024 ];
    u_strToWCS( valueW, 1024, &length, valueUni, -1, &error );

    return valueW;
}




const wchar_t * Utils::ANSIToWCS(const char * str)
{
    static wchar_t result[ 2048 ];

    wchar_t * o = result;
    while( *str )
        *o++ = *str++;
    *o = 0;

    return result;
}




const char * Utils::format(const char * fmt, ...)
{
    static char result[ 2048 ];

    va_list args;
    va_start(args, fmt);

#ifdef WIN32
    _vsnprintf(result, 2048, fmt, args);
#else
    vsnprintf(result, 2048, fmt, args);
#endif

    va_end(args);

    return result;
}




const wchar_t * Utils::clipTextToBounds(const wchar_t * text, float width, const gameplay::Font * font, float fontSize)
{
    static std::wstring result;
    
    result = text;

    float textw = 0, texth = 0;
    font->measureText( text, fontSize, &textw, &texth );
    if( textw >= width && width > 0 )
    {
        result.erase(result.end() - 1, result.end());
        result.push_back( '.' );
        result.push_back( '.' );
        result.push_back( '.' );
        do
        {
            result.erase( result.end() - 4, result.end() );
            result.push_back( '.' );
            result.push_back( '.' );
            result.push_back( '.' );
            font->measureText( result.c_str( ), fontSize, &textw, &texth );
        }
        while( textw >= width );
    }

    return result.c_str( );
}


void Utils::serializeString(gameplay::Stream * stream, const std::string& str)
{
    int32_t size = static_cast<int32_t>(str.size());
    stream->write(&size, sizeof(size), 1);
    stream->write(str.c_str(), sizeof(char), size);
}

void Utils::deserializeString(gameplay::Stream * stream, std::string& str)
{
    str.clear();
    int32_t size = 0;
    if (stream->read(&size, sizeof(size), 1) != 1)
        return;

    if (size < 0 || size > 65535)
        return; // something wrong with data

    char * buf = reinterpret_cast<char *>(alloca(sizeof(char)* (size + 1)));
    if (buf)
    {
        stream->read(buf, sizeof(char), size);
        buf[size] = '\0';
        str = buf;
    }
}
