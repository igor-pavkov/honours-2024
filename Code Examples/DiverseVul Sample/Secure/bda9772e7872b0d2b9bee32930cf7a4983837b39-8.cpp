int TokLinuxSocketType(int input) {
  int sock_type = input;
  int output = 0;

  if (sock_type & SOCK_NONBLOCK) {
    output |= kLinux_SOCK_NONBLOCK;
    sock_type &= ~SOCK_NONBLOCK;
  }

  if (sock_type & SOCK_CLOEXEC) {
    output |= kLinux_SOCK_CLOEXEC;
    sock_type &= ~SOCK_CLOEXEC;
  }

  if (!sock_type) {  // Only SOCK_CLOEXEC or SOCK_NONBLOCK are present.
    return output;
  }

  switch (sock_type) {
    case SOCK_STREAM:
      output |= kLinux_SOCK_STREAM;
      break;
    case SOCK_DGRAM:
      output |= kLinux_SOCK_DGRAM;
      break;
    case SOCK_SEQPACKET:
      output |= kLinux_SOCK_SEQPACKET;
      break;
    case SOCK_RAW:
      output |= kLinux_SOCK_RAW;
      break;
    case SOCK_RDM:
      output |= kLinux_SOCK_RDM;
      break;
    case SOCK_PACKET:
      output |= kLinux_SOCK_PACKET;
      break;
    default:
      output = -1;  // Unsupported
  }
  return output;
}