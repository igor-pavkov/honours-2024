int TokLinuxSignalNumber(int input) {
#if defined(SIGRTMIN) && defined(SIGRTMAX)
  if (input >= SIGRTMIN && input <= SIGRTMAX) {
    return kLinux_SIGRTMIN + input - SIGRTMIN;
  }
#endif
  return TokLinuxBaseSignalNumber(input);
}