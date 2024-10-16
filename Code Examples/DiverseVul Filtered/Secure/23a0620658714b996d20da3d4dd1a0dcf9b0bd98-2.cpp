bool SQInstance::InstanceOf(SQClass *trg)
{
    SQClass *parent = _class;
    while(parent != NULL) {
        if(parent == trg)
            return true;
        parent = parent->_base;
    }
    return false;
}