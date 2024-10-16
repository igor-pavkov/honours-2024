int enc_untrusted_statfs(const char *pathname, struct statfs *statbuf) {
  struct klinux_statfs statfs_kernel;
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_statfs, pathname, &statfs_kernel);

  if (FromkLinuxStatFs(&statfs_kernel, statbuf)) {
    statbuf->f_flags = FromkLinuxStatFsFlags(statfs_kernel.klinux_f_flags);
  }
  return result;
}