bool CStringCopy(const char *source_buf, char *dest_buf, size_t size) {
  int ret = snprintf(dest_buf, size, "%s", source_buf);
  return ret >= 0 && static_cast<size_t>(ret) < size;
}