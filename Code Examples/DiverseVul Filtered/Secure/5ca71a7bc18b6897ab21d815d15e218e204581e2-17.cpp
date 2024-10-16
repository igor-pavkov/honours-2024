cmsTagTypeHandler* _cmsGetTagTypeHandler(cmsContext ContextID, cmsTagTypeSignature sig)
{
    _cmsTagTypePluginChunkType* ctx = ( _cmsTagTypePluginChunkType*) _cmsContextGetClientChunk(ContextID, TagTypePlugin);

    return GetHandler(sig, ctx->TagTypes, SupportedTagTypes);
}