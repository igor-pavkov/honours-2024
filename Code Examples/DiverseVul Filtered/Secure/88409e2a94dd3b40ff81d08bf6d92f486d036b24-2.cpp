void EbmlMaster::Sort()
{
  std::sort(ElementList.begin(), ElementList.end(), EbmlElement::CompareElements);
}