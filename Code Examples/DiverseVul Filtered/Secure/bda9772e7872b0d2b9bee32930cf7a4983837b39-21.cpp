int FromkLinuxSignalNumber(int input) {
#if defined(SIGRTMIN) && defined(SIGRTMAX)
  if (input >= kLinux_SIGRTMIN && input <= kLinux_SIGRTMAX) {
    return SIGRTMIN + input - kLinux_SIGRTMIN;
  }
#endif
  return FromkLinuxBaseSignalNumber(input);
}