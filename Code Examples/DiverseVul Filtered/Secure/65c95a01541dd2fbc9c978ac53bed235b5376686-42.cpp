static Variant HHVM_METHOD(ZipArchive, getCommentName, const String& name,
                          int64_t flags) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(getCommentName, zipDir);
  if (name.empty()) {
    raise_notice("ZipArchive::getCommentName(): Empty string as source");
    return false;
  }

  int index = zip_name_locate(zipDir->getZip(), name.c_str(), 0);
  if (index != 0) {
    return false;
  }

  int len;
  auto comment = zip_get_file_comment(zipDir->getZip(), index, &len, flags);
  FAIL_IF_INVALID_PTR(comment);

  return String(comment, len, CopyString);
}