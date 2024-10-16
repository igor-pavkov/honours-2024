void PacketReader::xfrBlob(string& blob, int length)
{
  if(length) {
    blob.assign(&d_content.at(d_pos), &d_content.at(d_pos + length - 1 ) + 1 );
    
    d_pos += length;
  }
  else 
    blob.clear();
}