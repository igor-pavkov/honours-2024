uint64 EbmlMaster::UpdateSize(bool bWithDefault, bool bForceRender)
{
  SetSize_(0);

  if (!IsFiniteSize())
    return (0-1);

  if (!bForceRender) {
    assert(CheckMandatory());
    }

  size_t Index;

  for (Index = 0; Index < ElementList.size(); Index++) {
    if (!bWithDefault && (ElementList[Index])->IsDefaultValue())
      continue;
    (ElementList[Index])->UpdateSize(bWithDefault, bForceRender);
    uint64 SizeToAdd = (ElementList[Index])->ElementSize(bWithDefault);
#if defined(LIBEBML_DEBUG)
    if (static_cast<int64>(SizeToAdd) == (0-1))
      return (0-1);
#endif // LIBEBML_DEBUG
    SetSize_(GetSize() + SizeToAdd);
  }
  if (bChecksumUsed) {
    SetSize_(GetSize() + Checksum.ElementSize());
  }

  return GetSize();
}