SQInstance::SQInstance(SQSharedState *ss, SQClass *c, SQInteger memsize)
{
    _memsize = memsize;
    _class = c;
    SQUnsignedInteger nvalues = _class->_defaultvalues.size();
    for(SQUnsignedInteger n = 0; n < nvalues; n++) {
        new (&_values[n]) SQObjectPtr(_class->_defaultvalues[n].val);
    }
    Init(ss);
}