#include "xuuid/x_uuid.h"
#include "xunittest/xunittest.h"

using namespace xcore;

UNITTEST_SUITE_BEGIN(xuuid)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(test)
        {
			xuuid id("A0B1C2D3-AACC-88EE-FF44-5566AADD2200");
        }
	}
}
UNITTEST_SUITE_END
