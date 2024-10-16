static Variant HHVM_METHOD(ZipArchive, getStream, const String& name) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(getStream, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(getStream, name);

  auto zipStream = req::make<ZipStream>(zipDir->getZip(), name);
  if (zipStream->eof()) {
    return false;
  }
  return Variant(std::move(zipStream));
}