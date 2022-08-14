#pragma once

// HID Bootloader takes 4K
#define USER_PROGRAM 0x08001000

#define RTC_BOOTLOADER_FLAG 0x7662 /* Flag whether to jump into bootloader, "vb" */
#define RTC_INSECURE_FLAG 0x4953 /* Flag to indicate qmk that we want to boot into insecure mode, "IS" */

#if defined(TARGET_GENERIC)
#define VIAL_KEYBOARD_UID {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
/* setup for bluepill bootloader without vial */
/* to jump to bootloader, set the BOOT1 jumper to 1. */
#define BL_BANK GPIOB
#define BL_PIN 2
/* end TARGET_GENERIC */

#elif defined(TARGET_VIAL_TEST)
#define VIAL_KEYBOARD_UID {0x6F, 0xC5, 0xEE, 0x60, 0x90, 0x92, 0x53, 0xA4}
/* setup for bluepill vial test */
/* to jump to bootloader, set the BOOT1 jumper to 1. */
#define BL_BANK GPIOB
#define BL_PIN 2
#else
#error Unknown target
#endif
