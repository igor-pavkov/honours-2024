static bool addFile(zip* zipStruct, const char* source, const char* dest,
                    int64_t start = 0, int64_t length = 0) {
  if (!HHVM_FN(is_file)(source)) {
    return false;
  }

  auto zipSource = zip_source_file(zipStruct, source, start, length);
  FAIL_IF_INVALID_PTR(zipSource);

  auto index = zip_name_locate(zipStruct, dest, 0);
  if (index < 0) {
    if (zip_add(zipStruct, dest, zipSource) == -1) {
      zip_source_free(zipSource);
      return false;
    }
  } else {
    if (zip_replace(zipStruct, index, zipSource) == -1) {
      zip_source_free(zipSource);
      return false;
    }
  }

  zip_error_clear(zipStruct);
  return true;
}