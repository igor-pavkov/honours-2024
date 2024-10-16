static unsigned int nodeHash(i64 iNode){
  return ((unsigned)iNode) % HASHSIZE;
}