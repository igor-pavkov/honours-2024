static zip* _zip_open(const String& filename, int _flags, int* zep) {
  return zip_open(to_full_path(filename).c_str(), _flags, zep);
}