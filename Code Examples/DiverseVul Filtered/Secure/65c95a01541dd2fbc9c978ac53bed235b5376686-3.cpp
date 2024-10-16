static bool HHVM_METHOD(ZipArchive, close) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(close, zipDir);

  bool ret = zipDir->close();

  setVariable(Object{this_}, "zipDir", null_resource);

  return ret;
}