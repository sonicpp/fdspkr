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
#include <asm/io.h>

#define DRIVER_NAME	"fdspkr"
#define DRIVER_VERSION	"0.1"
#define DRIVER_LICENSE	"GPL v2"

/* Raspberry PI 2 GPIO (just some registers we will use) */
resource_size_t GPIO_BASE = 0x3f200000;
size_t GPIO_LEN = 0xb4;
#define GPFSEL0		0x00
#define GPFSEL1		0x04
#define GPFSEL2		0x08
#define GPFSEL3		0x0C
#define GPFSEL4		0x10
#define GPFSEL5		0x14
#define GPSET0		0x1C
#define GPSET1		0x20
#define GPCLR0		0x28
#define GPCLR1		0x2C

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
	int ret;

	if ((gpio_base = ioremap(GPIO_BASE, GPIO_LEN)) == NULL)
		return -ENOMEM;

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
	iounmap(gpio_base);

	printk("FD Speaker exit\n");
}

module_init(fdspkr_init);
module_exit(fdspkr_exit);

MODULE_AUTHOR("Jan Havran <havran.jan@email.cz>");
MODULE_DESCRIPTION("Floppy Disk Speaker");
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE(DRIVER_LICENSE);

