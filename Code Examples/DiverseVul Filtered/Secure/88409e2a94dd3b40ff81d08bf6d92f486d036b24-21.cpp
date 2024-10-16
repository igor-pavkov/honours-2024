bool EbmlMaster::PushElement(EbmlElement & element)
{
  ElementList.push_back(&element);
  return true;
}