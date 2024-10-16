EbmlMaster::~EbmlMaster()
{
  assert(!IsLocked()); // you're trying to delete a locked element !!!

  size_t Index;

  for (Index = 0; Index < ElementList.size(); Index++) {
    if (!(*ElementList[Index]).IsLocked())  {
      delete ElementList[Index];
    }
  }
}