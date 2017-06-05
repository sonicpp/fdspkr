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

#define DRIVER_NAME	"fdspkr"
#define DRIVER_VERSION	"0.1"
#define DRIVER_LICENSE	"GPL v2"

static int __init fdspkr_init(void)
{
	printk("FD Speaker init\n");

	return 0;
}

static void __exit fdspkr_exit(void)
{
	printk("FD Speaker exit\n");
}

module_init(fdspkr_init);
module_exit(fdspkr_exit);

MODULE_AUTHOR("Jan Havran <havran.jan@email.cz>");
MODULE_DESCRIPTION("Floppy Disk Speaker");
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE(DRIVER_LICENSE);

