bool EbmlMaster::CheckMandatory() const
{
  assert(Context.GetSize() != 0);

  unsigned int EltIdx;
  for (EltIdx = 0; EltIdx < EBML_CTX_SIZE(Context); EltIdx++) {
    if (EBML_CTX_IDX(Context,EltIdx).IsMandatory()) {
      if (FindElt(EBML_CTX_IDX_INFO(Context,EltIdx)) == NULL) {
#if defined(LIBEBML_DEBUG)
        // you are missing this Mandatory element
//         const char * MissingName = EBML_INFO_NAME(EBML_CTX_IDX_INFO(Context,EltIdx));
#endif // LIBEBML_DEBUG
        return false;
      }
    }
  }

  return true;
}