static Variant HHVM_METHOD(ZipArchive, getStatusString) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(getStatusString, zipDir);

  int zep, sep, len;
  zip_error_get(zipDir->getZip(), &zep, &sep);

  char error_string[128];
  len = zip_error_to_str(error_string, 128, zep, sep);

  return String(error_string, len, CopyString);
}