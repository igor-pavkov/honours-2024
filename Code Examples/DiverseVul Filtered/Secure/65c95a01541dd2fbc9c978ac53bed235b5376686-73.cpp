static Variant HHVM_METHOD(ZipArchive, getFromIndex, int64_t index,
                           int64_t length, int64_t flags) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(getFromIndex, zipDir);
  FAIL_IF_INVALID_INDEX(index);

  if (length < 0) {
    return empty_string_variant();
  }

  struct zip_stat zipStat;
  if (zip_stat_index(zipDir->getZip(), index, 0, &zipStat) != 0) {
    return false;
  }

  if (zipStat.size < 1) {
    return empty_string_variant();
  }

  auto zipFile = zip_fopen_index(zipDir->getZip(), index, flags);
  FAIL_IF_INVALID_PTR(zipFile);

  if (length == 0)  {
    length = zipStat.size;
  }

  StringBuffer sb(length);
  auto buf = sb.appendCursor(length);
  auto n   = zip_fread(zipFile, buf, length);
  if (n > 0) {
    sb.resize(n);
    return sb.detach();
  }
  return empty_string_variant();
}