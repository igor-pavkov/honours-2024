int FromkLinuxToNewlibWstatus(int input) {
  int info = static_cast<int>(input >> 8 & 0xff) << 8;
  int code = input & 0x7f;

  if (KLINUX_WIFEXITED(input)) {
    code = 0;
  } else if (KLINUX_WIFSTOPPED(input)) {
    code = 0x7f;
  }

  return info + code;
}