/*
 * fdspkr.c - Floppy Disk Speaker driver for Linux
 *
 * Copyright (C) 2017 Jan Havran <havran.jan@email.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <asm/io.h>

#define DRIVER_NAME	"fdspkr"
#define DRIVER_VERSION	"0.1"
#define DRIVER_LICENSE	"GPL v2"

/* Raspberry PI 2 GPIO (just some registers we will use) */
resource_size_t GPIO_BASE = 0x3f200000;
size_t GPIO_LEN = 0xb4;

/* GPIO registers */
#define GPFSEL0		0x00
#define GPSET0		0x1C
#define GPCLR0		0x28

#define GPFSELn(pin)	(GPFSEL0 + ((pin) / 10) * 4)
#define GPFSELs(pin)	(((pin) % 10) * 3)
#define GPSETn(pin)	(GPSET0 + ((pin) / 32) * 4)
#define GPSETs(pin)	((pin) % 32)
#define GPCLRn(pin)	(GPCLR0 + ((pin) / 32) * 4)
#define GPCLRs(pin)	GPSETs(pin)

#define GPIO_DIR	23
#define GPIO_STEP	24

volatile void __iomem *gpio_base;

struct input_dev *fdspkr_input;

static int fdspkr_open(struct input_dev *dev)
{
	printk("Open\n");

	return 0;
}

static void fdspkr_close(struct input_dev *dev)
{
	printk("Close\n");
}

static int fdspkr_event(struct input_dev *dev, unsigned int type,
unsigned int code, int value)
{
	printk("type %u, code %u, value %d\n", type, code, value);

	return 0;
}

static int __init fdspkr_init(void)
{
	int ret, i;
	u32 reg;

	/* Prepare GPIO */
	if ((gpio_base = ioremap(GPIO_BASE, GPIO_LEN)) == NULL)
		return -ENOMEM;

	reg = ioread32(gpio_base + GPFSELn(GPIO_DIR));
	reg |= (1 << GPFSELs(GPIO_DIR));
	iowrite32(reg, gpio_base + GPFSELn(GPIO_DIR));

	reg = ioread32(gpio_base + GPFSELn(GPIO_STEP));
	reg |= (1 << GPFSELs(GPIO_STEP));
	iowrite32(reg, gpio_base + GPFSELn(GPIO_STEP));

	/* Move motor to make sure we have enought space
	 * to move there and back again. */
	iowrite32(1 << GPCLRs(GPIO_DIR), gpio_base + GPCLRn(GPIO_DIR));
	for (i = 0; i < 4; i++) { /* Move motor in one direction...*/
		iowrite32(1 << GPSETs(GPIO_STEP),
			gpio_base + GPSETn(GPIO_STEP));
		iowrite32(1 << GPCLRs(GPIO_STEP),
			gpio_base + GPCLRn(GPIO_STEP));
		udelay(10ul);
	}

	iowrite32(1 << GPSETs(GPIO_DIR), gpio_base + GPSETn(GPIO_DIR));
	for (i = 0; i < 2; i++) { /* ...and move back a little */
		iowrite32(1 << GPSETs(GPIO_STEP),
			gpio_base + GPSETn(GPIO_STEP));
		iowrite32(1 << GPCLRs(GPIO_STEP),
			gpio_base + GPCLRn(GPIO_STEP));
		udelay(10ul);
	}

	/* Prepare input device */
	fdspkr_input = input_allocate_device();
	if (!fdspkr_input) {
		iounmap(gpio_base);
		return -ENOMEM;
	}

	fdspkr_input->name = "Floppy Disk Speaker";
	fdspkr_input->evbit[0] = BIT(EV_SND);
	fdspkr_input->sndbit[0] = BIT(SND_BELL) | BIT(SND_TONE);
	fdspkr_input->open = fdspkr_open;
	fdspkr_input->close = fdspkr_close;
	fdspkr_input->event = fdspkr_event;

	ret = input_register_device(fdspkr_input);
	if (ret) {
		input_free_device(fdspkr_input);
		iounmap(gpio_base);
		return ret;
	}

	printk("FD Speaker init\n");

	return 0;
}

static void __exit fdspkr_exit(void)
{
	input_unregister_device(fdspkr_input);
	iowrite32(1 << GPCLRs(GPIO_DIR), gpio_base + GPCLRn(GPIO_DIR));
	iowrite32(1 << GPCLRs(GPIO_STEP), gpio_base + GPCLRn(GPIO_STEP));
	iounmap(gpio_base);

	printk("FD Speaker exit\n");
}

module_init(fdspkr_init);
module_exit(fdspkr_exit);

MODULE_AUTHOR("Jan Havran <havran.jan@email.cz>");
MODULE_DESCRIPTION("Floppy Disk Speaker");
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE(DRIVER_LICENSE);

