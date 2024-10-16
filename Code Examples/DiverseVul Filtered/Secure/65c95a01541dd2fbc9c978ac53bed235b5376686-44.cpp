static bool HHVM_METHOD(ZipArchive, extractTo, const String& destination,
                        const Variant& entries) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(extractTo, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(extractTo, destination);

  auto fileCount = zip_get_num_files(zipDir->getZip());
  if (fileCount == -1) {
    raise_warning("Illegal archive");
    return false;
  }

  std::string to(destination.c_str());
  if (to[to.size() - 1] != '/') {
    to.push_back('/');
  }

  if (!HHVM_FN(is_dir)(to) && !HHVM_FN(mkdir)(to)) {
    return false;
  }

  char buf[1024];
  auto toSize = to.size();

  if (entries.isString()) {
    // extract only this file
    if (!extractFileTo(zipDir->getZip(), entries.asCStrRef().c_str(),
                       to, buf, sizeof(buf))) {
      return false;
    }
  } else if (entries.isArray() && entries.asCArrRef().size() != 0) {
    // extract ones in the array
    for (ArrayIter it(entries.asCArrRef()); it; ++it) {
      auto var = it.second();
      if (!var.isString() || !extractFileTo(zipDir->getZip(),
                                            var.asCStrRef().c_str(),
                                            to, buf, sizeof(buf))) {
        return false;
      }
      to.resize(toSize);
    }
  } else {
    // extract all files
    for (decltype(fileCount) index = 0; index < fileCount; ++index) {
      auto file = zip_get_name(zipDir->getZip(), index, ZIP_FL_UNCHANGED);
      if (!extractFileTo(zipDir->getZip(), file, to, buf, sizeof(buf))) {
        return false;
      }
      to.resize(toSize);
    }
  }

  zip_error_clear(zipDir->getZip());
  return true;
}