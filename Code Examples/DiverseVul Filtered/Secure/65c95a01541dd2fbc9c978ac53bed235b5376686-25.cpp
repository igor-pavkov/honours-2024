static Variant HHVM_METHOD(ZipArchive, getArchiveComment, int64_t flags) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(getArchiveComment, zipDir);

  int len;
  auto comment = zip_get_archive_comment(zipDir->getZip(), &len, flags);
  FAIL_IF_INVALID_PTR(comment);

  return String(comment, len, CopyString);
}