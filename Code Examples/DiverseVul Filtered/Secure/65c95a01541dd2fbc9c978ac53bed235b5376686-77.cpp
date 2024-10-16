static Variant HHVM_FUNCTION(zip_open, const String& filename) {
  FAIL_IF_EMPTY_STRING(zip_open, filename);

  int  err;
  auto z = _zip_open(filename, 0, &err);
  if (z == nullptr) {
    return err;
  }

  return Variant(req::make<ZipDirectory>(z));
}