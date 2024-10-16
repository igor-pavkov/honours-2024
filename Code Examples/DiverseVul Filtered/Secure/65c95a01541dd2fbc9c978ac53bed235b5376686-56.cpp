static req::ptr<T> getResource(ObjectData* obj, const char* varName) {
  auto var = obj->o_get(varName, true, s_ZipArchive);
  if (var.getType() == KindOfNull) {
    return nullptr;
  }
  return cast<T>(var);
}