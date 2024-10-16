void _cmsAllocTagPluginChunk(struct _cmsContext_struct* ctx, 
                                 const struct _cmsContext_struct* src)
{
    if (src != NULL) {

        DupTagList(ctx, src);
    }
    else {
        static _cmsTagPluginChunkType TagPluginChunk = { NULL };
        ctx ->chunks[TagPlugin] = _cmsSubAllocDup(ctx ->MemPool, &TagPluginChunk, sizeof(_cmsTagPluginChunkType));
    }

}