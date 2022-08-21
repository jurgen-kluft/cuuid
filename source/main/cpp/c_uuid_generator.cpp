#include "cbase/c_debug.h"
#include "cbase/c_runes.h"
#include "cuuid/c_uuid_generator.h"
#include "chash/c_md5.h"


namespace ncore
{
	class xuuid_ : public xuuid
	{
	public:
		xuuid_(cbuffer_t const& bytes, Version version)
			: xuuid(bytes, version)
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

	xuuid uuid_generator::create()
	{
		init();

		datetime_t dt;
		timeStamp(dt);

		u64 tv = dt.toBinary();
		u32 timeLow = u32(tv & 0xFFFFFFFF);
		u16 timeMid = u16((tv >> 32) & 0xFFFF);
		u16 timeHiAndVersion = u16((tv >> 48) & 0x0FFF) + (xuuid::UUID_TIME_BASED << 12);
		u16 clockSeq = (u16(_random.randU32() >> 4) & 0x3FFF) | 0x8000;
		return xuuid(timeLow, timeMid, timeHiAndVersion, clockSeq, _mac);
	}

	xuuid uuid_generator::createFromName(const xuuid& nsid, const crunes_t& name)
	{
		init();

		xdigest_engine_md5 md5;
		return createFromName(nsid, name, md5);
	}

	xuuid uuid_generator::createFromName(const xuuid& nsid, const crunes_t& name, xdigest_engine& de)
	{
		ASSERT(de.length() == 16);
		init();

		u8s16 uuid_buffer16;
		buffer_t uuid_buffer = uuid_buffer16.buffer();

		xuuid netNsid = nsid;
		netNsid.copyTo(uuid_buffer);
		cbuffer_t uuid_cbuffer = uuid_buffer.cbuffer();

		u8s16 digest;
		buffer_t buffer = digest.buffer();
		cbuffer_t cbuffer = digest.cbuffer();

		de.reset();
		de.update(uuid_cbuffer( 0, 4));
		de.update(uuid_cbuffer( 4, 2));
		de.update(uuid_cbuffer( 6, 2));
		de.update(uuid_cbuffer( 8, 2));
		de.update(uuid_cbuffer(10, 6));
		de.update(name.buffer());
		de.digest(buffer);

		return xuuid_(cbuffer, xuuid::UUID_NAME_BASED);
	}


	xuuid uuid_generator::createRandom()
	{
		init();

		u8s16 buffer16;
		buffer_t buffer = buffer16.buffer();
		cbuffer_t cbuffer = buffer16.cbuffer();
		_random.randBuffer(buffer);
		return xuuid_(cbuffer, xuuid::UUID_RANDOM);
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


	xuuid uuid_generator::createOne()
	{
		return create();
	}


} // namespace ncore
