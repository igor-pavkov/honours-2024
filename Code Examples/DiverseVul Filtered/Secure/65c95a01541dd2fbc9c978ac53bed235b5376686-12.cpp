  String read(int64_t len) {
    StringBuffer sb(len);
    auto buf = sb.appendCursor(len);
    auto n   = zip_fread(m_zipFile, buf, len);
    if (n > 0) {
      sb.resize(n);
      return sb.detach();
    }
    return empty_string();
  }