static Variant HHVM_METHOD(ZipArchive, getCommentIndex, int64_t index,
                          int64_t flags) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(getCommentIndex, zipDir);

  struct zip_stat zipStat;
  if (zip_stat_index(zipDir->getZip(), index, 0, &zipStat) != 0) {
    return false;
  }

  int len;
  auto comment = zip_get_file_comment(zipDir->getZip(), index, &len, flags);
  FAIL_IF_INVALID_PTR(comment);

  return String(comment, len, CopyString);
}