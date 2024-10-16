static bool HHVM_METHOD(ZipArchive, unchangeIndex, int64_t index) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(unchangeIndex, zipDir);
  FAIL_IF_INVALID_INDEX(index);

  if (zip_unchange(zipDir->getZip(), index) != 0) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}