void SQClass::Finalize() {
    _attributes.Null();
    _NULL_SQOBJECT_VECTOR(_defaultvalues,_defaultvalues.size());
    _methods.resize(0);
    _NULL_SQOBJECT_VECTOR(_metamethods,MT_LAST);
    __ObjRelease(_members);
    if(_base) {
        __ObjRelease(_base);
    }
}