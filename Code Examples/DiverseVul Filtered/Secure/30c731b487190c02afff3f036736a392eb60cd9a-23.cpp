static const char *print_uni_str(const Unicode *u, const unsigned uLen) {
  GooString *gstr_buff1 = nullptr;

  delete gstr_buff0;

  if (!uLen) return "";
  gstr_buff0 = GooString::format("{0:c}", (*u < 0x7F ? *u & 0xFF : '?'));
  for (unsigned i = 1; i < uLen; i++) {
    if (u[i] < 0x7F) {
      gstr_buff1 = gstr_buff0->append(u[i] < 0x7F ? static_cast<char>(u[i]) & 0xFF : '?');
      delete gstr_buff0;
      gstr_buff0 = gstr_buff1;
    }
  }

  return gstr_buff0->c_str();
}