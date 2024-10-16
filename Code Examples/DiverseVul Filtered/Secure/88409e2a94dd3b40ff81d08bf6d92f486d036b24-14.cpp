EbmlElement *EbmlMaster::FindElt(const EbmlCallbacks & Callbacks) const
{
  size_t Index;

  for (Index = 0; Index < ElementList.size(); Index++) {
    EbmlElement * tmp = ElementList[Index];
    if (EbmlId(*tmp) == EBML_INFO_ID(Callbacks))
      return tmp;
  }

  return NULL;
}