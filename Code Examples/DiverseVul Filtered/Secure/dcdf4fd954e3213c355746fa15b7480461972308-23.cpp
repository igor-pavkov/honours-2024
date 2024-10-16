ByteVector::~ByteVector()
{
  if(d->deref())
    delete d;
}