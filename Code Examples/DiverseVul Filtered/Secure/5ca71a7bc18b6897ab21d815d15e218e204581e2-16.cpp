void _cmsAllocMPETypePluginChunk(struct _cmsContext_struct* ctx, 
                               const struct _cmsContext_struct* src)
{
    if (src != NULL) {
        
        // Duplicate the LIST
        DupTagTypeList(ctx, src, MPEPlugin);
    }
    else {
        static _cmsTagTypePluginChunkType TagTypePluginChunk = { NULL };
        ctx ->chunks[MPEPlugin] = _cmsSubAllocDup(ctx ->MemPool, &TagTypePluginChunk, sizeof(_cmsTagTypePluginChunkType));
    }

}