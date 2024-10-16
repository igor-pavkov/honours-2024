void EbmlElement::Read(EbmlStream & inDataStream, const EbmlSemanticContext & /* Context */, int & /* UpperEltFound */, EbmlElement * & /* FoundElt */, bool /* AllowDummyElt */, ScopeMode ReadFully)
{
  ReadData(inDataStream.I_O(), ReadFully);
}