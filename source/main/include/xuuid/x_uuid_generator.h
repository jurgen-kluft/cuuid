#ifndef __XUUID_UUID_GENERATOR_H__
#define __XUUID_UUID_GENERATOR_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

//==============================================================================
// INCLUDES
//==============================================================================
#include "xuuid/x_uuid.h"
#include "xtime/x_datetime.h"
#include "xrandom/x_random.h"
#include "xrandom/x_random_good.h"

namespace xcore
{
	class xdigest_engine;

	// This class implements a generator for Universal Unique Identifiers,
	// as specified in Appendix A of the DCE 1.1 Remote Procedure
	// Call Specification (http://www.opengroup.org/onlinepubs/9629399/),
	// RFC 2518 (WebDAV), section 6.4.1 and the UUIDs and GUIDs internet
	// draft by Leach/Salz from February, 1998 
	// (http://ftp.ics.uci.edu/pub/ietf/webdav/uuid-guid/draft-leach-uuids-guids-01.txt)
	class xuuid_generator
	{
	public:
								xuuid_generator();
								/// Creates the xuuid_generator.

								~xuuid_generator();
								/// Destroys the xuuid_generator.

		xuuid					create();
		/// Creates a new time-based xuuid, using the MAC address of
		/// one of the system's ethernet adapters.

		xuuid					createFromName(const xuuid& nsid, const char* name);
		/// Creates a name-based xuuid.

		xuuid					createFromName(const xuuid& nsid, const char* name, xdigest_engine& de);
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
		void					timeStamp(xdatetime& dt);
		void					getNode();

	private:
		xrng_good				_random;
		xdatetime				_lastTime;
		s32						_ticks;
		xnode_t					_node;
		bool					_haveNode;

		xuuid_generator(const xuuid_generator&);
		xuuid_generator& operator = (const xuuid_generator&);
	};

} // namespace xcore


#endif // __XUUID_UUID_GENERATOR_H__
