void EbmlMaster::Remove(EBML_MASTER_RITERATOR & Itr)
{
  ElementList.erase(Itr.base());
}