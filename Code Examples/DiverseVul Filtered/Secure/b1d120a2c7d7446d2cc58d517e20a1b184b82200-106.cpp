int enc_untrusted_ioctl1(int fd, uint64_t request) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_ioctl, fd,
                                             request);
}