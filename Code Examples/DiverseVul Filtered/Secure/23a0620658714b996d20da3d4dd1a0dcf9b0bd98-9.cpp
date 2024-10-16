SQClass::SQClass(SQSharedState *ss,SQClass *base)
{
    _base = base;
    _typetag = 0;
    _hook = NULL;
    _udsize = 0;
    _locked = false;
    _constructoridx = -1;
    if(_base) {
        _constructoridx = _base->_constructoridx;
        _udsize = _base->_udsize;
        _defaultvalues.copy(base->_defaultvalues);
        _methods.copy(base->_methods);
        _COPY_VECTOR(_metamethods,base->_metamethods,MT_LAST);
        __ObjAddRef(_base);
    }
    _members = base?base->_members->Clone() : SQTable::Create(ss,0);
    __ObjAddRef(_members);

    INIT_CHAIN();
    ADD_TO_CHAIN(&_sharedstate->_gc_chain, this);
}