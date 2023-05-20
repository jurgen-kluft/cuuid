#include "ccore/c_debug.h"
#include "cbase/c_runes.h"
#include "cuuid/c_uuid_generator.h"
#include "chash/c_hash.h"

namespace ncore
{
	class xuuid_ : public uuid_t
	{
	public:
		xuuid_(const u8* bytes, Version version)
			: uuid_t(bytes, version)
		{
		}
	};

	uuid_generator::uuid_generator()
		: _initialized(false)
		, _ticks(0)
		, _haveMac(false)
	{
	}

	uuid_generator::~uuid_generator()
	{
	}
	 
	void uuid_generator::init()
	{
		_random.reset();
		if (!_haveMac)
		{
			_mac.clear();
			//xsystem::get_mac_address(_mac);
			_haveMac = true;
		}
	}

	uuid_t uuid_generator::create()
	{
		init();

		datetime_t dt;
		timeStamp(dt);

		u64 tv = dt.toBinary();
		u32 timeLow = u32(tv & 0xFFFFFFFF);
		u16 timeMid = u16((tv >> 32) & 0xFFFF);
		u16 timeHiAndVersion = u16((tv >> 48) & 0x0FFF) + (uuid_t::UUID_TIME_BASED << 12);
		u16 clockSeq = (u16(_random.generate() >> 4) & 0x3FFF) | 0x8000;
		return uuid_t(timeLow, timeMid, timeHiAndVersion, clockSeq, _mac);
	}

	uuid_t uuid_generator::createFromName(const uuid_t& nsid, const crunes_t& name)
	{
		init();

		hash_ctxt_t md5;
		return createFromName(nsid, name, md5);
	}

	uuid_t uuid_generator::createFromName(const uuid_t& nsid, const crunes_t& name, hash_ctxt_t de)
	{
		ASSERT(hash_size(de) == 16);
		init();

		u8 uuid_buffer[16];

		uuid_t netNsid = nsid;
		netNsid.copyTo(uuid_buffer);

		u8 digest[16];

		hash_begin(de);
		hash_update(de, &uuid_buffer[ 0], 4);
		hash_update(de, &uuid_buffer[ 4], 2);
		hash_update(de, &uuid_buffer[ 6], 2);
		hash_update(de, &uuid_buffer[ 8], 2);
		hash_update(de, &uuid_buffer[10], 6);
		hash_update(de, (const u8*)&name.m_ascii.m_bos[name.m_ascii.m_str], name.m_ascii.m_end - name.m_ascii.m_str);
		hash_end(de, digest, 16);

		return xuuid_(digest, uuid_t::UUID_NAME_BASED);
	}


	uuid_t uuid_generator::createRandom()
	{
		init();

		u8 buffer[16];
		nrnd::randBuffer(buffer, 16);

		return xuuid_(buffer, uuid_t::UUID_RANDOM);
	}


	void uuid_generator::timeStamp(datetime_t& dt)
	{
		datetime_t now = datetime_t::sNow();
		for (;;)
		{
			if (now != _lastTime)
			{
				_lastTime = now;
				_ticks = 0;
				break;
			}
			if (_ticks < 100)
			{
				++_ticks;
				break;
			}
			now = datetime_t::sNow();
		}
		u64 tv = datetime_t::sNow().toBinary() + _ticks;
		dt = datetime_t(tv);
	}


	uuid_t uuid_generator::createOne()
	{
		return create();
	}


} // namespace ncore
