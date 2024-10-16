void RefIfNonNull(core::RefCounted* buf) {
  if (buf) buf->Ref();
}