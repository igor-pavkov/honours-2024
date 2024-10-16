static Variant HHVM_FUNCTION(zip_close, const Resource& zip) {
  auto zipDir = cast<ZipDirectory>(zip);

  FAIL_IF_INVALID_ZIPDIRECTORY(zip_close, zipDir);

  zipDir->close();

  return init_null();
}