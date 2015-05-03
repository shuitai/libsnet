//
// ByteOrder.h
//
// $Id: //poco/1.4/Foundation/include/Poco/ByteOrder.h#5 $
//
// Library: Foundation
// Package: Core
// Module:  ByteOrder
//
// Copyright (c) 2004-2014, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Foundation_ByteOrder_INCLUDED
#define Foundation_ByteOrder_INCLUDED


#include "common.h"
#if defined(_MSC_VER)
#include <stdlib.h> // builtins
#endif

class ByteOrder
	/// This class contains a number of static methods
	/// to convert between big-endian and little-endian
	/// integers of various sizes.
{
public:
	static int16_t FlipBytes(int16_t value);
	static uint16_t FlipBytes(uint16_t value);
	static int32_t FlipBytes(int32_t value);
	static uint32_t FlipBytes(uint32_t value);

#if defined(LIB_HAVE_INT64)
	static int64_t FlipBytes(int64_t value);
	static uint64_t FlipBytes(uint64_t value);
#endif

	static int16_t ToBigEndian(int16_t value);
	static uint16_t ToBigEndian (uint16_t value);
	static int32_t ToBigEndian(int32_t value);
	static uint32_t ToBigEndian (uint32_t value);
#if defined(LIB_HAVE_INT64)
	static int64_t ToBigEndian(Int64 value);
	static uint64_t ToBigEndian (uint64_t value);
#endif

	static int16_t FromBigEndian(int16_t value);
	static uint16_t FromBigEndian (uint16_t value);
	static int32_t FromBigEndian(int32_t value);
	static uint32_t FromBigEndian (uint32_t value);
#if defined(LIB_HAVE_INT64)
	static int64_t FromBigEndian(int64_t value);
	static uint64_t FromBigEndian (uint64_t value);
#endif

	static int16_t ToLittleEndian(int16_t value);
	static uint16_t ToLittleEndian (uint16_t value);
	static int32_t ToLittleEndian(int32_t value);
	static uint32_t ToLittleEndian (uint32_t value);
#if defined(LIB_HAVE_INT64)
	static int64_t ToLittleEndian(int64_t value);
	static uint64_t ToLittleEndian (uint64_t value);
#endif

	static int16_t FromLittleEndian(int16_t value);
	static uint16_t FromLittleEndian (uint16_t value);
	static int32_t FromLittleEndian(int32_t value);
	static uint32_t FromLittleEndian (uint32_t value);
#if defined(LIB_HAVE_INT64)
	static int64_t FromLittleEndian(int64_t value);
	static uint64_t FromLittleEndian (uint64_t value);
#endif

	static int16_t ToNetwork(int16_t value);
	static uint16_t ToNetwork (uint16_t value);
	static int32_t ToNetwork(int32_t value);
	static uint32_t ToNetwork (uint32_t value);
#if defined(LIB_HAVE_INT64)
	static int64_t ToNetwork(int64_t value);
	static uint64_t ToNetwork (uint64_t value);
#endif

	static int16_t FromNetwork(int16_t value);
	static uint16_t FromNetwork (uint16_t value);
	static int32_t FromNetwork(int32_t value);
	static uint32_t FromNetwork (uint32_t value);
#if defined(LIB_HAVE_INT64)
	static int64_t FromNetwork(int64_t value);
	static uint64_t FromNetwork (uint64_t value);
#endif
};


#if !defined(BYTESWAP_BUILTINS)
	#if defined(_MSC_VER)
		#if (MSVC_VERSION > 71)
			#define PHAVE_MSC_BYTESWAP 1
		#endif
	#elif defined(__clang__) 
		#if __has_builtin(__builtin_bswap32)
			#define HAVE_GCC_BYTESWAP 1
		#endif
	#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
		#define HAVE_GCC_BYTESWAP 1
	#endif
#endif


//
// inlines
//
inline uint16_t ByteOrder::FlipBytes(uint16_t value)
{
#if defined(HAVE_MSC_BYTESWAP)
	return _byteswap_ushort(value);
#else
	return ((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00);
#endif
}


inline int16_t ByteOrder::FlipBytes(int16_t value)
{
	return int16_t(FlipBytes(uint16_t(value)));
}


inline uint32_t ByteOrder::FlipBytes(uint32_t value)
{
#if defined(HAVE_MSC_BYTESWAP)
	return _byteswap_ulong(value);
#elif defined(HAVE_GCC_BYTESWAP)
	return __builtin_bswap32(value);
#else
	return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00)
	     | ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
#endif
}


inline int32_t ByteOrder::FlipBytes(int32_t value)
{
	return int32_t(FlipBytes(uint32_t(value)));
}


#if defined(LIB_HAVE_INT64)
inline uint64_t ByteOrder::FlipBytes(UInt64 value)
{
#if defined(HAVE_MSC_BYTESWAP)
	return _byteswap_uint64(value);
#elif defined(HAVE_GCC_BYTESWAP)
	return __builtin_bswap64(value);
#else
	uint32_t hi = uint32_t(value >> 32);
	uint32_t lo = uint32_t(value & 0xFFFFFFFF);
	return uint64_t(FlipBytes(hi)) | (uint64_t(FlipBytes(lo)) << 32);
#endif
}


inline Int64 ByteOrder::FlipBytes(Int64 value)
{
	return int64_t(FlipBytes(uint64_t(value)));
}
#endif


//
// some macro trickery to automate the method implementation
//
#define IMPLEMENT_BYTEORDER_NOOP_(op, type) \
	inline type ByteOrder::op(type value)		\
	{											\
		return value;							\
	}
#define IMPLEMENT_BYTEORDER_FLIP_(op, type) \
	inline type ByteOrder::op(type value)		\
	{											\
		return FlipBytes(value);				\
	}


#if defined(LIB_HAVE_INT64)
	#define IMPLEMENT_BYTEORDER_NOOP(op) \
		IMPLEMENT_BYTEORDER_NOOP_(op, int16_t)	\
		IMPLEMENT_BYTEORDER_NOOP_(op, uint16_t)	\
		IMPLEMENT_BYTEORDER_NOOP_(op, int32_t)	\
		IMPLEMENT_BYTEORDER_NOOP_(op, uint32_t)	\
		IMPLEMENT_BYTEORDER_NOOP_(op, int64_t)	\
		IMPLEMENT_BYTEORDER_NOOP_(op, uint64_t)
	#define IMPLEMENT_BYTEORDER_FLIP(op) \
		IMPLEMENT_BYTEORDER_FLIP_(op, int16_t)	\
		IMPLEMENT_BYTEORDER_FLIP_(op, uint16_t)	\
		IMPLEMENT_BYTEORDER_FLIP_(op, int32_t)	\
		IMPLEMENT_BYTEORDER_FLIP_(op, uint32_t)	\
		IMPLEMENT_BYTEORDER_FLIP_(op, int64_t)	\
		IMPLEMENT_BYTEORDER_FLIP_(op, uint64_t)
#else
	#define IMPLEMENT_BYTEORDER_NOOP(op) \
		IMPLEMENT_BYTEORDER_NOOP_(op, int16_t)	\
		IMPLEMENT_BYTEORDER_NOOP_(op, uint16_t)	\
		IMPLEMENT_BYTEORDER_NOOP_(op, int32_t)	\
		IMPLEMENT_BYTEORDER_NOOP_(op, uint32_t)
	#define IMPLEMENT_BYTEORDER_FLIP(op) \
		IMPLEMENT_BYTEORDER_FLIP_(op, int16_t)	\
		IMPLEMENT_BYTEORDER_FLIP_(op, uint16_t)	\
		IMPLEMENT_BYTEORDER_FLIP_(op, int32_t)	\
		IMPLEMENT_BYTEORDER_FLIP_(op, uint32_t)
#endif


#if defined(ARCH_BIG_ENDIAN)
	#define IMPLEMENT_BYTEORDER_BIG IMPLEMENT_BYTEORDER_NOOP
	#define IMPLEMENT_BYTEORDER_LIT IMPLEMENT_BYTEORDER_FLIP
#else
	#define IMPLEMENT_BYTEORDER_BIG IMPLEMENT_BYTEORDER_FLIP
	#define IMPLEMENT_BYTEORDER_LIT IMPLEMENT_BYTEORDER_NOOP
#endif


IMPLEMENT_BYTEORDER_BIG(ToBigEndian)
IMPLEMENT_BYTEORDER_BIG(FromBigEndian)
IMPLEMENT_BYTEORDER_BIG(ToNetwork)
IMPLEMENT_BYTEORDER_BIG(FromNetwork)
IMPLEMENT_BYTEORDER_LIT(ToLittleEndian)
IMPLEMENT_BYTEORDER_LIT(FromLittleEndian)


#endif // Foundation_ByteOrder_INCLUDED
