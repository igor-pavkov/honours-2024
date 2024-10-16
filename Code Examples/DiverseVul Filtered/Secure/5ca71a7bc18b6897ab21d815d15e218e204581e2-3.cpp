cmsTagTypeHandler* GetHandler(cmsTagTypeSignature sig, _cmsTagTypeLinkedList* PluginLinkedList, _cmsTagTypeLinkedList* DefaultLinkedList)
{
    _cmsTagTypeLinkedList* pt;

    for (pt = PluginLinkedList;
         pt != NULL;
         pt = pt ->Next) {

            if (sig == pt -> Handler.Signature) return &pt ->Handler;
    }

    for (pt = DefaultLinkedList;
         pt != NULL;
         pt = pt ->Next) {

            if (sig == pt -> Handler.Signature) return &pt ->Handler;
    }

    return NULL;
}