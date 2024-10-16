void _cmsAllocTagTypePluginChunk(struct _cmsContext_struct* ctx, 
                                 const struct _cmsContext_struct* src)
{
    if (src != NULL) {
        
        // Duplicate the LIST
        DupTagTypeList(ctx, src, TagTypePlugin);
    }
    else {
        static _cmsTagTypePluginChunkType TagTypePluginChunk = { NULL };
        ctx ->chunks[TagTypePlugin] = _cmsSubAllocDup(ctx ->MemPool, &TagTypePluginChunk, sizeof(_cmsTagTypePluginChunkType));
    }
}