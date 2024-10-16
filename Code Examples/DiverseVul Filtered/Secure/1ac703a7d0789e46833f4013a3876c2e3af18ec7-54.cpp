void qemuProcessStop(virQEMUDriverPtr driver,
                     virDomainObjPtr vm,
                     virDomainShutoffReason reason,
                     qemuDomainAsyncJob asyncJob,
                     unsigned int flags)
{
    int ret;
    int retries = 0;
    qemuDomainObjPrivatePtr priv = vm->privateData;
    virErrorPtr orig_err;
    virDomainDefPtr def = vm->def;
    const virNetDevVPortProfile *vport = NULL;
    size_t i;
    g_autofree char *timestamp = NULL;
    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);
    g_autoptr(virConnect) conn = NULL;

    VIR_DEBUG("Shutting down vm=%p name=%s id=%d pid=%lld, "
              "reason=%s, asyncJob=%s, flags=0x%x",
              vm, vm->def->name, vm->def->id,
              (long long)vm->pid,
              virDomainShutoffReasonTypeToString(reason),
              qemuDomainAsyncJobTypeToString(asyncJob),
              flags);

    /* This method is routinely used in clean up paths. Disable error
     * reporting so we don't squash a legit error. */
    virErrorPreserveLast(&orig_err);

    if (asyncJob != QEMU_ASYNC_JOB_NONE) {
        if (qemuDomainObjBeginNestedJob(driver, vm, asyncJob) < 0)
            goto cleanup;
    } else if (priv->job.asyncJob != QEMU_ASYNC_JOB_NONE &&
               priv->job.asyncOwner == virThreadSelfID() &&
               priv->job.active != QEMU_JOB_ASYNC_NESTED) {
        VIR_WARN("qemuProcessStop called without a nested job (async=%s)",
                 qemuDomainAsyncJobTypeToString(asyncJob));
    }

    if (!virDomainObjIsActive(vm)) {
        VIR_DEBUG("VM '%s' not active", vm->def->name);
        goto endjob;
    }

    qemuProcessBuildDestroyMemoryPaths(driver, vm, NULL, false);

    if (!!g_atomic_int_dec_and_test(&driver->nactive) && driver->inhibitCallback)
        driver->inhibitCallback(false, driver->inhibitOpaque);

    /* Wake up anything waiting on domain condition */
    virDomainObjBroadcast(vm);

    if ((timestamp = virTimeStringNow()) != NULL) {
        qemuDomainLogAppendMessage(driver, vm, "%s: shutting down, reason=%s\n",
                                   timestamp,
                                   virDomainShutoffReasonTypeToString(reason));
    }

    /* Clear network bandwidth */
    virDomainClearNetBandwidth(vm->def);

    virDomainConfVMNWFilterTeardown(vm);

    if (cfg->macFilter) {
        for (i = 0; i < def->nnets; i++) {
            virDomainNetDefPtr net = def->nets[i];
            if (net->ifname == NULL)
                continue;
            ignore_value(ebtablesRemoveForwardAllowIn(driver->ebtables,
                                                      net->ifname,
                                                      &net->mac));
        }
    }

    virPortAllocatorRelease(priv->nbdPort);
    priv->nbdPort = 0;

    if (priv->agent) {
        qemuAgentClose(priv->agent);
        priv->agent = NULL;
    }
    priv->agentError = false;

    if (priv->mon) {
        qemuMonitorClose(priv->mon);
        priv->mon = NULL;
    }

    if (priv->monConfig) {
        if (priv->monConfig->type == VIR_DOMAIN_CHR_TYPE_UNIX)
            unlink(priv->monConfig->data.nix.path);
        virObjectUnref(priv->monConfig);
        priv->monConfig = NULL;
    }

    qemuDomainObjStopWorker(vm);

    /* Remove the master key */
    qemuDomainMasterKeyRemove(priv);

    /* Do this before we delete the tree and remove pidfile. */
    qemuProcessKillManagedPRDaemon(vm);

    ignore_value(virDomainChrDefForeach(vm->def,
                                        false,
                                        qemuProcessCleanupChardevDevice,
                                        NULL));


    /* shut it off for sure */
    ignore_value(qemuProcessKill(vm,
                                 VIR_QEMU_PROCESS_KILL_FORCE|
                                 VIR_QEMU_PROCESS_KILL_NOCHECK));

    qemuDomainCleanupRun(driver, vm);

    qemuExtDevicesStop(driver, vm);

    qemuDBusStop(driver, vm);

    vm->def->id = -1;

    virFileDeleteTree(priv->libDir);
    virFileDeleteTree(priv->channelTargetDir);

    /* Stop autodestroy in case guest is restarted */
    qemuProcessAutoDestroyRemove(driver, vm);

    /* now that we know it's stopped call the hook if present */
    if (virHookPresent(VIR_HOOK_DRIVER_QEMU)) {
        g_autofree char *xml = qemuDomainDefFormatXML(driver, NULL, vm->def, 0);

        /* we can't stop the operation even if the script raised an error */
        ignore_value(virHookCall(VIR_HOOK_DRIVER_QEMU, vm->def->name,
                                 VIR_HOOK_QEMU_OP_STOPPED, VIR_HOOK_SUBOP_END,
                                 NULL, xml, NULL));
    }

    /* Reset Security Labels unless caller don't want us to */
    if (!(flags & VIR_QEMU_PROCESS_STOP_NO_RELABEL))
        qemuSecurityRestoreAllLabel(driver, vm,
                                    !!(flags & VIR_QEMU_PROCESS_STOP_MIGRATED));

    for (i = 0; i < vm->def->ndisks; i++) {
        virDomainDeviceDef dev;
        virDomainDiskDefPtr disk = vm->def->disks[i];

        dev.type = VIR_DOMAIN_DEVICE_DISK;
        dev.data.disk = disk;
        ignore_value(qemuRemoveSharedDevice(driver, &dev, vm->def->name));
    }

    /* Clear out dynamically assigned labels */
    for (i = 0; i < vm->def->nseclabels; i++) {
        if (vm->def->seclabels[i]->type == VIR_DOMAIN_SECLABEL_DYNAMIC)
            VIR_FREE(vm->def->seclabels[i]->label);
        VIR_FREE(vm->def->seclabels[i]->imagelabel);
    }

    qemuHostdevReAttachDomainDevices(driver, vm->def);

    for (i = 0; i < def->nnets; i++) {
        virDomainNetDefPtr net = def->nets[i];
        vport = virDomainNetGetActualVirtPortProfile(net);

        switch (virDomainNetGetActualType(net)) {
        case VIR_DOMAIN_NET_TYPE_DIRECT:
            ignore_value(virNetDevMacVLanDeleteWithVPortProfile(
                             net->ifname, &net->mac,
                             virDomainNetGetActualDirectDev(net),
                             virDomainNetGetActualDirectMode(net),
                             virDomainNetGetActualVirtPortProfile(net),
                             cfg->stateDir));
            break;
        case VIR_DOMAIN_NET_TYPE_ETHERNET:
            if (net->managed_tap != VIR_TRISTATE_BOOL_NO && net->ifname) {
                ignore_value(virNetDevTapDelete(net->ifname, net->backend.tap));
                VIR_FREE(net->ifname);
            }
            break;
        case VIR_DOMAIN_NET_TYPE_BRIDGE:
        case VIR_DOMAIN_NET_TYPE_NETWORK:
#ifdef VIR_NETDEV_TAP_REQUIRE_MANUAL_CLEANUP
            if (!(vport && vport->virtPortType == VIR_NETDEV_VPORT_PROFILE_OPENVSWITCH))
                ignore_value(virNetDevTapDelete(net->ifname, net->backend.tap));
#endif
            break;
        case VIR_DOMAIN_NET_TYPE_USER:
        case VIR_DOMAIN_NET_TYPE_VHOSTUSER:
        case VIR_DOMAIN_NET_TYPE_SERVER:
        case VIR_DOMAIN_NET_TYPE_CLIENT:
        case VIR_DOMAIN_NET_TYPE_MCAST:
        case VIR_DOMAIN_NET_TYPE_INTERNAL:
        case VIR_DOMAIN_NET_TYPE_HOSTDEV:
        case VIR_DOMAIN_NET_TYPE_UDP:
        case VIR_DOMAIN_NET_TYPE_VDPA:
        case VIR_DOMAIN_NET_TYPE_LAST:
            /* No special cleanup procedure for these types. */
            break;
        }
        /* release the physical device (or any other resources used by
         * this interface in the network driver
         */
        if (vport) {
            if (vport->virtPortType == VIR_NETDEV_VPORT_PROFILE_MIDONET) {
                ignore_value(virNetDevMidonetUnbindPort(vport));
            } else if (vport->virtPortType == VIR_NETDEV_VPORT_PROFILE_OPENVSWITCH) {
                ignore_value(virNetDevOpenvswitchRemovePort(
                                 virDomainNetGetActualBridgeName(net),
                                 net->ifname));
            }
        }

        /* kick the device out of the hostdev list too */
        virDomainNetRemoveHostdev(def, net);
        if (net->type == VIR_DOMAIN_NET_TYPE_NETWORK) {
            if (conn || (conn = virGetConnectNetwork()))
                virDomainNetReleaseActualDevice(conn, vm->def, net);
            else
                VIR_WARN("Unable to release network device '%s'", NULLSTR(net->ifname));
        }
    }

 retry:
    if ((ret = qemuRemoveCgroup(vm)) < 0) {
        if (ret == -EBUSY && (retries++ < 5)) {
            g_usleep(200*1000);
            goto retry;
        }
        VIR_WARN("Failed to remove cgroup for %s",
                 vm->def->name);
    }

    /* Remove resctrl allocation after cgroups are cleaned up which makes it
     * kind of safer (although removing the allocation should work even with
     * pids in tasks file */
    for (i = 0; i < vm->def->nresctrls; i++) {
        size_t j = 0;

        for (j = 0; j < vm->def->resctrls[i]->nmonitors; j++) {
            virDomainResctrlMonDefPtr mon = NULL;

            mon = vm->def->resctrls[i]->monitors[j];
            virResctrlMonitorRemove(mon->instance);
        }

        virResctrlAllocRemove(vm->def->resctrls[i]->alloc);
    }

    /* clean up a possible backup job */
    if (priv->backup)
        qemuBackupJobTerminate(vm, QEMU_DOMAIN_JOB_STATUS_CANCELED);

    qemuProcessRemoveDomainStatus(driver, vm);

    /* Remove VNC and Spice ports from port reservation bitmap, but only if
       they were reserved by the driver (autoport=yes)
    */
    for (i = 0; i < vm->def->ngraphics; ++i) {
        virDomainGraphicsDefPtr graphics = vm->def->graphics[i];
        if (graphics->type == VIR_DOMAIN_GRAPHICS_TYPE_VNC) {
            if (graphics->data.vnc.autoport) {
                virPortAllocatorRelease(graphics->data.vnc.port);
            } else if (graphics->data.vnc.portReserved) {
                virPortAllocatorRelease(graphics->data.vnc.port);
                graphics->data.vnc.portReserved = false;
            }
            if (graphics->data.vnc.websocketGenerated) {
                virPortAllocatorRelease(graphics->data.vnc.websocket);
                graphics->data.vnc.websocketGenerated = false;
                graphics->data.vnc.websocket = -1;
            } else if (graphics->data.vnc.websocket) {
                virPortAllocatorRelease(graphics->data.vnc.websocket);
            }
        }
        if (graphics->type == VIR_DOMAIN_GRAPHICS_TYPE_SPICE) {
            if (graphics->data.spice.autoport) {
                virPortAllocatorRelease(graphics->data.spice.port);
                virPortAllocatorRelease(graphics->data.spice.tlsPort);
            } else {
                if (graphics->data.spice.portReserved) {
                    virPortAllocatorRelease(graphics->data.spice.port);
                    graphics->data.spice.portReserved = false;
                }

                if (graphics->data.spice.tlsPortReserved) {
                    virPortAllocatorRelease(graphics->data.spice.tlsPort);
                    graphics->data.spice.tlsPortReserved = false;
                }
            }
        }
    }

    for (i = 0; i < vm->ndeprecations; i++)
        g_free(vm->deprecations[i]);
    g_free(vm->deprecations);
    vm->ndeprecations = 0;
    vm->deprecations = NULL;
    vm->taint = 0;
    vm->pid = -1;
    virDomainObjSetState(vm, VIR_DOMAIN_SHUTOFF, reason);
    for (i = 0; i < vm->def->niothreadids; i++)
        vm->def->iothreadids[i]->thread_id = 0;

    /* Do this explicitly after vm->pid is reset so that security drivers don't
     * try to enter the domain's namespace which is non-existent by now as qemu
     * is no longer running. */
    if (!(flags & VIR_QEMU_PROCESS_STOP_NO_RELABEL)) {
        for (i = 0; i < def->ndisks; i++) {
            virDomainDiskDefPtr disk = def->disks[i];

            if (disk->mirror) {
                if (qemuSecurityRestoreImageLabel(driver, vm, disk->mirror, false) < 0)
                    VIR_WARN("Unable to restore security label on %s", disk->dst);

                if (virStorageSourceChainHasNVMe(disk->mirror))
                    qemuHostdevReAttachOneNVMeDisk(driver, vm->def->name, disk->mirror);
            }

            qemuBlockRemoveImageMetadata(driver, vm, disk->dst, disk->src);

            /* for now transient disks are forbidden with migration so they
             * can be handled here */
            if (disk->transient &&
                !priv->inhibitDiskTransientDelete) {
                VIR_DEBUG("Removing transient overlay '%s' of disk '%s'",
                          disk->src->path, disk->dst);
                if (qemuDomainStorageFileInit(driver, vm, disk->src, NULL) >= 0) {
                    virStorageSourceUnlink(disk->src);
                    virStorageSourceDeinit(disk->src);
                }
            }
        }
    }

    qemuSecurityReleaseLabel(driver->securityManager, vm->def);

    /* clear all private data entries which are no longer needed */
    qemuDomainObjPrivateDataClear(priv);

    /* The "release" hook cleans up additional resources */
    if (virHookPresent(VIR_HOOK_DRIVER_QEMU)) {
        g_autofree char *xml = qemuDomainDefFormatXML(driver, NULL, vm->def, 0);

        /* we can't stop the operation even if the script raised an error */
        virHookCall(VIR_HOOK_DRIVER_QEMU, vm->def->name,
                    VIR_HOOK_QEMU_OP_RELEASE, VIR_HOOK_SUBOP_END,
                    NULL, xml, NULL);
    }

    virDomainObjRemoveTransientDef(vm);

 endjob:
    if (asyncJob != QEMU_ASYNC_JOB_NONE)
        qemuDomainObjEndJob(driver, vm);

 cleanup:
    virErrorRestore(&orig_err);
}