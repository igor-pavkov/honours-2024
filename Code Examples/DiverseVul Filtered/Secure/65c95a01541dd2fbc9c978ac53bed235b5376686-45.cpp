static Variant HHVM_METHOD(ZipArchive, open, const String& filename,
                           int64_t flags) {
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(open, filename);

  int  err;
  auto z = _zip_open(filename, flags, &err);
  if (z == nullptr) {
    return err;
  }

  auto zipDir = req::make<ZipDirectory>(z);

  setVariable(Object{this_}, "zipDir", Variant(zipDir));
  setVariable(Object{this_}, "filename", filename);

  zip_error_clear(zipDir->getZip());
  return true;
}