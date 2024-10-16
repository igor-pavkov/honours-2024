int enc_untrusted_gettimeofday(struct timeval *tv, struct timezone *tz) {
  struct kLinux_timeval ktv;
  TokLinuxtimeval(tv, &ktv);

  // We do not convert timezone to a klinux value since this struct is expected
  // to be identical across enclave boundary. Besides, the use of the timezone
  // structure is obsolete; the tz argument should normally be specified as
  // NULL.
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_gettimeofday, &ktv, tz);
  FromkLinuxtimeval(&ktv, tv);
  return result;
}