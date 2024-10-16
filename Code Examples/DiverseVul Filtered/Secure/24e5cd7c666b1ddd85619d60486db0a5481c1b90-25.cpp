EbmlElement *EbmlElement::CreateElementUsingContext(const EbmlId & aID, const EbmlSemanticContext & Context,
                                                    int & LowLevel, bool IsGlobalContext, bool bAllowDummy, unsigned int MaxLowerLevel)
{
  unsigned int ContextIndex;
  EbmlElement *Result = NULL;

  // elements at the current level
  for (ContextIndex = 0; ContextIndex < EBML_CTX_SIZE(Context); ContextIndex++) {
    if (aID == EBML_CTX_IDX_ID(Context,ContextIndex)) {
      return &EBML_SEM_CREATE(EBML_CTX_IDX(Context,ContextIndex));
    }
  }

  // global elements
  assert(Context.GetGlobalContext != NULL); // global should always exist, at least the EBML ones
  const EbmlSemanticContext & tstContext = Context.GetGlobalContext();
  if (tstContext != Context) {
    LowLevel--;
    MaxLowerLevel--;
    // recursive is good, but be carefull...
    Result = CreateElementUsingContext(aID, tstContext, LowLevel, true, bAllowDummy, MaxLowerLevel);
    if (Result != NULL) {
      return Result;
    }
    LowLevel++;
    MaxLowerLevel++;
  } else {
    return NULL;
  }

  // parent elements
  if (EBML_CTX_MASTER(Context) != NULL && aID == EBML_INFO_ID(*EBML_CTX_MASTER(Context))) {
    LowLevel++; // already one level up (same as context)
    return &EBML_INFO_CREATE(*EBML_CTX_MASTER(Context));
  }

  // check wether it's not part of an upper context
  if (EBML_CTX_PARENT(Context) != NULL) {
    LowLevel++;
    MaxLowerLevel++;
    return CreateElementUsingContext(aID, *EBML_CTX_PARENT(Context), LowLevel, IsGlobalContext, bAllowDummy, MaxLowerLevel);
  }

  if (!IsGlobalContext && bAllowDummy) {
    LowLevel = 0;
    Result = new (std::nothrow) EbmlDummy(aID);
  }

  return Result;
}