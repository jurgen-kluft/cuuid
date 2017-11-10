#include "xuuid/x_uuid.h"
#include "xbase/x_va_list.h"
#include "xbase/x_memory_std.h"
#include "xbase/x_string_std.h"
#include "xbase/x_endian.h"

namespace xcore
{
	xuuid::xuuid()
		: _timeLow(0)
		, _timeMid(0)
		, _timeHiAndVersion(0)
		, _clockSeq(0)
	{
		for (s32 i=0; i<6; ++i)
			_node[i]=0;
	}

	xuuid::xuuid(const xuuid& uuid)
		: _timeLow(uuid._timeLow)
		, _timeMid(uuid._timeMid)
		, _timeHiAndVersion(uuid._timeHiAndVersion)
		, _clockSeq(uuid._clockSeq)
	{
		for (s32 i=0; i<6; ++i)
			_node[i]=uuid._node[i];
	}

	xuuid::xuuid(const char* uuid)
	{
		tryParse(uuid);
	}


	xuuid::xuuid(u32 timeLow, u32 timeMid, u32 timeHiAndVersion, u16 clockSeq, u8 node[])
		: _timeLow(timeLow)
		, _timeMid(timeMid)
		, _timeHiAndVersion(timeHiAndVersion)
		, _clockSeq(clockSeq)
	{
		for (s32 i=0; i<6; ++i)
			_node[i]=node[i];
	}

	xuuid::xuuid(const xbyte* bytes, Version version)
	{
		copyFrom(bytes);

		_timeHiAndVersion &= 0x0FFF;
		_timeHiAndVersion |= (version << 12);
		_clockSeq &= 0x3FFF;
		_clockSeq |= 0x8000;
	}


	xuuid::~xuuid()
	{
	}


	xuuid& xuuid::operator = (const xuuid& uuid)
	{
		if (&uuid != this)
		{
			_timeLow = uuid._timeLow;
			_timeMid = uuid._timeMid;
			_timeHiAndVersion = uuid._timeHiAndVersion;
			_clockSeq         = uuid._clockSeq;
			for (s32 i=0; i<6; ++i)
				_node[i]=uuid._node[i];
		}
		return *this;
	}

	inline void swap_u32(u32& a, u32& b)
	{
		u32 temp = a; a = b; b = temp;
	}
	inline void swap_u16(u16& a, u16& b)
	{
		u16 temp = a; a = b; b = temp;
	}
	inline void swap_u8(u8& a, u8& b)
	{
		u8 temp = a; a = b; b = temp;
	}

	void xuuid::swap(xuuid& uuid)
	{
		swap_u32(_timeLow, uuid._timeLow);
		swap_u16(_timeMid, uuid._timeMid);
		swap_u16(_timeHiAndVersion, uuid._timeHiAndVersion);
		swap_u16(_clockSeq, uuid._clockSeq);
		for (s32 i=0; i<6; ++i)
			swap_u8(_node[i], uuid._node[i]);
	}
	
	bool xuuid::tryParse(const char* uuid)
	{
		s32 str_len = x_strlen(uuid);

		if (str_len < 36)
			return false;

		if (uuid[8] != '-'|| uuid[13] != '-' || uuid[18] != '-' || uuid[23] != '-')
			return false;

		char const* it = uuid;

		_timeLow = 0;
		for (s32 i = 0; i < 8; ++i)
			_timeLow = (_timeLow << 4) | nibble(*it++);
		
		++it;
		_timeMid = 0;
		for (s32 i = 0; i < 4; ++i)
			_timeMid = (_timeMid << 4) | nibble(*it++);
		
		++it;
		_timeHiAndVersion = 0;
		for (s32 i = 0; i < 4; ++i)
			_timeHiAndVersion = (_timeHiAndVersion << 4) | nibble(*it++);

		++it;
		_clockSeq = 0;
		for (s32 i = 0; i < 4; ++i)
			_clockSeq = (_clockSeq << 4) | nibble(*it++);

		++it;
		for (s32 i = 0; i < 6; ++i)
			_node[i] = (nibble(*it++) << 4) | nibble(*it++) ;			

		return true;
	}


	void	xuuid::toString(char* str, s32 max_str_len) const
	{
		if (max_str_len < 36)
			return;

		s32 len = x_sprintf(str, max_str_len, "%08X-%04X-%04X-%04X-", x_va(_timeLow), x_va(_timeMid), x_va(_timeHiAndVersion), x_va(_clockSeq));
		x_sprintf(str+len, max_str_len-len, "%02X%02X%02X%02X-", x_va(_node[0]), x_va(_node[0]), x_va(_node[0]), x_va(_node[0]));
	}

	inline xbyte const* from_bytes(xbyte const* b, u32& value)
	{
		u32 i;
		xbyte* dst = (xbyte*)&i;
		*dst++ = *b++;*dst++ = *b++;*dst++ = *b++;*dst++ = *b++;
		value = i;
		return b + 4;
	}

	inline xbyte const* from_bytes(xbyte const* b, u16& value)
	{
		u16 i;
		xbyte* dst = (xbyte*)&i;
		*dst++ = *b++;*dst++ = *b++;
		value = i;
		return b + 2;
	}

	void xuuid::copyFrom(const xbyte* bytes)
	{
		u32 i32;
		bytes = from_bytes(bytes, i32);
		_timeLow = x_NetworkEndian::swap(i32);
		u16 i16;
		bytes = from_bytes(bytes, i16);
		_timeMid = x_NetworkEndian::swap(i16);
		bytes = from_bytes(bytes, i16);
		_timeHiAndVersion = x_NetworkEndian::swap(i16);
		bytes = from_bytes(bytes, i16);
		_clockSeq = x_NetworkEndian::swap(i16);

		for (s32 i=0; i<6; ++i)
			_node[i]=bytes[i];
	}

	inline xbyte* to_bytes(xbyte* b, u32 value)
	{
		u32 i;
		xbyte* dst = (xbyte*)&i;
		*dst++ = *b++;*dst++ = *b++;*dst++ = *b++;*dst++ = *b++;
		value = i;
		return b + 4;
	}

	inline xbyte* to_bytes(xbyte* b, u16 value)
	{
		u16 i;
		xbyte* dst = (xbyte*)&i;
		*dst++ = *b++;*dst++ = *b++;
		value = i;
		return b + 2;
	}

	void xuuid::copyTo(xbyte* bytes) const
	{
		u32 i32 = x_NetworkEndian::swap(_timeLow);
		bytes   = to_bytes(bytes, i32);
		u32 i16 = x_NetworkEndian::swap(_timeMid);
		bytes   = to_bytes(bytes, i16);
		    i16 = x_NetworkEndian::swap(_timeHiAndVersion);
		bytes   = to_bytes(bytes, i16);
		    i16 = x_NetworkEndian::swap(_clockSeq);
		bytes   = to_bytes(bytes, i16);

		xbyte const* n = (xbyte const*)_node;
		*bytes++ = *n++; *bytes++ = *n++;
		*bytes++ = *n++; *bytes++ = *n++;
		*bytes++ = *n++; *bytes++ = *n++;
	}

	s32 xuuid::variant() const
	{
		s32 v = _clockSeq >> 13;
		if ((v & 6) == 6)
			return v;
		else if (v & 4)
			return 2;
		else
			return 0;
	}


	s32 xuuid::compare(const xuuid& uuid) const
	{
		if (_timeLow != uuid._timeLow) return _timeLow < uuid._timeLow ? -1 : 1;
		if (_timeMid != uuid._timeMid) return _timeMid < uuid._timeMid ? -1 : 1;
		if (_timeHiAndVersion != uuid._timeHiAndVersion) return _timeHiAndVersion < uuid._timeHiAndVersion ? -1 : 1;
		if (_clockSeq != uuid._clockSeq) return _clockSeq < uuid._clockSeq ? -1 : 1;
		for (s32 i = 0; i < sizeof(_node); ++i)
		{
			if (_node[i] < uuid._node[i]) 
				return -1;
			else if (_node[i] > uuid._node[i])
				return 1;	
		}
		return 0;
	}

	char* xuuid::appendHex(char* str, u8 n) 
	{
		const char* digits = "0123456789abcdef";
		str += digits[(n >> 4) & 0xF];
		str += digits[n & 0xF];
		return str;
	}

	char* xuuid::appendHex(char* str, u16 n)
	{
		str = appendHex(str, u8(n >> 8));
		str = appendHex(str, u8(n & 0xFF));
		return str;
	}


	char* xuuid::appendHex(char* str, u32 n)
	{
		str = appendHex(str, u16(n >> 16));
		str = appendHex(str, u16(n & 0xFFFF));
		return str;
	}

	u8 xuuid::nibble(char hex)
	{
		if (hex >= 'a' && hex <= 'f')
			return u8(hex - 'a' + 10);
		else if (hex >= 'A' && hex <= 'F')
			return u8(hex - 'A' + 10);
		else if (hex >= '0' && hex <= '9')
			return u8(hex - '0');
		else
			return u8(0);
	}


	void xuuid::fromNetwork()
	{
		_timeLow = x_NetworkEndian::swap(_timeLow);
		_timeMid = x_NetworkEndian::swap(_timeMid);
		_timeHiAndVersion = x_NetworkEndian::swap(_timeHiAndVersion);
		_clockSeq = x_NetworkEndian::swap(_clockSeq);
	}


	void xuuid::toNetwork()
	{
		_timeLow = x_NetworkEndian::swap(_timeLow);
		_timeMid = x_NetworkEndian::swap(_timeMid);
		_timeHiAndVersion = x_NetworkEndian::swap(_timeHiAndVersion);
		_clockSeq = x_NetworkEndian::swap(_clockSeq);
	}


	namespace
	{
		static xuuid uuidNull;
		static xuuid uuidDNS("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
		static xuuid uuidURI("6ba7b811-9dad-11d1-80b4-00c04fd430c8");
		static xuuid uuidOID("6ba7b812-9dad-11d1-80b4-00c04fd430c8");
		static xuuid uuidX500("6ba7b814-9dad-11d1-80b4-00c04fd430c8");
	}


	const xuuid& xuuid::null()
	{
		return uuidNull;
	}


	const xuuid& xuuid::dns()
	{
		return uuidDNS;
	}


	const xuuid& xuuid::uri()
	{
		return uuidURI;
	}


	const xuuid& xuuid::oid()
	{
		return uuidOID;
	}


	const xuuid& xuuid::x500()
	{
		return uuidX500;
	}


} // namespace Poco
