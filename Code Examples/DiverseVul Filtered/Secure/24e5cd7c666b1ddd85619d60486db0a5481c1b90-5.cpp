uint64 EbmlElement::VoidMe(IOCallback & output, bool bWithDefault)
{
  if (ElementPosition == 0) {
    return 0; // the element has not been written
  }

  EbmlVoid Dummy;
  return Dummy.Overwrite(*this, output, bWithDefault);
}