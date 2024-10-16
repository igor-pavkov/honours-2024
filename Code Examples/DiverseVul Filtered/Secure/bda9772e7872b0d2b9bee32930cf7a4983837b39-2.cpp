int FromkLinuxOptionName(int level, int klinux_option_name) {
  if (level == IPPROTO_TCP) {
    return FromkLinuxTcpOptionName(klinux_option_name);
  } else if (level == IPPROTO_IPV6) {
    return TokLinuxIpV6OptionName(klinux_option_name);
  } else if (level == SOL_SOCKET) {
    return FromkLinuxSocketOptionName(klinux_option_name);
  }

  return -1;
}