int qemuProcessAutoDestroyAdd(virQEMUDriverPtr driver,
                              virDomainObjPtr vm,
                              virConnectPtr conn)
{
    VIR_DEBUG("vm=%s, conn=%p", vm->def->name, conn);
    return virCloseCallbacksSet(driver->closeCallbacks, vm, conn,
                                qemuProcessAutoDestroy);
}