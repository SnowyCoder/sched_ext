UIO EXAMPLE SCHEDULERS
============================

Completely copied from SCHED_EXT.

## Toolchain dependencies

1. clang >= 16.0.0

The schedulers are BPF programs, and therefore must be compiled with clang. gcc
is actively working on adding a BPF backend compiler as well, but are still
missing some features such as BTF type tags which are necessary for using
kptrs.

2. pahole >= 1.25

You may need pahole in order to generate BTF from DWARF.

3. rust >= 1.70.0

Rust schedulers uses features present in the rust toolchain >= 1.70.0. You
should be able to use the stable build from rustup, but if that doesn't
work, try using the rustup nightly build.

There are other requirements as well, such as make, but these are the main /
non-trivial ones.

## Compiling the kernel

In order to run a sched_ext scheduler, you'll have to run a kernel compiled
with the patches in this repository, and with a minimum set of necessary
Kconfig options:

```
CONFIG_BPF=y
CONFIG_SCHED_CLASS_EXT=y
CONFIG_BPF_SYSCALL=y
CONFIG_BPF_JIT=y
CONFIG_DEBUG_INFO_BTF=y
```

It's also recommended that you also include the following Kconfig options:

```
CONFIG_BPF_JIT_ALWAYS_ON=y
CONFIG_BPF_JIT_DEFAULT_ON=y
CONFIG_PAHOLE_HAS_SPLIT_BTF=y
CONFIG_PAHOLE_HAS_BTF_TAG=y
```

There is a `Kconfig` file in this directory whose contents you can append to
your local `.config` file, as long as there are no conflicts with any existing
options in the file.

## Getting a vmlinux.h file

You may notice that most of the example schedulers include a "vmlinux.h" file.
This is a large, auto-generated header file that contains all of the types
defined in some vmlinux binary that was compiled with
[BTF](https://docs.kernel.org/bpf/btf.html) (i.e. with the BTF-related Kconfig
options specified above).

The header file is created using `bpftool`, by passing it a vmlinux binary
compiled with BTF as follows:

```bash
$ bpftool btf dump file /path/to/vmlinux format c > vmlinux.h
```

The scheduler build system will generate this vmlinux.h file as part of the
scheduler build pipeline. It looks for a vmlinux file in the following
dependency order:

1. If the O= environment variable is defined, at `$O/vmlinux`
2. If the KBUILD_OUTPUT= environment variable is defined, at
   `$KBUILD_OUTPUT/vmlinux`
3. At `../../vmlinux` (i.e. at the root of the kernel tree where you're
   compiling the schedulers)
3. `/sys/kernel/btf/vmlinux`
4. `/boot/vmlinux-$(uname -r)`

In other words, if you have compiled a kernel in your local repo, its vmlinux
file will be used to generate vmlinux.h. Otherwise, it will be the vmlinux of
the kernel you're currently running on. This means that if you're running on a
kernel with sched_ext support, you may not need to compile a local kernel at
all.

--------------------------------------------------------------------------------

# Troubleshooting

There are a number of common issues that you may run into when building the
schedulers. We'll go over some of the common ones here.

## Build Failures

### Stale kernel build / incomplete vmlinux.h file

As described above, you'll need a `vmlinux.h` file that was generated from a
vmlinux built with BTF, and with sched_ext support enabled. If you don't,
you'll see errors such as the following which indicate that a type being
referenced in a scheduler is unknown:

```
/path/to/sched_ext/tools/sched_ext/user_exit_info.h:25:23: note: forward declaration of 'struct uio_exit_info'

const struct uio_exit_info *ei)

^
```

In order to resolve this, please follow the steps above in
[Getting a vmlinux.h file](#getting-a-vmlinuxh-file) in order to ensure your
schedulers are using a vmlinux.h file that includes the requisite types.

## Misc

### llvm: [OFF]

You may see the following output when building the schedulers:

```
Auto-detecting system features:
...                         clang-bpf-co-re: [ on  ]
...                                    llvm: [ OFF ]
...                                  libcap: [ on  ]
...                                  libbfd: [ on  ]
```

Seeing `llvm: [ OFF ]` here is not an issue. You can safely ignore.
