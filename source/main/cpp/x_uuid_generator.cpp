#include "xbase/x_debug.h"
#include "xbase/x_string_ascii.h"
#include "xuuid/x_uuid_generator.h"
#include "xhash/x_md5.h"

namespace xcore
{
	class xuuid_ : public xuuid
	{
	public:
		xuuid_(const xbyte* bytes, Version version)
			: xuuid(bytes, version)
		{
		}
	};

	xuuid_generator::xuuid_generator()
		: _ticks(0)
		, _haveNode(false)
	{
	}

	xuuid_generator::~xuuid_generator()
	{
	}

	xuuid xuuid_generator::create()
	{
		if (!_haveNode)
		{
			xsystem::nodeId(_node);
			_haveNode = true;
		}
		xdatetime dt;
		timeStamp(dt);

		u64 tv = dt.toBinary();
		u32 timeLow = u32(tv & 0xFFFFFFFF);
		u16 timeMid = u16((tv >> 32) & 0xFFFF);
		u16 timeHiAndVersion = u16((tv >> 48) & 0x0FFF) + (xuuid::UUID_TIME_BASED << 12);
		u16 clockSeq = (u16(_random.rand() >> 4) & 0x3FFF) | 0x8000;
		return xuuid(timeLow, timeMid, timeHiAndVersion, clockSeq, _node);
	}

	xuuid xuuid_generator::createFromName(const xuuid& nsid, const char* name)
	{
		xdigest_engine_md5 md5;
		return createFromName(nsid, name, md5);
	}

	xuuid xuuid_generator::createFromName(const xuuid& nsid, const char* name, xdigest_engine& de)
	{
		ASSERT(de.length() == 16);

		xbyte uuid_buffer[16];
		xuuid netNsid = nsid;
		netNsid.copyTo(uuid_buffer);

		xbyte digest[16];
		de.reset();
		de.update(&uuid_buffer[ 0], 4);
		de.update(&uuid_buffer[ 4], 2);
		de.update(&uuid_buffer[ 6], 2);
		de.update(&uuid_buffer[ 8], 2);
		de.update(&uuid_buffer[10], 6);
		de.update(name, ascii::size(name));
		de.digest(digest);

		return xuuid_(digest, xuuid::UUID_NAME_BASED);
	}


	xuuid xuuid_generator::createRandom()
	{
		xbyte buffer[16];
		xrandom_utils::randBuffer(_random, buffer, sizeof(buffer));
		return xuuid_(buffer, xuuid::UUID_RANDOM);
	}


	void xuuid_generator::timeStamp(xdatetime& dt)
	{
		xdatetime now = xdatetime::sNow();
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
			now = xdatetime::sNow();
		}
		u64 tv = xdatetime::sNow().toBinary() + _ticks;
		dt = xdatetime(tv);
	}


	xuuid xuuid_generator::createOne()
	{
		return create();
	}


} // namespace xcore
