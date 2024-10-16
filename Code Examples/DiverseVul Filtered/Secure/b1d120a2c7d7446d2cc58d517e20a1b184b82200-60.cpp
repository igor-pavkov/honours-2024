int enc_untrusted_getifaddrs(struct ifaddrs **ifap) {
  MessageWriter input;
  MessageReader output;

  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kGetIfAddrsHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_getifaddrs", 3,
                           /*match_exact_params=*/false);

  int result = output.next<int>();
  int klinux_errno = output.next<int>();
  if (result != 0) {
    errno = FromkLinuxErrorNumber(klinux_errno);
    return result;
  }

  if (!asylo::host_call::DeserializeIfAddrs(
          &output, ifap, TrustedPrimitives::BestEffortAbort)) {
    TrustedPrimitives::DebugPuts(
        "enc_untrusted_getifaddrs: Invalid ifaddrs in response.");
    return -1;
  }
  return 0;
}