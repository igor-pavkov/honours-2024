EbmlElement::EbmlElement(const EbmlElement & ElementToClone)
  :Size(ElementToClone.Size)
  ,DefaultSize(ElementToClone.DefaultSize)
  ,SizeLength(ElementToClone.SizeLength)
  ,bSizeIsFinite(ElementToClone.bSizeIsFinite)
  ,ElementPosition(ElementToClone.ElementPosition)
  ,SizePosition(ElementToClone.SizePosition)
  ,bValueIsSet(ElementToClone.bValueIsSet)
  ,DefaultIsSet(ElementToClone.DefaultIsSet)
  ,bLocked(ElementToClone.bLocked)
{
}