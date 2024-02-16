
#ifndef _LINUX_UIO_EXT_H
#define _LINUX_UIO_EXT_H

#define UIO_OPS_NAME_LEN	128

enum uio_exit_kind {
	UIO_EXIT_NONE,
	UIO_EXIT_DONE,

	UIO_EXIT_UNREG = 64,	/* BPF unregistration */
	UIO_EXIT_SYSRQ,		/* requested by 'S' sysrq */

	UIO_EXIT_ERROR = 1024,	/* runtime error, error msg contains details */
	UIO_EXIT_ERROR_BPF,	/* ERROR but triggered through scx_bpf_error() */
};

/*
 * uio_exit_info is passed to ops.exit() to describe why the BPF scheduler is
 * being disabled.
 */
struct uio_exit_info {
	/* %UIO_EXIT_* - broad category of the exit reason */
	enum uio_exit_kind	kind;

	/* textual representation of the above */
	const char		*reason;

	/* backtrace if exiting due to an error */
	unsigned long		*bt;
	u32			bt_len;

	/* informational message */
	char			*msg;

	/* debug dump */
	char			*dump;
};

/**
 * struct uio_ext_ops - Operation table for BPF UIO driver implementation
 *
 * Userland can implement an arbitrary UIO driver by implementing and
 * loading operations in this table.
 */
struct uio_ext_ops {
	/**
	 * init - Initialize the BPF IO driver
	 */
	s32 (*init)(void);

	/**
	 * exit - Clean up after the BPF IO driver
	 * @info: Exit info
	 */
	void (*exit)(struct uio_exit_info *info);

	/**
	 * flags - %UIO_OPS_* flags
	 */
	u64 flags;

	/**
	 * name - BPF IO driver's name
	 *
	 * Must be a non-zero valid BPF object name including only isalnum(),
	 * '_' and '.' chars. Shows up in kernel.sched_ext_ops sysctl while the
	 * BPF scheduler is enabled.
	 */
	char name[UIO_OPS_NAME_LEN];
};

#endif /* _LINUX_UIO_EXT_H */
