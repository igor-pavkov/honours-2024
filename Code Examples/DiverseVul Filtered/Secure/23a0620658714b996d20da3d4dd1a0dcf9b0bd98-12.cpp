void SQInstance::Init(SQSharedState *ss)
{
    _userpointer = NULL;
    _hook = NULL;
    __ObjAddRef(_class);
    _delegate = _class->_members;
    INIT_CHAIN();
    ADD_TO_CHAIN(&_sharedstate->_gc_chain, this);
}