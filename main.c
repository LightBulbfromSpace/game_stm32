#include <stdint.h>
#include <utils.h>
#include <stm32f10x.h>

#define min(a,b) ((a) < (b) ? (a) : (b))



int __attribute((noreturn)) main(void) {
	// Enable clock for AFIO
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	// Enable clock for GPIOC
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    // Enable clock for GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	// Enable PC13 push-pull mode
	GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13); //clear cnf bits
	GPIOC->CRH |= GPIO_CRH_MODE13_0; //Max speed = 10Mhz
									 //General purpose Push-pull [00]
	GPIOC->CRH = GPIOC->CRH & ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14) | GPIO_CRH_MODE14_1;
    GPIOC->ODR |= GPIO_ODR_ODR14; //enable PC14 Pull-up (for MID)

    GPIOC->CRH = GPIOC->CRH & ~(GPIO_CRH_CNF15 | GPIO_CRH_MODE15) | GPIO_CRH_MODE15_1;
    GPIOC->ODR |= GPIO_ODR_ODR15; //enable PC15 Pull-up (for UP)

    GPIOA->CRL = GPIOA->CRL & ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0) | GPIO_CRL_MODE0_1;
    GPIOA->ODR |= GPIO_ODR_ODR0; //enable PC14 Pull-up (for DOWN)

	uint32_t ledPeriod = 1000000;
	uint32_t btnPeriod = 10000;
	uint32_t ledPhase = ledPeriod;
	uint32_t btnPhase = btnPeriod;
	_Bool ledEnabled = 1;
	_Bool buttonPrevState = GPIOC->IDR & (1 << 13U);
    while (1) {
		uint32_t tau = min(btnPhase, ledPhase);
		delay_us(tau);
		ledPhase -= tau;
		btnPhase -= tau;
        if (btnPhase == 0) {
            if (GPIOC->IDR & (1 << 14U)) {
                ledPeriod += 10;
            }
            if (GPIOA->IDR & 1) {
                ledPeriod -= 10;
            }
			btnPhase = btnPeriod;
			_Bool btnNewState = !(GPIOC->IDR & GPIO_IDR_IDR14);
			if (!btnNewState && buttonPrevState) {
				ledEnabled = !ledEnabled;
			}
			buttonPrevState = btnNewState;
		}
		if (ledPhase == 0) {
			ledPhase = ledPeriod;
			
			if (ledEnabled) {
				GPIOC->BSRR = ((GPIOC->ODR & GPIO_ODR_ODR13) << 16) | ( ~GPIOC->ODR & GPIO_ODR_ODR13);
			}
			
		}
    }
}
