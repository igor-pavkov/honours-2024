void enc_freeaddrinfo(struct addrinfo *res) {
  struct addrinfo *prev_info = nullptr;
  for (struct addrinfo *info = res; info != nullptr; info = info->ai_next) {
    if (prev_info) free(prev_info);
    if (info->ai_addr) free(info->ai_addr);
    if (info->ai_canonname) free(info->ai_canonname);
    prev_info = info;
  }
  if (prev_info) free(prev_info);
}