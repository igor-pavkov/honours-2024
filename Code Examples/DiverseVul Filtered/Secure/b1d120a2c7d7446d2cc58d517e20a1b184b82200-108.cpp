int enc_untrusted_flock(int fd, int operation) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_flock, fd,
                                             TokLinuxFLockOperation(operation));
}