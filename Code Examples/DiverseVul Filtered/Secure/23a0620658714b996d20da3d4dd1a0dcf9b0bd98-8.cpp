SQInstance::SQInstance(SQSharedState *ss, SQInstance *i, SQInteger memsize)
{
    _memsize = memsize;
    _class = i->_class;
    SQUnsignedInteger nvalues = _class->_defaultvalues.size();
    for(SQUnsignedInteger n = 0; n < nvalues; n++) {
        new (&_values[n]) SQObjectPtr(i->_values[n]);
    }
    Init(ss);
}