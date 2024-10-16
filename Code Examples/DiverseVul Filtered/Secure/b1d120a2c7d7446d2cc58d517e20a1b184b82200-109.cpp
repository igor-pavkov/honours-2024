int enc_untrusted_pipe2(int pipefd[2], int flags) {
  if (flags & ~(O_CLOEXEC | O_DIRECT | O_NONBLOCK)) {
    errno = EINVAL;
    return -1;
  }

  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_pipe2, pipefd, TokLinuxFileStatusFlag(flags));
}