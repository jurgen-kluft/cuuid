#ifndef __CUUID_UUID_GENERATOR_H__
#define __CUUID_UUID_GENERATOR_H__
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "cbase/c_runes.h"
#include "cuuid/c_uuid.h"
#include "ctime/c_datetime.h"
#include "crandom/c_random.h"
#include "crandom/c_random_good.h"

namespace ncore
{
	// This class implements a generator for Universal Unique Identifiers,
	// as specified in Appendix A of the DCE 1.1 Remote Procedure
	// Call Specification (http://www.opengroup.org/onlinepubs/9629399/),
	// RFC 2518 (WebDAV), section 6.4.1 and the UUIDs and GUIDs internet
	// draft by Leach/Salz from February, 1998 
	// (http://ftp.ics.uci.edu/pub/ietf/webdav/uuid-guid/draft-leach-uuids-guids-01.txt)
	class uuid_generator
	{
	public:
								uuid_generator();
								/// Creates the uuid_generator.

								~uuid_generator();
								/// Destroys the uuid_generator.

		xuuid					create();
		/// Creates a new time-based xuuid, using the MAC address of
		/// one of the system's ethernet adapters.

		xuuid					createFromName(const xuuid& nsid, const crunes_t& name);
		/// Creates a name-based xuuid.

		xuuid					createFromName(const xuuid& nsid, const crunes_t& name, xdigest_engine& de);
		/// Creates a name-based xuuid, using the given digest engine.

		xuuid					createRandom();
		/// Creates a random xuuid.

		xuuid					createOne();
		/// Tries to create and return a time-based xuuid (see create()), and, 
		/// if that does not work due to the unavailability of a MAC address,
		/// creates and returns a random xuuid (see createRandom()).
		///
		/// The xuuid::version() method can be used to determine the actual kind of
		/// the xuuid generated.

	protected:
		void					init();
		void					timeStamp(datetime_t& dt);

	private:
		bool					_initialized;
		bool					_haveMac;
		nrnd::good_t			_random;
		datetime_t				_lastTime;
		s32						_ticks;
		xmac_t					_mac;

								uuid_generator(const uuid_generator&);
		uuid_generator&		operator = (const uuid_generator&) { return *this; }
	};

} // namespace ncore


#endif // __CUUID_UUID_GENERATOR_H__
