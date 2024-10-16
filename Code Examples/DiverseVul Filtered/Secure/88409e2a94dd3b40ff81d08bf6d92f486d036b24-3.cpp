EbmlMaster::EbmlMaster(const EbmlMaster & ElementToClone)
 :EbmlElement(ElementToClone)
 ,ElementList(ElementToClone.ListSize())
 ,Context(ElementToClone.Context)
 ,bChecksumUsed(ElementToClone.bChecksumUsed)
 ,Checksum(ElementToClone.Checksum)
{
  // add a clone of the list
  std::vector<EbmlElement *>::const_iterator Itr = ElementToClone.ElementList.begin();
  std::vector<EbmlElement *>::iterator myItr = ElementList.begin();
  while (Itr != ElementToClone.ElementList.end())
  {
    *myItr = (*Itr)->Clone();
    ++Itr; ++myItr;
  }

}