/****************************************************************************
 * Copyright � 2003-2005 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#if !defined(dataelement_h)
#define dataelement_h 1

#include "Types.h"

namespace MRN {

typedef union{
    char c;
    unsigned char uc;
    int16_t hd;
    uint16_t uhd;
    int32_t d;
    uint32_t ud;
    int64_t ld;
    uint64_t uld;
    float f;
    double lf;
    const void * p; // May need to be allocated by pdr routine
} DataValue;

typedef enum {
    UNKNOWN_T, CHAR_T, UCHAR_T,
    CHAR_ARRAY_T, UCHAR_ARRAY_T,
    STRING_T, STRING_ARRAY_T,
    INT16_T, UINT16_T,
    INT16_ARRAY_T, UINT16_ARRAY_T,
    INT32_T, UINT32_T,
    INT32_ARRAY_T, UINT32_ARRAY_T,
    INT64_T, UINT64_T,
    INT64_ARRAY_T, UINT64_ARRAY_T,
    FLOAT_T, DOUBLE_T,
    FLOAT_ARRAY_T, DOUBLE_ARRAY_T
} DataType;

class DataElement{
 public:
    DataValue val;
    DataType type;
    uint32_t array_len;
    bool destroy_data;

    DataElement( ) : type(UNKNOWN_T), array_len(0), destroy_data(false) { 
        val.uld=0;
    }
    ~DataElement();

    void set_DestroyData( bool d ) { destroy_data = d; }
    DataType get_Type( void ) const { return type; }

    char get_char( void ) const { return val.c; }
    void set_char( char c ) { val.c = c; type = CHAR_T; }

    unsigned char get_uchar( void ) const { return val.uc; }
    void set_uchar( unsigned char uc ) { val.uc = uc; type = UCHAR_T; }

    int16_t get_int16_t( void ) const { return val.hd; }
    void set_int16_t( int16_t hd ) { val.hd = hd; type = INT16_T; }

    uint16_t get_uint16_t( void ) const { return val.uhd; }
    void set_uint16_t( uint16_t uhd ) { val.uhd = uhd; type = UINT16_T; };

    int32_t get_int32_t( void ) const { return val.d; }
    void set_int32_t( int32_t d ) { val.d = d; type = INT32_T; };

    uint32_t get_uint32_t( void ) const { return val.ud; }
    void set_uint32_t( uint32_t ud ) { val.ud = ud; type = UINT32_T; };

    int64_t get_int64_t( void ) const { return val.ld; }
    void set_int64_t( int64_t ld ) { val.ld = ld; type = INT64_T; };

    uint64_t get_uint64_t( void ) const { return val.uld; }
    void set_uint64_t( uint64_t uld ) { val.uld = uld; type = UINT64_T; };

    float get_float( void ) const { return val.f; }
    void set_float( float f ) { val.f = f; type = FLOAT_T; };

    double get_double( void ) const { return val.lf; }
    void set_double( double lf ) { val.lf = lf; type = DOUBLE_T; };

    const char * get_string( void ) const { return (const char *)val.p; }
    void set_string( const char *p ) { val.p = p; type = STRING_T; };

    const void * get_array( DataType *t, uint32_t *len ) const
        { *t=type; *len=array_len; return val.p; }
    void set_array( const void *p, DataType t, uint32_t len )
        { val.p = p; type = t; array_len = len; }

		void print()const;
};

DataType Fmt2Type(const char * cur_fmt);

} /* namespace MRN */

#endif /* dataelement_h */
