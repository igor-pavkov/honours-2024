static bool HHVM_METHOD(ZipArchive, unchangeArchive) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(unchangeArchive, zipDir);

  if (zip_unchange_archive(zipDir->getZip()) != 0) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}