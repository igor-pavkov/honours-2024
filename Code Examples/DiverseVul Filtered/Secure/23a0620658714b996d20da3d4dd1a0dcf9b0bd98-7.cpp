SQClass::~SQClass()
{
    REMOVE_FROM_CHAIN(&_sharedstate->_gc_chain, this);
    Finalize();
}