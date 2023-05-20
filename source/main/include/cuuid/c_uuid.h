#ifndef __CUUID_UUID_H__
#define __CUUID_UUID_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "cbase/c_buffer.h"
#include "cbase/c_runes.h"

namespace ncore
{
    struct mac_t
    {
        void clear()
        {
            for (int i = 0; i < 6; ++i)
                m_data[i] = 0;
        }
        static s8 compare(const mac_t& lhs, const mac_t& rhs)
        {
            for (int i = 0; i < 6; ++i)
            {
                if (lhs.m_data[i] < rhs.m_data[i])
                    return -1;
                else if (lhs.m_data[i] > rhs.m_data[i])
                    return 1;
            }
            return 0;
        }
        u8 m_data[6];
    };

    // A uuid_t is an identifier that is unique across both space and time,
    // with respect to the space of all UUIDs. Since a uuid_t is a fixed
    // size and contains a time field, it is possible for values to
    // rollover (around A.D. 3400, depending on the specific algorithm
    // used). A uuid_t can be used for multiple purposes, from tagging
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
    class uuid_t
    {
    public:
        enum Version
        {
            UUID_TIME_BASED = 0x01,
            UUID_DCE_UID    = 0x02,
            UUID_NAME_BASED = 0x03,
            UUID_RANDOM     = 0x04
        };

        uuid_t();
        /// Creates a nil (all zero) uuid_t.

        uuid_t(const uuid_t& uuid);
        /// Copy constructor.

        explicit uuid_t(const char* uuid);
        /// Parses the uuid_t from a string.

        ~uuid_t();
        /// Destroys the uuid_t.

        uuid_t& operator=(const uuid_t& uuid);
        /// Assignment operator.

        void swap(uuid_t& uuid);
        /// Swaps the uuid_t with another one.

        bool tryParse(crunes_t const& uuid);
        /// Tries to interpret the given string as an uuid_t.
        /// If the uuid_t is syntactically valid, assigns the
        /// members and returns true. Otherwise leaves the
        /// object unchanged and returns false.

        void toString(runes_t& str) const;
        /// Returns a string representation of the uuid_t consisting
        /// of groups of hexadecimal digits separated by hyphens.

        void copyFrom(const u8* buffer);
        /// Copies the uuid_t (16 bytes) from a buffer or byte array.
        /// The uuid_t fields are expected to be
        /// stored in network byte order.
        /// The buffer need not be aligned.

        void copyTo(u8* buffer) const;
        /// Copies the uuid_t to the buffer. The fields
        /// are in network byte order.
        /// The buffer need not be aligned.
        /// There must have room for at least 16 bytes.

        Version version() const;
        /// Returns the version of the uuid_t.

        s32 variant() const;
        /// Returns the variant number of the uuid_t:
        ///   - 0 reserved for NCS backward compatibility
        ///   - 2 the Leach-Salz variant (used by this class)
        ///   - 6 reserved, Microsoft Corporation backward compatibility
        ///   - 7 reserved for future definition

        bool operator==(const uuid_t& uuid) const;
        bool operator!=(const uuid_t& uuid) const;
        bool operator<(const uuid_t& uuid) const;
        bool operator<=(const uuid_t& uuid) const;
        bool operator>(const uuid_t& uuid) const;
        bool operator>=(const uuid_t& uuid) const;

        bool isNull() const;
        /// Returns true iff the uuid_t is nil (in other words,
        /// consists of all zeros).

        static const uuid_t& null();
        /// Returns a null/nil uuid_t.

        static const uuid_t& dns();
        /// Returns the namespace identifier for the DNS namespace.

        static const uuid_t& uri();
        /// Returns the namespace identifier for the URI (former URL) namespace.

        static const uuid_t& oid();
        /// Returns the namespace identifier for the OID namespace.

        static const uuid_t& x500();
        /// Returns the namespace identifier for the X500 namespace.

    protected:
        uuid_t(u32 timeLow, u32 timeMid, u32 timeHiAndVersion, u16 clockSeq, mac_t mac);
        uuid_t(const u8* bytes, Version version);

        s32 compare(const uuid_t& uuid) const;

        static char* appendHex(char* str, u8 n);
        static char* appendHex(char* str, u16 n);
        static char* appendHex(char* str, u32 n);

        static u8 nibble(char hex);

        void fromNetwork();
        void toNetwork();

    private:
        u32   _timeLow;
        u16   _timeMid;
        u16   _timeHiAndVersion;
        u16   _clockSeq;
        mac_t _mac;

        friend class uuid_generator;
    };

    //
    // inlines
    //
    inline bool uuid_t::operator==(const uuid_t& uuid) const { return compare(uuid) == 0; }
    inline bool uuid_t::operator!=(const uuid_t& uuid) const { return compare(uuid) != 0; }
    inline bool uuid_t::operator<(const uuid_t& uuid) const { return compare(uuid) < 0; }
    inline bool uuid_t::operator<=(const uuid_t& uuid) const { return compare(uuid) <= 0; }
    inline bool uuid_t::operator>(const uuid_t& uuid) const { return compare(uuid) > 0; }
    inline bool uuid_t::operator>=(const uuid_t& uuid) const { return compare(uuid) >= 0; }
    inline uuid_t::Version uuid_t::version() const { return Version(_timeHiAndVersion >> 12); }
    inline bool uuid_t::isNull() const { return compare(null()) == 0; }
    inline void swap(uuid_t& u1, uuid_t& u2) { u1.swap(u2); }

}  // namespace ncore

#endif  // __CUUID_UUID_H__