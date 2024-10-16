void UnrefIfNonNull(core::RefCounted* buf) {
  if (buf) buf->Unref();
}