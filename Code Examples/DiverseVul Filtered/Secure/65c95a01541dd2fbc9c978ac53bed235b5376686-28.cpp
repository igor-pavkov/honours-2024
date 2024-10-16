static Variant HHVM_METHOD(ZipArchive, getNameIndex, int64_t index,
                          int64_t flags) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(getNameIndex, zipDir);
  FAIL_IF_INVALID_INDEX(index);

  auto name = zip_get_name(zipDir->getZip(), index, flags);
  FAIL_IF_INVALID_PTR(name);

  return String(name, CopyString);
}