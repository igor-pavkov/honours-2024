void EbmlMaster::Remove(size_t Index)
{
  if (Index < ElementList.size()) {
    std::vector<EbmlElement *>::iterator Itr = ElementList.begin();
    while (Index-- > 0) {
      ++Itr;
    }

    ElementList.erase(Itr);
  }
}