#include <xpcc/architecture/platform.hpp>

#define XPCC_CAN_MODULE_NAME "can"
#define XPCC_IOBUFFER_MODULE_NAME "iobuffer"
#define XPCC_UART_MODULE_NAME "uart"

using namespace Board;

extern "C" void xpcc_abandon(const char * module,
							 const char * location,
							 const char * failure,
							 uintptr_t context)
{
	XPCC_LOG_ERROR << "Assertion '"
			<< module << "." << location << "." << failure
			<< "' @ " << (void *) context
			<< " failed! Abandoning..." << xpcc::endl;

	LedGreen::setOutput();
	while(1) {
		LedBlue::set();
		xpcc::delayMilliseconds(20);
		LedBlue::reset();
		xpcc::delayMilliseconds(180);
	}
}

static xpcc::Abandonment
test_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * /* failure */,
					   uintptr_t /* context */)
{
	if (strcmp(module, XPCC_IOBUFFER_MODULE_NAME) == 0)
		return xpcc::Abandonment::Ignore;
	return xpcc::Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(test_assertion_handler);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	xpcc_assert(true, XPCC_CAN_MODULE_NAME, "init", "timeout");

	xpcc_assert_debug(false, XPCC_IOBUFFER_MODULE_NAME, "tx", "full");

	xpcc_assert(false, XPCC_UART_MODULE_NAME, "init", "mode");

	while(1)
	{
		LedRed::toggle();
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
