EbmlElement *EbmlMaster::AddNewElt(const EbmlCallbacks & Callbacks)
{
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