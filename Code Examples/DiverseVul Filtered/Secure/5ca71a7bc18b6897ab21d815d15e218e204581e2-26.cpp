cmsTagDescriptor* _cmsGetTagDescriptor(cmsContext ContextID, cmsTagSignature sig)
{
    _cmsTagLinkedList* pt;
    _cmsTagPluginChunkType* TagPluginChunk = ( _cmsTagPluginChunkType*) _cmsContextGetClientChunk(ContextID, TagPlugin);

    for (pt = TagPluginChunk->Tag;
             pt != NULL;
             pt = pt ->Next) {

                if (sig == pt -> Signature) return &pt ->Descriptor;
    }

    for (pt = SupportedTags;
            pt != NULL;
            pt = pt ->Next) {

                if (sig == pt -> Signature) return &pt ->Descriptor;
    }

    return NULL;
}