static String to_full_path(const String& filename) {
  if (filename.charAt(0) == '/') {
    return filename;
  }
  return f_getcwd().toString() + String::FromChar('/') + filename;
}