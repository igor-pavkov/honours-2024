static Variant HHVM_FUNCTION(zip_read, const Resource& zip) {
  auto zipDir = cast<ZipDirectory>(zip);

  FAIL_IF_INVALID_ZIPDIRECTORY(zip_read, zipDir);

  return zipDir->nextFile();
}