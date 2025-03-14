/* Code based on https://github.com/rogerclarkmelbourne/STM32duino-bootloader/blob/master/hardware.c */

#include <inttypes.h>
#include <stm32f1xx.h>

#include "config.h"

int checkUserCode(void) {
    uint32_t sp = *(volatile uint32_t *) USER_PROGRAM;

    if ((sp & 0x2FFE0000) == 0x20000000) {
        return 0;
    } else {
        return 1;
    }
}

int checkAndClearBootloaderFlag(void) {
    int flag = 0;

    // Enable clocks for the backup domain registers
    RCC->APB1ENR |= (RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);

    if (BKP->DR10 == RTC_BOOTLOADER_FLAG) {
        flag = 1;
    }

    /* clear the flag unless it is set to RTC_INSECURE_FLAG (as we want qmk to read that one) */
    if (BKP->DR10 != RTC_INSECURE_FLAG) {
        // Disable backup register write protection
        PWR->CR |= PWR_CR_DBP;
        // store value in pBK DR10
        BKP->DR10 = 0;
        // Re-enable backup register write protection
        PWR->CR &=~ PWR_CR_DBP;
    }

    // Disable clocks
    RCC->APB1ENR &= ~(RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);

    return flag;
}

void setInsecureFlag(void) {
    // Enable clocks for the backup domain registers
    RCC->APB1ENR |= (RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);

    // Disable backup register write protection
    PWR->CR |= PWR_CR_DBP;
    // store value in pBK DR10
    BKP->DR10 = RTC_INSECURE_FLAG;
    // Re-enable backup register write protection
    PWR->CR &=~ PWR_CR_DBP;

    // Disable clocks
    RCC->APB1ENR &= ~(RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);
}

int checkKbMatrix(void) {
    /* read and check col_pin */
    /* if it's high, we need to jump to bootloader */
    return BL_BANK->IDR & (1U << BL_PIN);
}

#define CR_INPUT_PU_PD      0x08
#define CR_OUTPUT_PP        0x01
#define CR_SHITF(pin) ((pin - 8*(pin>7))<<2)
#define GPIO_CR(port,pin) (__IO uint32_t*)((uint32_t)port + (0x04*(pin>7)))

// Used to create the control register masking pattern, when setting control register mode.
static uint32_t crMask(uint32_t pin)
{
    uint32_t mask;
    if (pin >= 8)
        pin -= 8;
    mask = 0x0F << (pin<<2);
    return ~mask;
}

void setupGPIO(void) {
    uint32_t cr;

    /* Enable All GPIO channels (A to G) */
    RCC->APB2ENR |= 0b111111100;

    /* To read/write the AFIO_MAPR register, the AFIO clock should first be enabled. */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    /* Disable JTAG to release PB3, PB4, PA15 */
    AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    /* Set bootloader pin as pullup input */
    cr = *GPIO_CR(BL_BANK,BL_PIN) & crMask(BL_PIN);
    *GPIO_CR(BL_BANK,BL_PIN) = cr | CR_INPUT_PU_PD << CR_SHITF(BL_PIN);
    BL_BANK->BSRR = (1U << BL_PIN);
}
