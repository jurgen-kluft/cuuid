#include "cbase/c_base.h"
#include "cbase/c_allocator.h"
#include "cbase/c_console.h"
#include "xtime/x_time.h"

#include "cunittest/cunittest.h"
#include "cunittest/private/ut_ReportAssert.h"

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

		virtual ncore::xbool	HandleAssert(u32& flags, const char* fileName, s32 lineNumber, const char* exprString, const char* messageString)
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
		virtual void*	Allocate(xsize_t size)								{ return mAllocator->allocate((u32)size, 4); }
		virtual void	Deallocate(void* ptr)								{ mAllocator->deallocate(ptr); }
	};

	class TestAllocator : public x_iallocator
	{
		x_iallocator*		mAllocator;
	public:
							TestAllocator(x_iallocator* allocator) : mAllocator(allocator) { }

		virtual const char*	name() const										{ return "cbase unittest test heap allocator"; }

		virtual void*		allocate(xsize_t size, u32 alignment)
		{
			UnitTest::IncNumAllocations();
			return mAllocator->allocate(size, alignment);
		}

		virtual void*		reallocate(void* mem, xsize_t size, u32 alignment)
		{
			if (mem==NULL)
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
			mAllocator->release();
			mAllocator = NULL;
		}
	};
}

ncore::x_iallocator* gTestAllocator = NULL;
ncore::UnitTestAssertHandler gAssertHandler;

bool gRunUnitTest(UnitTest::TestReporter& reporter)
{
#ifdef SPU
	ncore::s32 progSize;
	ncore::s32 stackSize;

	::getProgramAndStackSizeForSPU(&progSize, &stackSize);

	ncore::gSetSPUConfig(progSize, stackSize);
#endif

	cbase::init();
	xtime::x_Init();

#ifdef TARGET_DEBUG
	ncore::x_asserthandler::sRegisterHandler(&gAssertHandler);
#endif

	ncore::x_iallocator* systemAllocator = ncore::x_iallocator::get_default();
	ncore::UnitTestAllocator unittestAllocator( systemAllocator );
	UnitTest::SetAllocator(&unittestAllocator);
	
	ncore::xconsole::write("Configuration: ");
	ncore::xconsole::writeLine(TARGET_FULL_DESCR_STR);

	ncore::TestAllocator testAllocator(systemAllocator);
	gTestAllocator = &testAllocator;

	int r = UNITTEST_SUITE_RUN(reporter, xCoreUnitTest);

	gTestAllocator->release();

	UnitTest::SetAllocator(NULL);

	xtime::x_Exit();
	cbase::exit();
	return r==0;
}

