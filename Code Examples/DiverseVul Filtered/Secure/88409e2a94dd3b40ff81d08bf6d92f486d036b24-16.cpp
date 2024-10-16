EbmlElement *EbmlMaster::FindFirstElt(const EbmlCallbacks & Callbacks) const
{
  size_t Index;

  for (Index = 0; Index < ElementList.size(); Index++) {
    if (EbmlId(*(ElementList[Index])) == EBML_INFO_ID(Callbacks))
      return ElementList[Index];
  }

  return NULL;
}