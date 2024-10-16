int qemuProcessStopCPUs(virQEMUDriverPtr driver,
                        virDomainObjPtr vm,
                        virDomainPausedReason reason,
                        qemuDomainAsyncJob asyncJob)
{
    int ret = -1;
    qemuDomainObjPrivatePtr priv = vm->privateData;

    VIR_FREE(priv->lockState);

    priv->pausedReason = reason;

    if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) < 0)
        goto cleanup;

    ret = qemuMonitorStopCPUs(priv->mon);
    if (qemuDomainObjExitMonitor(driver, vm) < 0)
        ret = -1;

    if (ret < 0)
        goto cleanup;

    /* de-activate netdevs after stopping CPUs */
    ignore_value(qemuInterfaceStopDevices(vm->def));

    if (priv->job.current)
        ignore_value(virTimeMillisNow(&priv->job.current->stopped));

    /* The STOP event handler will change the domain state with the reason
     * saved in priv->pausedReason and it will also emit corresponding domain
     * lifecycle event.
     */

    if (virDomainLockProcessPause(driver->lockManager, vm, &priv->lockState) < 0)
        VIR_WARN("Unable to release lease on %s", vm->def->name);
    VIR_DEBUG("Preserving lock state '%s'", NULLSTR(priv->lockState));

 cleanup:
    if (ret < 0)
        priv->pausedReason = VIR_DOMAIN_PAUSED_UNKNOWN;

    return ret;
}