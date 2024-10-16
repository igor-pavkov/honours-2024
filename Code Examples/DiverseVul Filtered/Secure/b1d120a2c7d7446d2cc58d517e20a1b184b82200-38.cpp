int enc_untrusted_fstatfs(int fd, struct statfs *statbuf) {
  struct klinux_statfs statfs_kernel;
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_fstatfs, fd, &statfs_kernel);

  if (FromkLinuxStatFs(&statfs_kernel, statbuf)) {
    statbuf->f_flags = FromkLinuxStatFsFlags(statfs_kernel.klinux_f_flags);
  }
  return result;
}