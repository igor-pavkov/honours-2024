int TokLinuxOptionName(int level, int option_name) {
  if (level == IPPROTO_TCP) {
    return TokLinuxTcpOptionName(option_name);
  } else if (level == IPPROTO_IPV6) {
    return TokLinuxIpV6OptionName(option_name);
  } else if (level == SOL_SOCKET) {
    return TokLinuxSocketOptionName(option_name);
  }

  return -1;
}