filepos_t EbmlElement::RenderHead(IOCallback & output, bool bForceRender, bool bWithDefault, bool bKeepPosition)
{
  if (EBML_ID_LENGTH((const EbmlId&)*this) <= 0 || EBML_ID_LENGTH((const EbmlId&)*this) > 4)
    return 0;

  UpdateSize(bWithDefault, bForceRender);

  return MakeRenderHead(output, bKeepPosition);
}