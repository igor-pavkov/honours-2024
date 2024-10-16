Local<Value> Http2Settings::Pack(
    Environment* env,
    size_t count,
    const nghttp2_settings_entry* entries) {
  EscapableHandleScope scope(env->isolate());
  const size_t size = count * 6;
  AllocatedBuffer buffer = AllocatedBuffer::AllocateManaged(env, size);
  ssize_t ret =
      nghttp2_pack_settings_payload(
          reinterpret_cast<uint8_t*>(buffer.data()),
          size,
          entries,
          count);
  Local<Value> buf = Undefined(env->isolate());
  if (ret >= 0) buf = buffer.ToBuffer().ToLocalChecked();
  return scope.Escape(buf);
}