static Variant setVariable(const Object& obj, const char* varName, const Variant& varValue) {
  return obj->o_set(varName, varValue, s_ZipArchive);
}