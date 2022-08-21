#ifndef __XUUID_UUID_H__
#define __XUUID_UUID_H__
#include "cbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "cbase/x_buffer.h"
#include "cbase/x_runes.h"

namespace ncore
{
	struct xmac_t
	{
		u8 m_data[6];
	};

	// A xuuid is an identifier that is unique across both space and time,
	// with respect to the space of all UUIDs. Since a xuuid is a fixed
	// size and contains a time field, it is possible for values to
	// rollover (around A.D. 3400, depending on the specific algorithm
	// used). A xuuid can be used for multiple purposes, from tagging
	// objects with an extremely short lifetime, to reliably identifying
	// very persistent objects across a network.
	// This class implements a Universal Unique Identifier,
	// as specified in Appendix A of the DCE 1.1 Remote Procedure
	// Call Specification (http://www.opengroup.org/onlinepubs/9629399/),
	// RFC 2518 (WebDAV), section 6.4.1 and the UUIDs and GUIDs internet
	// draft by Leach/Salz from February, 1998 
	// (http://ftp.ics.uci.edu/pub/ietf/webdav/uuid-guid/draft-leach-uuids-guids-01.txt)
	// and also
	// http://www.ietf.org/internet-drafts/draft-mealling-uuid-urn-03.txt
	class xuuid
	{
	public:
		enum Version
		{
			UUID_TIME_BASED = 0x01,
			UUID_DCE_UID    = 0x02,
			UUID_NAME_BASED = 0x03,
			UUID_RANDOM     = 0x04
		};

					xuuid();
		/// Creates a nil (all zero) xuuid.

					xuuid(const xuuid& uuid);
		/// Copy constructor.

		explicit	xuuid(const char* uuid);
		/// Parses the xuuid from a string.

					~xuuid();
		/// Destroys the xuuid.

		xuuid&		operator = (const xuuid& uuid);
		/// Assignment operator.

		void		swap(xuuid& uuid);
		/// Swaps the xuuid with another one.	

		bool		tryParse(crunes_t const& uuid);
		/// Tries to interpret the given string as an xuuid.
		/// If the xuuid is syntactically valid, assigns the
		/// members and returns true. Otherwise leaves the 
		/// object unchanged and returns false.

		void		toString(runes_t& str) const;
		/// Returns a string representation of the xuuid consisting
		/// of groups of hexadecimal digits separated by hyphens.

		void		copyFrom(cbuffer_t const& buffer);
		/// Copies the xuuid (16 bytes) from a buffer or byte array.
		/// The xuuid fields are expected to be
		/// stored in network byte order.
		/// The buffer need not be aligned.

		void		copyTo(buffer_t& buffer) const;
		/// Copies the xuuid to the buffer. The fields
		/// are in network byte order.
		/// The buffer need not be aligned.
		/// There must have room for at least 16 bytes.

		Version		version() const;
		/// Returns the version of the xuuid.

		s32			variant() const;
		/// Returns the variant number of the xuuid:
		///   - 0 reserved for NCS backward compatibility
		///   - 2 the Leach-Salz variant (used by this class)
		///   - 6 reserved, Microsoft Corporation backward compatibility
		///   - 7 reserved for future definition

		bool		operator == (const xuuid& uuid) const;
		bool		operator != (const xuuid& uuid) const;
		bool		operator <  (const xuuid& uuid) const;
		bool		operator <= (const xuuid& uuid) const;
		bool		operator >  (const xuuid& uuid) const;
		bool		operator >= (const xuuid& uuid) const;

		bool		isNull() const;
		/// Returns true iff the xuuid is nil (in other words,
		/// consists of all zeros).

		static const xuuid& null();
		/// Returns a null/nil xuuid.

		static const xuuid& dns();
		/// Returns the namespace identifier for the DNS namespace.

		static const xuuid& uri();
		/// Returns the namespace identifier for the URI (former URL) namespace.

		static const xuuid& oid();
		/// Returns the namespace identifier for the OID namespace.

		static const xuuid& x500();
		/// Returns the namespace identifier for the X500 namespace.

	protected:
						xuuid(u32 timeLow, u32 timeMid, u32 timeHiAndVersion, u16 clockSeq, xmac_t mac);
						xuuid(cbuffer_t const& bytes, Version version);
		
		s32				compare(const xuuid& uuid) const;

		static char*	appendHex(char* str, u8 n);
		static char*	appendHex(char* str, u16 n);
		static char*	appendHex(char* str, u32 n);
		
		static u8		nibble(char hex);

		void			fromNetwork();
		void			toNetwork();

	private:
		u32				_timeLow;
		u16				_timeMid;
		u16				_timeHiAndVersion;
		u16				_clockSeq;
		xmac_t			_mac;

		friend class	xuuid_generator;
	};


	//
	// inlines
	//
	inline bool xuuid::operator == (const xuuid& uuid) const
	{
		return compare(uuid) == 0;
	}


	inline bool xuuid::operator != (const xuuid& uuid) const
	{
		return compare(uuid) != 0;
	}


	inline bool xuuid::operator < (const xuuid& uuid) const
	{
		return compare(uuid) < 0;
	}


	inline bool xuuid::operator <= (const xuuid& uuid) const
	{
		return compare(uuid) <= 0;
	}


	inline bool xuuid::operator > (const xuuid& uuid) const
	{
		return compare(uuid) > 0;
	}


	inline bool xuuid::operator >= (const xuuid& uuid) const
	{
		return compare(uuid) >= 0;
	}


	inline xuuid::Version xuuid::version() const
	{
		return Version(_timeHiAndVersion >> 12);
	}


	inline bool xuuid::isNull() const
	{
		return compare(null()) == 0;
	}


	inline void swap(xuuid& u1, xuuid& u2)
	{
		u1.swap(u2);
	}


} // namespace ncore


#endif // __XUUID_UUID_H__
