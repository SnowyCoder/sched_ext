/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 Meta Platforms, Inc. and affiliates.
 * Copyright (c) 2022 Tejun Heo <tj@kernel.org>
 * Copyright (c) 2022 David Vernet <dvernet@meta.com>
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>
#include <bpf/bpf.h>
#include <uio/common.h>
#include "uio_simple.bpf.skel.h"

const char help_fmt[] =
"A simple uio_ext driver.\n"
"\n"
"See the top-level comment in .bpf.c for more details.\n"
"\n"
"Usage: %s [-f] [-p]\n"
"\n"
"  -h            Display this help and exit\n";

static volatile int exit_req;

static void sigint_handler(int simple)
{
	exit_req = 1;
}

static int __base_pr(enum libbpf_print_level level, const char *format,
		     va_list args)
{
	return vfprintf(stderr, format, args);
}

int main(int argc, char **argv)
{
	struct uio_simple *skel;
	struct bpf_link *link;
	__u32 opt;

	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigint_handler);

	libbpf_set_strict_mode(LIBBPF_STRICT_ALL);
	libbpf_set_print(__base_pr);

	skel = uio_simple__open();
	UIO_BUG_ON(!skel, "Failed to open skel");

	while ((opt = getopt(argc, argv, "fph")) != -1) {
		switch (opt) {
		/*case 'f':
			skel->rodata->fifo_sched = true;
			break;
		case 'p':
			skel->rodata->switch_partial = true;
			break;*/
		default:
			fprintf(stderr, help_fmt, basename(argv[0]));
			return opt != 'h';
		}
	}

	UIO_BUG_ON(uio_simple__load(skel), "Failed to load skel");

	link = bpf_map__attach_struct_ops(skel->maps.simple_ops);
	UIO_BUG_ON(!link, "Failed to attach struct_ops");

	while (!exit_req && !uei_exited(&skel->bss->uei)) {
		sleep(1);
	}

	bpf_link__destroy(link);
	uei_print(&skel->bss->uei);
	uio_simple__destroy(skel);
	return 0;
}
