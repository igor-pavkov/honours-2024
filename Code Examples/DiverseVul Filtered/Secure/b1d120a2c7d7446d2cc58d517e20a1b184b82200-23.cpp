int enc_untrusted_getaddrinfo(const char *node, const char *service,
                              const struct addrinfo *hints,
                              struct addrinfo **res) {
  MessageWriter input;
  input.PushByReference(Extent{node, (node != nullptr) ? strlen(node) + 1 : 0});
  input.PushByReference(
      Extent{service, (service != nullptr) ? strlen(service) + 1 : 0});
  if (hints != nullptr) {
    input.Push<int>(TokLinuxAddressInfoFlag(hints->ai_flags));
    input.Push<int>(TokLinuxAfFamily(hints->ai_family));
    input.Push<int>(TokLinuxSocketType(hints->ai_socktype));
    input.Push<int>(hints->ai_protocol);
  }

  MessageReader output;
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kGetAddrInfoHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_getaddrinfo", 3,
                           /*match_exact_params=*/false);

  int klinux_ret = output.next<int>();
  int klinux_errno = output.next<int>();

  int ret = FromkLinuxAddressInfoError(klinux_ret);
  if (ret != 0) {
    if (ret == EAI_SYSTEM) {
      errno = FromkLinuxErrorNumber(klinux_errno);
    }
    return ret;
  }

  if (!asylo::host_call::DeserializeAddrinfo(
          &output, res, TrustedPrimitives::BestEffortAbort)) {
    TrustedPrimitives::DebugPuts(
        "enc_untrusted_getaddrinfo: Invalid addrinfo in response.");
    return -1;
  }
  return 0;
}