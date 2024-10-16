cmsBool  _cmsRegisterTagPlugin(cmsContext id, cmsPluginBase* Data)
{
    cmsPluginTag* Plugin = (cmsPluginTag*) Data;
    _cmsTagLinkedList *pt;
    _cmsTagPluginChunkType* TagPluginChunk = ( _cmsTagPluginChunkType*) _cmsContextGetClientChunk(id, TagPlugin);

    if (Data == NULL) {

        TagPluginChunk->Tag = NULL;
        return TRUE;
    }

    pt = (_cmsTagLinkedList*) _cmsPluginMalloc(id, sizeof(_cmsTagLinkedList));
    if (pt == NULL) return FALSE;

    pt ->Signature  = Plugin ->Signature;
    pt ->Descriptor = Plugin ->Descriptor;
    pt ->Next       = TagPluginChunk ->Tag;

    TagPluginChunk ->Tag = pt;
    
    return TRUE;
}