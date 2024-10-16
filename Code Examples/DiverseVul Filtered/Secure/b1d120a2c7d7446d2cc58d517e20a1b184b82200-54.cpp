int enc_untrusted_sched_getaffinity(pid_t pid, size_t cpusetsize,
                                    cpu_set_t *mask) {
  klinux_cpu_set_t klinux_mask{};
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_sched_getaffinity, pid,
      static_cast<uint64_t>(cpusetsize), &klinux_mask);
  // On success, the raw getaffinity syscall returns the size of the cpumask_t
  // data type, To mimic the glibc behavior, we return 0 on success and -1 on
  // failure. See https://linux.die.net/man/2/sched_getaffinity, under "notes".
  if (result < 0) {
    return -1;
  }
  if (!FromkLinuxCpuSet(&klinux_mask, mask)) {
    errno = EFAULT;
    return -1;
  }
  return 0;
}