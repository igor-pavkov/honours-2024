static bool HHVM_METHOD(ZipArchive, unchangeAll) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(unchangeAll, zipDir);

  if (zip_unchange_all(zipDir->getZip()) != 0) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}