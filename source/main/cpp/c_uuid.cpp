#include "cuuid/c_uuid.h"
#include "cbase/c_va_list.h"
#include "cbase/c_memory.h"
#include "cbase/c_runes.h"
#include "cbase/c_printf.h"
#include "cbase/c_endian.h"

namespace ncore
{
    uuid_t::uuid_t()
        : _timeLow(0)
        , _timeMid(0)
        , _timeHiAndVersion(0)
        , _clockSeq(0)
    {
        _mac.clear();
    }

    uuid_t::uuid_t(const uuid_t& uuid)
        : _timeLow(uuid._timeLow)
        , _timeMid(uuid._timeMid)
        , _timeHiAndVersion(uuid._timeHiAndVersion)
        , _clockSeq(uuid._clockSeq)
    {
        _mac = uuid._mac;
    }

    uuid_t::uuid_t(const char* uuid) { tryParse(uuid); }

    uuid_t::uuid_t(u32 timeLow, u32 timeMid, u32 timeHiAndVersion, u16 clockSeq, mac_t node)
        : _timeLow(timeLow)
        , _timeMid(timeMid)
        , _timeHiAndVersion(timeHiAndVersion)
        , _clockSeq(clockSeq)
    {
        _mac = node;
    }

    uuid_t::uuid_t(const u8* bytes, Version version)
    {
        copyFrom(bytes);

        _timeHiAndVersion &= 0x0FFF;
        _timeHiAndVersion |= (version << 12);
        _clockSeq &= 0x3FFF;
        _clockSeq |= 0x8000;
    }

    uuid_t::~uuid_t() {}

    uuid_t& uuid_t::operator=(const uuid_t& uuid)
    {
        if (&uuid != this)
        {
            _timeLow          = uuid._timeLow;
            _timeMid          = uuid._timeMid;
            _timeHiAndVersion = uuid._timeHiAndVersion;
            _clockSeq         = uuid._clockSeq;
            _mac              = uuid._mac;
        }
        return *this;
    }

    inline void swap_u32(u32& a, u32& b)
    {
        u32 temp = a;
        a        = b;
        b        = temp;
    }
    inline void swap_u16(u16& a, u16& b)
    {
        u16 temp = a;
        a        = b;
        b        = temp;
    }
    inline void swap_u8(u8& a, u8& b)
    {
        u8 temp = a;
        a       = b;
        b       = temp;
    }

    void uuid_t::swap(uuid_t& uuid)
    {
        swap_u32(_timeLow, uuid._timeLow);
        swap_u16(_timeMid, uuid._timeMid);
        swap_u16(_timeHiAndVersion, uuid._timeHiAndVersion);
        swap_u16(_clockSeq, uuid._clockSeq);
        for (s32 i = 0; i < 6; ++i)
            swap_u8(_mac.m_data[i], uuid._mac.m_data[i]);
    }

    bool uuid_t::tryParse(crunes_t const& uuid)
    {
        if (!uuid.is_ascii())
            return false;

        const s32 str_len = uuid.size();
        if (str_len < 36)
            return false;

        const char* str = &uuid.m_ascii.m_bos[uuid.m_ascii.m_str];
        if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-')
            return false;

        uchar const* it = str;

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
        {
            _mac.m_data[i] = (nibble(it[0]) << 4) | nibble(it[1]);
            it += 2;
        }

        return true;
    }

    void uuid_t::toString(runes_t& str) const
    {
        if (str.cap() >= 24)
        {
            crunes_t format("%08X-%04X-%04X-%04X-");
            sprintf(str, format, va_t(_timeLow), va_t(_timeMid), va_t(_timeHiAndVersion), va_t(_clockSeq));
        }
        if (str.cap() >= (24 + 36))
        {
            crunes_t format("%02X%02X%02X%02X%02X%02X");
            sprintf(str, format, va_t(_mac.m_data[0]), va_t(_mac.m_data[1]), va_t(_mac.m_data[2]), va_t(_mac.m_data[3]), va_t(_mac.m_data[4]), va_t(_mac.m_data[5]));
        }
    }

    inline u32 from_bytes(const u8* bytes, u32 i, u32& value)
    {
        u32 dst32;
        u8* dst = (u8*)&dst32;
        *dst++  = bytes[i++];
        *dst++  = bytes[i++];
        *dst++  = bytes[i++];
        *dst++  = bytes[i++];
        value   = dst32;
        return i;
    }

    inline u32 from_bytes(const u8* bytes, u32 i, u16& value)
    {
        u16 dst16;
        u8* dst = (u8*)&dst16;
        *dst++  = bytes[i++];
        *dst++  = bytes[i++];
        value   = dst16;
        return i;
    }

    void uuid_t::copyFrom(const u8* bytes)
    {
        u32 idx = 0;
        u32 i32;
        idx      = from_bytes(bytes, idx, i32);
        _timeLow = nendian_ne::swap(i32);
        u16 i16;
        idx               = from_bytes(bytes, idx, i16);
        _timeMid          = nendian_ne::swap(i16);
        idx               = from_bytes(bytes, idx, i16);
        _timeHiAndVersion = nendian_ne::swap(i16);
        idx               = from_bytes(bytes, idx, i16);
        _clockSeq         = nendian_ne::swap(i16);

        for (s32 i = 0; i < 6; ++i)
            _mac.m_data[i] = bytes[i];
    }

    inline u32 to_bytes(u8* bytes, u32 idx, u32 value)
    {
        bytes[idx++] = (value >> 24) & 0xFF;
        bytes[idx++] = (value >> 16) & 0xFF;
        bytes[idx++] = (value >> 8) & 0xFF;
        bytes[idx++] = (value >> 0) & 0xFF;
        return idx;
    }

    inline u32 to_bytes(u8* bytes, u32 idx, u16 value)
    {
        bytes[idx++] = (value >> 8) & 0xFF;
        bytes[idx++] = (value >> 0) & 0xFF;
        return idx;
    }

    void uuid_t::copyTo(u8* bytes) const
    {
        u32 idx = 0;
        u32 i32 = nendian_ne::swap(_timeLow);
        idx     = to_bytes(bytes, idx, i32);
        u32 i16 = nendian_ne::swap(_timeMid);
        idx     = to_bytes(bytes, idx, i16);
        i16     = nendian_ne::swap(_timeHiAndVersion);
        idx     = to_bytes(bytes, idx, i16);
        i16     = nendian_ne::swap(_clockSeq);
        idx     = to_bytes(bytes, idx, i16);
		for (s32 i = 0; i < 6; ++i)
			bytes[idx++] = _mac.m_data[i];
    }

    s32 uuid_t::variant() const
    {
        s32 v = _clockSeq >> 13;
        if ((v & 6) == 6)
            return v;
        else if (v & 4)
            return 2;
        else
            return 0;
    }

    s32 uuid_t::compare(const uuid_t& uuid) const
    {
        if (_timeLow != uuid._timeLow)
            return _timeLow < uuid._timeLow ? -1 : 1;
        if (_timeMid != uuid._timeMid)
            return _timeMid < uuid._timeMid ? -1 : 1;
        if (_timeHiAndVersion != uuid._timeHiAndVersion)
            return _timeHiAndVersion < uuid._timeHiAndVersion ? -1 : 1;
        if (_clockSeq != uuid._clockSeq)
            return _clockSeq < uuid._clockSeq ? -1 : 1;
		s8 const c = mac_t::compare(_mac, uuid._mac);
        return c;
    }

    char* uuid_t::appendHex(char* str, u8 n)
    {
        const char* digits = "0123456789abcdef";
        str += digits[(n >> 4) & 0xF];
        str += digits[n & 0xF];
        return str;
    }

    char* uuid_t::appendHex(char* str, u16 n)
    {
        str = appendHex(str, u8(n >> 8));
        str = appendHex(str, u8(n & 0xFF));
        return str;
    }

    char* uuid_t::appendHex(char* str, u32 n)
    {
        str = appendHex(str, u16(n >> 16));
        str = appendHex(str, u16(n & 0xFFFF));
        return str;
    }

    u8 uuid_t::nibble(char hex)
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

    void uuid_t::fromNetwork()
    {
        _timeLow          = nendian_ne::swap(_timeLow);
        _timeMid          = nendian_ne::swap(_timeMid);
        _timeHiAndVersion = nendian_ne::swap(_timeHiAndVersion);
        _clockSeq         = nendian_ne::swap(_clockSeq);
    }

    void uuid_t::toNetwork()
    {
        _timeLow          = nendian_ne::swap(_timeLow);
        _timeMid          = nendian_ne::swap(_timeMid);
        _timeHiAndVersion = nendian_ne::swap(_timeHiAndVersion);
        _clockSeq         = nendian_ne::swap(_clockSeq);
    }

    namespace
    {
        static uuid_t uuidNull;
        static uuid_t uuidDNS("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
        static uuid_t uuidURI("6ba7b811-9dad-11d1-80b4-00c04fd430c8");
        static uuid_t uuidOID("6ba7b812-9dad-11d1-80b4-00c04fd430c8");
        static uuid_t uuidX500("6ba7b814-9dad-11d1-80b4-00c04fd430c8");
    }  // namespace

    const uuid_t& uuid_t::null() { return uuidNull; }

    const uuid_t& uuid_t::dns() { return uuidDNS; }

    const uuid_t& uuid_t::uri() { return uuidURI; }

    const uuid_t& uuid_t::oid() { return uuidOID; }

    const uuid_t& uuid_t::x500() { return uuidX500; }

}  // namespace ncore
