#include "xuuid/x_uuid.h"
#include "xuuid/x_uuid_generator.h"
#include "xunittest/xunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(xuuid)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(is_null)
		{
			xuuid id;
			CHECK_TRUE(id.isNull());
		}

		UNITTEST_TEST(test)
		{
			xuuid id("A0B1C2D3-AACC-88EE-FF44-5566AADD2200");
			CHECK_FALSE(id.isNull());
		}

		UNITTEST_TEST(generate_1)
		{
			xuuid_generator gen;
			xuuid id = gen.create();
			CHECK_FALSE(id.isNull());
		}
	}
}
UNITTEST_SUITE_END
