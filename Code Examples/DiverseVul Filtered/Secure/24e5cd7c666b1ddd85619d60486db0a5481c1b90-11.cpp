const EbmlSemantic & EbmlSemanticContext::GetSemantic(size_t i) const
{
  assert(i<Size);
  if (i<Size)
    return MyTable[i];
  else
    return *(EbmlSemantic*)NULL;
}