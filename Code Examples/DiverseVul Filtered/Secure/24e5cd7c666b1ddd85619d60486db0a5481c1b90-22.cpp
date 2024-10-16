filepos_t EbmlElement::Render(IOCallback & output, bool bWithDefault, bool bKeepPosition, bool bForceRender)
{
  assert(bValueIsSet || (bWithDefault && DefaultISset())); // an element is been rendered without a value set !!!
  // it may be a mandatory element without a default value
  if (!bWithDefault && IsDefaultValue()) {
    return 0;
  }
#if defined(LIBEBML_DEBUG)
  uint64 SupposedSize = UpdateSize(bWithDefault, bForceRender);
#endif // LIBEBML_DEBUG
  filepos_t result = RenderHead(output, bForceRender, bWithDefault, bKeepPosition);
  uint64 WrittenSize = RenderData(output, bForceRender, bWithDefault);
#if defined(LIBEBML_DEBUG)
  if (static_cast<int64>(SupposedSize) != (0-1))
    assert(WrittenSize == SupposedSize);
#endif // LIBEBML_DEBUG
  result += WrittenSize;
  return result;
}