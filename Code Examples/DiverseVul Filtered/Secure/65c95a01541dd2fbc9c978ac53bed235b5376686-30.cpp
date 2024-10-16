static Variant HHVM_METHOD(ZipArchive, getProperty, int64_t property) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  if (zipDir == nullptr) {
    switch (property) {
      case 0:
      case 1:
      case 2:
        return 0;
      case 3:
      case 4:
        return empty_string_variant();
      default:
        return init_null();
    }
  }

  switch (property) {
    case 0:
    case 1:
    {
      int zep, sys;
      zip_error_get(zipDir->getZip(), &zep, &sys);
      if (property == 0) {
        return zep;
      }
      return sys;
    }
    case 2:
    {
      return zip_get_num_files(zipDir->getZip());
    }
    case 3:
    {
      return this_->o_get("filename", true, s_ZipArchive).asCStrRef();
    }
    case 4:
    {
      int len;
      auto comment = zip_get_archive_comment(zipDir->getZip(), &len, 0);
      if (comment == nullptr) {
        return empty_string_variant();
      }
      return String(comment, len, CopyString);
    }
    default:
      return init_null();
  }
}