filepos_t EbmlMaster::ReadData(IOCallback & input, ScopeMode /* ReadFully */)
{
  input.setFilePointer(GetSize(), seek_current);
  return GetSize();
}