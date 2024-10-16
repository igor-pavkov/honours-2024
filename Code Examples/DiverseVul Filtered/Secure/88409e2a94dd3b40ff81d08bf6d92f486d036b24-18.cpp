bool EbmlMaster::InsertElement(EbmlElement & element, const EbmlElement & before)
{
  std::vector<EbmlElement *>::iterator Itr = ElementList.begin();
  while (Itr != ElementList.end() && *Itr != &before)
  {
    ++Itr;
  }
  if (Itr == ElementList.end())
    return false;

  ElementList.insert(Itr, &element);
  return true;
}