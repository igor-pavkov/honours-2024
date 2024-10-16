SQInstance::~SQInstance()
{
    REMOVE_FROM_CHAIN(&_sharedstate->_gc_chain, this);
    if(_class){ Finalize(); } //if _class is null it was already finalized by the GC
}