static inline void set_attr(map<string, bufferlist>& attrs, const char* key, const char* value)
{
  bufferlist bl;
  encode(value,bl);
  attrs.emplace(key, std::move(bl));
}