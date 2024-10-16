void CheckStatusAndParamCount(const asylo::primitives::PrimitiveStatus &status,
                              const MessageReader &output, const char *name,
                              int expected_params, bool match_exact_params) {
  if (!status.ok()) {
    std::string message = absl::StrCat("Host call '", name, "' failed.");
    TrustedPrimitives::BestEffortAbort(message.c_str());
  }

  if (!match_exact_params) {
    if (output.size() < expected_params) {
      std::string message = absl::StrCat(
          "Host call '", name, "': Expected at least ", expected_params,
          " parameters on the MessageReader, found ", output.size());
      TrustedPrimitives::BestEffortAbort(message.c_str());
    }
  } else {
    if (output.size() != expected_params) {
      std::string message = absl::StrCat(
          "Host call '", name, "': Expected ", expected_params,
          " parameters on the MessageReader, found ", output.size());
      TrustedPrimitives::BestEffortAbort(message.c_str());
    }
  }
}