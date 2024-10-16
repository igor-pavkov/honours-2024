bool qemuProcessAutoDestroyActive(virQEMUDriverPtr driver,
                                  virDomainObjPtr vm)
{
    virCloseCallback cb;
    VIR_DEBUG("vm=%s", vm->def->name);
    cb = virCloseCallbacksGet(driver->closeCallbacks, vm, NULL);
    return cb == qemuProcessAutoDestroy;
}