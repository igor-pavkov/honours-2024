void CopySockaddr(void *source, socklen_t source_len, void *addr_dest,
                  socklen_t *addrlen_dest) {
  memcpy(addr_dest, source, std::min(*addrlen_dest, source_len));
  *addrlen_dest = source_len;
}