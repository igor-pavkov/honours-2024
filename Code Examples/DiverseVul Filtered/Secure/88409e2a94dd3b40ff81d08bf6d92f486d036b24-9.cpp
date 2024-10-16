bool EbmlMaster::InsertElement(EbmlElement & element, size_t position)
{
  std::vector<EbmlElement *>::iterator Itr = ElementList.begin();
  while (Itr != ElementList.end() && position--)
  {
    ++Itr;
  }
  if ((Itr == ElementList.end()) && position)
    return false;

  ElementList.insert(Itr, &element);
  return true;
}