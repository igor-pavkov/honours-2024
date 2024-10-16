int enc_untrusted_sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
  klinux_sigset_t klinux_set;
  if (!TokLinuxSigset(set, &klinux_set)) {
    errno = EINVAL;
    return -1;
  }

  int klinux_how = TokLinuxSigMaskAction(how);
  if (klinux_how == -1) {
    errno = EINVAL;
    return -1;
  }

  MessageWriter input;
  input.Push<int>(klinux_how);
  input.Push<klinux_sigset_t>(klinux_set);
  MessageReader output;
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kSigprocmaskHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_sigprocmask", 3);

  int result = output.next<int>();
  int klinux_errno = output.next<int>();
  // sigprocmask() returns -1 on failure, with errno set to indicate the cause
  // of the error.
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
    return result;
  }

  klinux_sigset_t klinux_oldset = output.next<klinux_sigset_t>();
  if (oldset != nullptr) {
    if (!FromkLinuxSigset(&klinux_oldset, oldset)) {
      errno = EINVAL;
      return -1;
    }
  }
  return result;
}