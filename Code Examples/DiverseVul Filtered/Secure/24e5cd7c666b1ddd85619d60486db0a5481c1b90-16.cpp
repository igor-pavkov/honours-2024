bool EbmlElement::CompareElements(const EbmlElement *A, const EbmlElement *B)
{
  if (EbmlId(*A) == EbmlId(*B))
    return A->IsSmallerThan(B);
  else
    return false;
}