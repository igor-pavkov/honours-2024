EbmlElement *EbmlMaster::FindNextElt(const EbmlElement & PastElt, bool bCreateIfNull)
{
  size_t Index;

  for (Index = 0; Index < ElementList.size(); Index++) {
    if ((ElementList[Index]) == &PastElt) {
      // found past element, new one is :
      Index++;
      break;
    }
  }

  while (Index < ElementList.size()) {
    if ((EbmlId)PastElt == (EbmlId)(*ElementList[Index]))
      break;
    Index++;
  }

  if (Index != ElementList.size())
    return ElementList[Index];

  if (bCreateIfNull) {
    // add the element
    EbmlElement *NewElt = &(PastElt.CreateElement());
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