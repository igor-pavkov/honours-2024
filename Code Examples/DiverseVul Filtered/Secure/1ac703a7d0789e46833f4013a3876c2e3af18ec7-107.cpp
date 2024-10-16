int qemuProcessAutoDestroyRemove(virQEMUDriverPtr driver,
                                 virDomainObjPtr vm)
{
    VIR_DEBUG("vm=%s", vm->def->name);
    return virCloseCallbacksUnset(driver->closeCallbacks, vm,
                                  qemuProcessAutoDestroy);
}