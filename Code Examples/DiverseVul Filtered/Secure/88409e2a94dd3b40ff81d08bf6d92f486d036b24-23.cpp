EbmlMaster::EbmlMaster(const EbmlSemanticContext & aContext, bool bSizeIsknown)
 :EbmlElement(0), Context(aContext), bChecksumUsed(bChecksumUsedByDefault)
{
  SetSizeIsFinite(bSizeIsknown);
  SetValueIsSet();
  ProcessMandatory();
}