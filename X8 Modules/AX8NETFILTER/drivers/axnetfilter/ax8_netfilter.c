/*
 * Author: AnDyX <AnDyX at xda-developers>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  This module contains code from kernel required to initialise netfilter.  
 */
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <net/net_namespace.h>
#include <linux/skbuff.h>

#include "ax8netfilter.h"

#define AX_MODULE_NAME 			"ax8netfilter"
#define AX_MODULE_VER			"v001b"

#define X8
#define X10M_
#define X10MP_

// patch offsets

#ifdef X8
#define DEVICE_NAME			"X8"
#define OFS_KALLSYMS_LOOKUP_NAME	0xC00B0654			// kallsyms_lookup_name
#endif

#ifdef X10M
#define DEVICE_NAME			"X10 mini"
#define OFS_KALLSYMS_LOOKUP_NAME	0xC00AF6D8			// kallsyms_lookup_name
#endif

#ifdef X10MP
#define DEVICE_NAME			"X10 mini pro"
#define OFS_KALLSYMS_LOOKUP_NAME	0xC00B09F0			// kallsyms_lookup_name
#endif

struct ax8netfilter_net init_ax8netfilter_net;
EXPORT_SYMBOL(init_ax8netfilter_net);

// for get proc address
typedef unsigned long (*kallsyms_lookup_name_type)(const char *name);
static kallsyms_lookup_name_type kallsyms_lookup_name_ax;

static void patch(unsigned int addr, unsigned int value) {
	*(unsigned int*)addr = value;
}

// patch to an jump obcode
static void patch_to_jmp(unsigned int addr, void * func) {
	int write_value;
	// calculate the offset
	write_value = ((((unsigned int)func - 8 - addr) >> 2) & 0x00FFFFFF);
	// add the unconditional jump opcode
	write_value |= 0xEA000000;
	// and patch it
	patch(addr, write_value);
}

struct cfg_value_map {
	const char* name;
	void * new_func;
	int required;
};

static const struct cfg_value_map func_mapping_table[] = {
	{NULL, 0, 0},
};

static int hijack_functions(int check_only)
{	
	const struct cfg_value_map * t = func_mapping_table;
	int func;
	int ret = 1;

	while (t->name) {
		func = kallsyms_lookup_name_ax(t->name);
		if(check_only)
		{
			if(!func && t->required)
			{
				printk(KERN_ERR AX_MODULE_NAME ": Pointer to %s not found!!!\n", t->name);	
				ret = 0;
			}
		}
		else
			if(func)
			{
				patch_to_jmp(func, t->new_func);
				printk(KERN_ERR AX_MODULE_NAME ": Function %s hijacked\n", t->name);	
			}
			else
				ret = 0;
		t++;
	}

	return ret;
}

/* inits of netfilters */
int ipv4_netfilter_init(void);
int ipv6_netfilter_init(void);
void netfilter_init(void);

/********* Module methods *************/
// init module
static int __init ax8netfilter_init(void)
{
	int ret = -1;
	//struct sk_buff aaa;
	//long a;


	//a = ((long)&aaa.nfct) - ((long)&aaa.cb);

	printk(KERN_INFO AX_MODULE_NAME ": module " AX_MODULE_VER " for " DEVICE_NAME " device loaded\n");


	//printk(KERN_INFO AX_MODULE_NAME ": place of nfct %d!!!\n", (int)a);

	// our 'GetProcAddress' :D
	kallsyms_lookup_name_ax = (void*) OFS_KALLSYMS_LOOKUP_NAME;

	if(!hijack_functions(1))
	{
		goto eof;
	}

	ax8netfilter_skbuff_head_cache = (void*) kallsyms_lookup_name_ax("skbuff_head_cache");
	if(!ax8netfilter_skbuff_head_cache)
	{
		printk(KERN_INFO AX_MODULE_NAME ": skbuff_head_cache missing!!!\n");
		goto eof;
	}

	ax8netfilter_skbuff_fclone_cache = (void*) kallsyms_lookup_name_ax("skbuff_fclone_cache");
	if(!ax8netfilter_skbuff_fclone_cache)
	{
		printk(KERN_INFO AX_MODULE_NAME ": skbuff_fclone_cache missing!!!\n");
		goto eof;
	}

	netfilter_init();

	ret = ipv4_netfilter_init();

	if( ret < 0 )
	{
		printk(KERN_INFO AX_MODULE_NAME ": ipv4_netfilter_init() failed\n");
		goto eof;
	}

	ret = ipv6_netfilter_init();

	if( ret < 0 )
	{
		printk(KERN_INFO AX_MODULE_NAME ": ipv6_netfilter_init() failed\n");
		goto eof;
	}

	hijack_functions(0);

	eof:
	return ret;
}

module_init(ax8netfilter_init);

MODULE_AUTHOR("AnDyX@xda-developers.com");
MODULE_DESCRIPTION("Netfilter module for " DEVICE_NAME );
MODULE_LICENSE("GPL");
