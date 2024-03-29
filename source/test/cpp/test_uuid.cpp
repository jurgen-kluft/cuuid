#include "cuuid/c_uuid.h"
#include "cuuid/c_uuid_generator.h"
#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(uuid)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(is_null)
		{
			uuid_t id;
			CHECK_TRUE(id.isNull());
		}

		UNITTEST_TEST(test)
		{
			uuid_t id("A0B1C2D3-AACC-88EE-FF44-5566AADD2200");
			CHECK_FALSE(id.isNull());
		}

		UNITTEST_TEST(generate_1)
		{
			uuid_generator gen;
			uuid_t id = gen.create();
			CHECK_FALSE(id.isNull());
		}
	}
}
UNITTEST_SUITE_END
