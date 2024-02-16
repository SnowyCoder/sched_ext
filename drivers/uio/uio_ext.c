

/********************************************************************************
 * bpf_struct_ops plumbing.
 */
#include <linux/bpf_verifier.h>
#include <linux/bpf.h>
#include <linux/uio_ext.h>

extern struct btf *btf_vmlinux;

static bool bpf_uio_is_valid_access(int off, int size,
				    enum bpf_access_type type,
				    const struct bpf_prog *prog,
				    struct bpf_insn_access_aux *info)
{
	pr_warn("Is valid access\n");
	// Wtf is bpf_context??

	return btf_ctx_access(off, size, type, prog, info);
}

static int bpf_uio_btf_struct_access(struct bpf_verifier_log *log,
				     const struct bpf_reg_state *reg, int off,
				     int size)
{

	pr_warn("Struct access\n");
	return -EACCES;
}

static const struct bpf_func_proto *
bpf_uio_get_func_proto(enum bpf_func_id func_id, const struct bpf_prog *prog)
{
	// We don't have any custom function for now
	switch (func_id) {
	default:
		return bpf_base_func_proto(func_id);
	}
}

const struct bpf_verifier_ops bpf_uio_verifier_ops = {
	.get_func_proto = bpf_uio_get_func_proto,
	.is_valid_access = bpf_uio_is_valid_access,
	.btf_struct_access = bpf_uio_btf_struct_access,
};

static int bpf_uio_init_member(const struct btf_type *t,
			       const struct btf_member *member,
			       void *kdata, const void *udata)
{
	const struct uio_ext_ops *uops = udata;
	struct uio_ext_ops *ops = kdata;
	u32 moff = __btf_member_bit_offset(t, member) / 8;
	int ret;

	switch (moff) {
	case offsetof(struct uio_ext_ops, flags):
		ops->flags = *(u64 *)(udata + moff);
		return 1;
	case offsetof(struct uio_ext_ops, name):
		ret = bpf_obj_name_cpy(ops->name, uops->name,
				       sizeof(ops->name));
		if (ret < 0)
			return ret;
		if (ret == 0)
			return -EINVAL;
		return 1;
	}

	return 0;
}

static int bpf_uio_check_member(const struct btf_type *t,
				const struct btf_member *member,
				const struct bpf_prog *prog)
{
	pr_warn("Check member\n");
	return 0;
}

static int bpf_uio_reg(void *kdata)
{
	pr_warn("Reg\n");
    return 0;
}

static void bpf_uio_unreg(void *kdata)
{
	pr_warn("Unreg\n");
}

static int bpf_uio_init(struct btf *btf)
{

	pr_warn("Init\n");
	return 0;
}

static int bpf_uio_update(void *kdata, void *old_kdata)
{

	pr_warn("Update\n");
	/*
	 * uio_ext does not support updating the actively-loaded BPF
	 * driver, as registering a BPF driver can always fail if the
	 * driver returns an error code for e.g. ops.init(),
	 * ops.init_task(), etc. Similarly, we can always race with
	 * unregistration happening elsewhere, such as with sysrq.
	 */
	return -EOPNOTSUPP;
}

static int bpf_uio_validate(void *kdata)
{
	pr_warn("Validate\n");
	return 0;
}

/* "extern" to avoid sparse warning, only used in this file */
extern struct bpf_struct_ops bpf_uio_ext_ops;

struct bpf_struct_ops bpf_uio_ext_ops = {
	.verifier_ops = &bpf_uio_verifier_ops,
	.reg = bpf_uio_reg,
	.unreg = bpf_uio_unreg,
	.check_member = bpf_uio_check_member,
	.init_member = bpf_uio_init_member,
	.init = bpf_uio_init,
	.update = bpf_uio_update,
	.validate = bpf_uio_validate,
	.name = "uio_ext_ops",
};


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Lorenzo Rossi");
