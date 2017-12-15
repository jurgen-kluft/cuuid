#include "xuuid/x_uuid.h"
#include "xbase/x_va_list.h"
#include "xbase/x_memory_std.h"
#include "xbase/x_string_ascii.h"
#include "xbase/x_endian.h"

namespace xcore
{
	xuuid::xuuid()
		: _timeLow(0)
		, _timeMid(0)
		, _timeHiAndVersion(0)
		, _clockSeq(0)
	{
		_mac.reset(0);
	}

	xuuid::xuuid(const xuuid& uuid)
		: _timeLow(uuid._timeLow)
		, _timeMid(uuid._timeMid)
		, _timeHiAndVersion(uuid._timeHiAndVersion)
		, _clockSeq(uuid._clockSeq)
	{
		_mac = uuid._mac;
	}

	xuuid::xuuid(const char* uuid)
	{
		tryParse(uuid);
	}


	xuuid::xuuid(u32 timeLow, u32 timeMid, u32 timeHiAndVersion, u16 clockSeq, xmac_t node)
		: _timeLow(timeLow)
		, _timeMid(timeMid)
		, _timeHiAndVersion(timeHiAndVersion)
		, _clockSeq(clockSeq)
	{
		_mac = node;
	}

	xuuid::xuuid(xcbuffer const & bytes, Version version)
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
			_mac             = uuid._mac;
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
			swap_u8(_mac[i], uuid._mac[i]);
	}
	
	bool xuuid::tryParse(xcchars const& uuid)
	{
		s32 str_len = uuid.size();

		if (str_len < 36)
			return false;

		if (uuid[8] != '-'|| uuid[13] != '-' || uuid[18] != '-' || uuid[23] != '-')
			return false;

		uchar const* it = uuid.m_str;

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
			_mac[i] = (nibble(*it++) << 4) | nibble(*it++) ;			

		return true;
	}


	void	xuuid::toString(xchars& str) const
	{
		if (str.size() >= 24)
			ascii::sprintf(&str.m_str[0], str.m_eos, "%08X-%04X-%04X-%04X-", NULL, x_va(_timeLow), x_va(_timeMid), x_va(_timeHiAndVersion), x_va(_clockSeq));
		if (str.size() >= 36)
			ascii::sprintf(&str.m_str[24], str.m_eos, "%02X%02X%02X%02X%02X%02X", x_va(_mac[0]), x_va(_mac[1]), x_va(_mac[2]), x_va(_mac[3]), x_va(_mac[4]), x_va(_mac[5]));
	}

	inline u32	from_bytes(xcbuffer const& bytes, u32 i, u32& value)
	{
		u32 dst32;
		xbyte* dst = (xbyte*)&dst32;
		*dst++ = bytes[i++];
		*dst++ = bytes[i++];
		*dst++ = bytes[i++];
		*dst++ = bytes[i++];
		value = dst32;
		return i;
	}

	inline u32	 from_bytes(xcbuffer const& bytes, u32 i, u16& value)
	{
		u16 dst16;
		xbyte* dst = (xbyte*)&dst16;
		*dst++ = bytes[i++];
		*dst++ = bytes[i++];
		value = dst16;
		return i;
	}

	void xuuid::copyFrom(xcbuffer const& bytes)
	{
		u32 idx = 0;
		u32 i32;
		idx = from_bytes(bytes, idx, i32);
		_timeLow = x_NetworkEndian::swap(i32);
		u16 i16;
		idx = from_bytes(bytes, idx, i16);
		_timeMid = x_NetworkEndian::swap(i16);
		idx = from_bytes(bytes, idx, i16);
		_timeHiAndVersion = x_NetworkEndian::swap(i16);
		idx = from_bytes(bytes, idx, i16);
		_clockSeq = x_NetworkEndian::swap(i16);

		for (s32 i=0; i<6; ++i)
			_mac[i] = bytes[i];
	}

	inline u32	to_bytes(xbuffer& bytes, u32 idx, u32 value)
	{
		bytes[idx++] = (value >> 24) & 0xFF;
		bytes[idx++] = (value >> 16) & 0xFF;
		bytes[idx++] = (value >> 8) & 0xFF;
		bytes[idx++] = (value >> 0) & 0xFF;
		return idx;
	}

	inline u32	to_bytes(xbuffer& bytes, u32 idx, u16 value)
	{
		bytes[idx++] = (value >> 8) & 0xFF;
		bytes[idx++] = (value >> 0) & 0xFF;
		return idx;
	}

	void xuuid::copyTo(xbuffer& bytes) const
	{
		u32 idx = 0;
		u32 i32 = x_NetworkEndian::swap(_timeLow);
		idx = to_bytes(bytes, idx, i32);
		u32 i16 = x_NetworkEndian::swap(_timeMid);
		idx = to_bytes(bytes, idx, i16);
		i16 = x_NetworkEndian::swap(_timeHiAndVersion);
		idx = to_bytes(bytes, idx, i16);
		i16 = x_NetworkEndian::swap(_clockSeq);
		idx = to_bytes(bytes, idx, i16);

		bytes.write_at(idx, _mac.cbuffer());
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
		if (_mac != uuid._mac) return _mac < uuid._mac ? -1 : 1;
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
