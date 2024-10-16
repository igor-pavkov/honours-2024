int enc_untrusted_uname(struct utsname *buf) {
  struct klinux_utsname klinux_buf {};
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_uname, &klinux_buf);
  if (result != 0) {
    return result;
  }

  if (!FromkLinuxUtsName(&klinux_buf, buf)) {
    TrustedPrimitives::BestEffortAbort(
        "enc_untrusted_uname: Returned an ill-formed utsname.");
  }
  return 0;
}