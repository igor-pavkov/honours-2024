void ByteVector::detach()
{
  if(d->count() > 1) {
    d->deref();
    d = new ByteVectorPrivate(d->data);
  }
}