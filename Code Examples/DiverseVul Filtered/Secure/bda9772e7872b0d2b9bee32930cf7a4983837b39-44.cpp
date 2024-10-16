bool TokLinuxSockAddr(const struct sockaddr *input, socklen_t input_len,
                      struct klinux_sockaddr *output, socklen_t *output_len,
                      void (*abort_handler)(const char *)) {
  if (!input || input_len == 0 || !output || !output_len) {
    return false;
  }

  if (input->sa_family == AF_UNIX) {
    struct klinux_sockaddr_un klinux_sock_un;
    if (!SockaddrTokLinuxSockaddrUn(input, input_len, &klinux_sock_un)) {
      return false;
    }
    CopySockaddr(&klinux_sock_un, sizeof(klinux_sock_un), output, output_len);
  } else if (input->sa_family == AF_INET) {
    struct klinux_sockaddr_in klinux_sock_in;
    if (!SockaddrTokLinuxSockaddrIn(input, input_len, &klinux_sock_in)) {
      return false;
    }
    CopySockaddr(&klinux_sock_in, sizeof(klinux_sock_in), output, output_len);
  } else if (input->sa_family == AF_INET6) {
    struct klinux_sockaddr_in6 klinux_sock_in6;
    if (!SockaddrTokLinuxSockaddrIn6(input, input_len, &klinux_sock_in6)) {
      return false;
    }
    CopySockaddr(&klinux_sock_in6, sizeof(klinux_sock_in6), output, output_len);
  } else if (input->sa_family == AF_UNSPEC) {
    output = nullptr;
    *output_len = 0;
  } else {
    if (abort_handler != nullptr) {
      std::string message =
          absl::StrCat("Unsupported AF family encountered: ", input->sa_family);
      abort_handler(message.c_str());
    } else {
      abort();
    }
  }
  return true;
}