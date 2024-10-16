int decode_attr_bl_single_value(map<string, bufferlist>& attrs, const char *attr_name, T *result, T def_val)
{
  map<string, bufferlist>::iterator iter = attrs.find(attr_name);
  if (iter == attrs.end()) {
    *result = def_val;
    return 0;
  }
  bufferlist& bl = iter->second;
  if (bl.length() == 0) {
    *result = def_val;
    return 0;
  }
  auto bliter = bl.cbegin();
  try {
    decode(*result, bliter);
  } catch (buffer::error& err) {
    return -EIO;
  }
  return 0;
}