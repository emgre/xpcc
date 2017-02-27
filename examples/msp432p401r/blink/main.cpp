#include <xpcc/architecture/platform.hpp>

extern "C" void xpcc_gpio_enable(void) {}

int main()
{
	P1->DIR |= BIT0; // P1.0 set as output

	while (1)                               // continuous loop
    {
		P1->OUT ^= BIT0; // Blink P1.0 LED
		for (volatile uint32_t i = 20000; i > 0; i--); // Delay
    }

	return 0;
}
