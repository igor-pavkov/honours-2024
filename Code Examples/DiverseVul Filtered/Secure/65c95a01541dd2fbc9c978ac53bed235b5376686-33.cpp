static bool HHVM_METHOD(ZipArchive, deleteIndex, int64_t index) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(deleteIndex, zipDir);
  FAIL_IF_INVALID_INDEX(index);

  if (zip_delete(zipDir->getZip(), index) != 0) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}