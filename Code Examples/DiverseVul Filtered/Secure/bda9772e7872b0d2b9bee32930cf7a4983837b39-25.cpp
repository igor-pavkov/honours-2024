int FromkLinuxSocketType(int input) {
  int kLinux_sock_type = input;
  int output = 0;

  if (kLinux_sock_type & kLinux_SOCK_NONBLOCK) {
    output |= SOCK_NONBLOCK;
    kLinux_sock_type &= ~kLinux_SOCK_NONBLOCK;
  }

  if (kLinux_sock_type & kLinux_SOCK_CLOEXEC) {
    output |= SOCK_CLOEXEC;
    kLinux_sock_type &= ~kLinux_SOCK_CLOEXEC;
  }

  if (!kLinux_sock_type) {  // Only kLinux_SOCK_CLOEXEC or kLinux_SOCK_NONBLOCK
                            // are present.
    return output;
  }

  switch (kLinux_sock_type) {
    case kLinux_SOCK_STREAM:
      output |= SOCK_STREAM;
      break;
    case kLinux_SOCK_DGRAM:
      output |= SOCK_DGRAM;
      break;
    case kLinux_SOCK_SEQPACKET:
      output |= SOCK_SEQPACKET;
      break;
    case kLinux_SOCK_RAW:
      output |= SOCK_RAW;
      break;
    case kLinux_SOCK_RDM:
      output |= SOCK_RDM;
      break;
    case kLinux_SOCK_PACKET:
      output |= SOCK_PACKET;
      break;
    default:
      output = -1;  // Unsupported
  }
  return output;
}