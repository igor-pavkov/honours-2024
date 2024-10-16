EbmlElement *EbmlMaster::FindFirstElt(const EbmlCallbacks & Callbacks, bool bCreateIfNull)
{
  size_t Index;

  for (Index = 0; Index < ElementList.size(); Index++) {
    if (ElementList[Index] && EbmlId(*(ElementList[Index])) == EBML_INFO_ID(Callbacks))
      return ElementList[Index];
  }

  if (bCreateIfNull) {
    // add the element
    EbmlElement *NewElt = &EBML_INFO_CREATE(Callbacks);
    if (NewElt == NULL)
      return NULL;

    if (!PushElement(*NewElt)) {
      delete NewElt;
      NewElt = NULL;
    }
    return NewElt;
  }

  return NULL;
}