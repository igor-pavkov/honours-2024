bool EbmlMaster::ProcessMandatory()
{
  if (EBML_CTX_SIZE(Context) == 0)
  {
    return true;
  }

  assert(Context.GetSize() != 0);

  unsigned int EltIdx;
  for (EltIdx = 0; EltIdx < EBML_CTX_SIZE(Context); EltIdx++) {
    if (EBML_CTX_IDX(Context,EltIdx).IsMandatory() && EBML_CTX_IDX(Context,EltIdx).IsUnique()) {
//      assert(EBML_CTX_IDX(Context,EltIdx).Create != NULL);
            PushElement(EBML_SEM_CREATE(EBML_CTX_IDX(Context,EltIdx)));
    }
  }
  return true;
}