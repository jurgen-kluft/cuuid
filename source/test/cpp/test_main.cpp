#include "xbase/x_base.h"
#include "xbase/x_allocator.h"
#include "xbase/x_console.h"
#include "xbase/x_context.h"

#include "xtime/x_time.h"

#include "xunittest/xunittest.h"
#include "xunittest/private/ut_ReportAssert.h"

UNITTEST_SUITE_LIST(xCoreUnitTest);


UNITTEST_SUITE_DECLARE(xCoreUnitTest, xuuid);


namespace ncore
{
	// Our own assert handler
	class UnitTestAssertHandler : public ncore::x_asserthandler
	{
	public:
		UnitTestAssertHandler()
		{
			NumberOfAsserts = 0;
		}

		virtual bool	HandleAssert(u32& flags, const char* fileName, s32 lineNumber, const char* exprString, const char* messageString)
		{
			UnitTest::reportAssert(exprString, fileName, lineNumber);
			NumberOfAsserts++;
			return false;
		}


		ncore::s32		NumberOfAsserts;
	};

	class UnitTestAllocator : public UnitTest::Allocator
	{
		ncore::x_iallocator*	mAllocator;
	public:
						UnitTestAllocator(ncore::x_iallocator* allocator)	{ mAllocator = allocator; }
		virtual void*	Allocate(uint_t size)								{ return mAllocator->allocate((u32)size, 4); }
		virtual void	Deallocate(void* ptr)								{ mAllocator->deallocate(ptr); }
	};

	class TestAllocator : public x_iallocator
	{
		x_iallocator*		mAllocator;
	public:
							TestAllocator(x_iallocator* allocator) : mAllocator(allocator) { }

		virtual const char*	name() const										{ return "xbase unittest test heap allocator"; }

		virtual void*		allocate(uint_t size, u32 alignment)
		{
			UnitTest::IncNumAllocations();
			return mAllocator->allocate(size, alignment);
		}

		virtual void*		reallocate(void* mem, uint_t size, u32 alignment)
		{
			if (mem==nullptr)
				return allocate(size, alignment);
			else 
				return mAllocator->reallocate(mem, size, alignment);
		}

		virtual void		deallocate(void* mem)
		{
			UnitTest::DecNumAllocations();
			mAllocator->deallocate(mem);
		}

		virtual void		release()
		{
			mAllocator = nullptr;
		}
	};
}

ncore::alloc_t *gTestAllocator = nullptr;
ncore::UnitTestAssertHandler gAssertHandler;

bool gRunUnitTest(UnitTest::TestReporter& reporter)
{
	xbase::init();

#ifdef TARGET_DEBUG
	ncore::context_t::set_assert_handler(&gAssertHandler);
#endif
	ncore::console->write("Configuration: ");
	ncore::console->setColor(ncore::console_t::YELLOW);
	ncore::console->writeLine(TARGET_FULL_DESCR_STR);
	ncore::console->setColor(ncore::console_t::NORMAL);

	ncore::alloc_t* systemAllocator = ncore::context_t::system_alloc();
	ncore::UnitTestAllocator unittestAllocator( systemAllocator );
	UnitTest::SetAllocator(&unittestAllocator);

	ncore::TestAllocator testAllocator(systemAllocator);
	gTestAllocator = &testAllocator;
	ncore::context_t::set_system_alloc(&testAllocator);

	int r = UNITTEST_SUITE_RUN(reporter, xCoreUnitTest);
	if (UnitTest::GetNumAllocations()!=0)
	{
		reporter.reportFailure(__FILE__, __LINE__, "xunittest", "memory leaks detected!");
		r = -1;
	}

	gTestAllocator->release();

	UnitTest::SetAllocator(nullptr);
	ncore::context_t::set_system_alloc(systemAllocator);

	xbase::exit();
	return r==0;
}

