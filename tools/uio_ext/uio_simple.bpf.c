/* SPDX-License-Identifier: GPL-2.0 */
#include <uio/common.bpf.h>

char _license[] SEC("license") = "GPL";

struct user_exit_info uei;

s32 BPF_STRUCT_OPS(simple_init)
{
	bpf_printk("INIT!\n");

	return 0;
}

void BPF_STRUCT_OPS(simple_exit, struct uio_exit_info *ei)
{
	bpf_printk("EXIT!");
	uei_record(&uei, ei);
}

SEC(".struct_ops.link")
struct uio_ext_ops simple_ops = {
	.init			= (void *)simple_init,
	.exit			= (void *)simple_exit,
	.name			= "simple",
};
