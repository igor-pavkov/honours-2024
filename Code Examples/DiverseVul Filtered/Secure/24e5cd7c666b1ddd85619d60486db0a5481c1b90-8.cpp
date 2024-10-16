bool EbmlElement::IsSmallerThan(const EbmlElement *Cmp) const
{
  return EbmlId(*this) == EbmlId(*Cmp);
}