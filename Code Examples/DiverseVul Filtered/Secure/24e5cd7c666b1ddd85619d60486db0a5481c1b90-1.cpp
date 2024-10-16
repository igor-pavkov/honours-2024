EbmlCallbacks::EbmlCallbacks(EbmlElement & (*Creator)(), const EbmlId & aGlobalId, const char * aDebugName, const EbmlSemanticContext & aContext)
  :Create(Creator)
  ,GlobalId(aGlobalId)
  ,DebugName(aDebugName)
  ,Context(aContext)
{
  assert((Create!=NULL) || !strcmp(aDebugName, "DummyElement"));
}