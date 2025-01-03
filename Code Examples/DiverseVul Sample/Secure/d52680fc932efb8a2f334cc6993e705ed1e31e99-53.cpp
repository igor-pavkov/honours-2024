static void ioport_write(void *opaque, hwaddr addr,
                         uint64_t val, unsigned size)
{
    PCIQXLDevice *d = opaque;
    uint32_t io_port = addr;
    qxl_async_io async = QXL_SYNC;
    uint32_t orig_io_port = io_port;

    if (d->guest_bug && io_port != QXL_IO_RESET) {
        return;
    }

    if (d->revision <= QXL_REVISION_STABLE_V10 &&
        io_port > QXL_IO_FLUSH_RELEASE) {
        qxl_set_guest_bug(d, "unsupported io %d for revision %d\n",
            io_port, d->revision);
        return;
    }

    switch (io_port) {
    case QXL_IO_RESET:
    case QXL_IO_SET_MODE:
    case QXL_IO_MEMSLOT_ADD:
    case QXL_IO_MEMSLOT_DEL:
    case QXL_IO_CREATE_PRIMARY:
    case QXL_IO_UPDATE_IRQ:
    case QXL_IO_LOG:
    case QXL_IO_MEMSLOT_ADD_ASYNC:
    case QXL_IO_CREATE_PRIMARY_ASYNC:
        break;
    default:
        if (d->mode != QXL_MODE_VGA) {
            break;
        }
        trace_qxl_io_unexpected_vga_mode(d->id,
            addr, val, io_port_to_string(io_port));
        /* be nice to buggy guest drivers */
        if (io_port >= QXL_IO_UPDATE_AREA_ASYNC &&
            io_port < QXL_IO_RANGE_SIZE) {
            qxl_send_events(d, QXL_INTERRUPT_IO_CMD);
        }
        return;
    }

    /* we change the io_port to avoid ifdeffery in the main switch */
    orig_io_port = io_port;
    switch (io_port) {
    case QXL_IO_UPDATE_AREA_ASYNC:
        io_port = QXL_IO_UPDATE_AREA;
        goto async_common;
    case QXL_IO_MEMSLOT_ADD_ASYNC:
        io_port = QXL_IO_MEMSLOT_ADD;
        goto async_common;
    case QXL_IO_CREATE_PRIMARY_ASYNC:
        io_port = QXL_IO_CREATE_PRIMARY;
        goto async_common;
    case QXL_IO_DESTROY_PRIMARY_ASYNC:
        io_port = QXL_IO_DESTROY_PRIMARY;
        goto async_common;
    case QXL_IO_DESTROY_SURFACE_ASYNC:
        io_port = QXL_IO_DESTROY_SURFACE_WAIT;
        goto async_common;
    case QXL_IO_DESTROY_ALL_SURFACES_ASYNC:
        io_port = QXL_IO_DESTROY_ALL_SURFACES;
        goto async_common;
    case QXL_IO_FLUSH_SURFACES_ASYNC:
    case QXL_IO_MONITORS_CONFIG_ASYNC:
async_common:
        async = QXL_ASYNC;
        qemu_mutex_lock(&d->async_lock);
        if (d->current_async != QXL_UNDEFINED_IO) {
            qxl_set_guest_bug(d, "%d async started before last (%d) complete",
                io_port, d->current_async);
            qemu_mutex_unlock(&d->async_lock);
            return;
        }
        d->current_async = orig_io_port;
        qemu_mutex_unlock(&d->async_lock);
        break;
    default:
        break;
    }
    trace_qxl_io_write(d->id, qxl_mode_to_string(d->mode),
                       addr, io_port_to_string(addr),
                       val, size, async);

    switch (io_port) {
    case QXL_IO_UPDATE_AREA:
    {
        QXLCookie *cookie = NULL;
        QXLRect update = d->ram->update_area;

        if (d->ram->update_surface > d->ssd.num_surfaces) {
            qxl_set_guest_bug(d, "QXL_IO_UPDATE_AREA: invalid surface id %d\n",
                              d->ram->update_surface);
            break;
        }
        if (update.left >= update.right || update.top >= update.bottom ||
            update.left < 0 || update.top < 0) {
            qxl_set_guest_bug(d,
                    "QXL_IO_UPDATE_AREA: invalid area (%ux%u)x(%ux%u)\n",
                    update.left, update.top, update.right, update.bottom);
            if (update.left == update.right || update.top == update.bottom) {
                /* old drivers may provide empty area, keep going */
                qxl_clear_guest_bug(d);
                goto cancel_async;
            }
            break;
        }
        if (async == QXL_ASYNC) {
            cookie = qxl_cookie_new(QXL_COOKIE_TYPE_IO,
                                    QXL_IO_UPDATE_AREA_ASYNC);
            cookie->u.area = update;
        }
        qxl_spice_update_area(d, d->ram->update_surface,
                              cookie ? &cookie->u.area : &update,
                              NULL, 0, 0, async, cookie);
        break;
    }
    case QXL_IO_NOTIFY_CMD:
        qemu_spice_wakeup(&d->ssd);
        break;
    case QXL_IO_NOTIFY_CURSOR:
        qemu_spice_wakeup(&d->ssd);
        break;
    case QXL_IO_UPDATE_IRQ:
        qxl_update_irq(d);
        break;
    case QXL_IO_NOTIFY_OOM:
        if (!SPICE_RING_IS_EMPTY(&d->ram->release_ring)) {
            break;
        }
        d->oom_running = 1;
        qxl_spice_oom(d);
        d->oom_running = 0;
        break;
    case QXL_IO_SET_MODE:
        qxl_set_mode(d, val, 0);
        break;
    case QXL_IO_LOG:
        if (TRACE_QXL_IO_LOG_ENABLED || d->guestdebug) {
            /* We cannot trust the guest to NUL terminate d->ram->log_buf */
            char *log_buf = g_strndup((const char *)d->ram->log_buf,
                                      sizeof(d->ram->log_buf));
            trace_qxl_io_log(d->id, log_buf);
            if (d->guestdebug) {
                fprintf(stderr, "qxl/guest-%d: %" PRId64 ": %s", d->id,
                        qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL), log_buf);
            }
            g_free(log_buf);
        }
        break;
    case QXL_IO_RESET:
        qxl_hard_reset(d, 0);
        break;
    case QXL_IO_MEMSLOT_ADD:
        if (val >= NUM_MEMSLOTS) {
            qxl_set_guest_bug(d, "QXL_IO_MEMSLOT_ADD: val out of range");
            break;
        }
        if (d->guest_slots[val].active) {
            qxl_set_guest_bug(d,
                        "QXL_IO_MEMSLOT_ADD: memory slot already active");
            break;
        }
        d->guest_slots[val].slot = d->ram->mem_slot;
        qxl_add_memslot(d, val, 0, async);
        break;
    case QXL_IO_MEMSLOT_DEL:
        if (val >= NUM_MEMSLOTS) {
            qxl_set_guest_bug(d, "QXL_IO_MEMSLOT_DEL: val out of range");
            break;
        }
        qxl_del_memslot(d, val);
        break;
    case QXL_IO_CREATE_PRIMARY:
        if (val != 0) {
            qxl_set_guest_bug(d, "QXL_IO_CREATE_PRIMARY (async=%d): val != 0",
                          async);
            goto cancel_async;
        }
        d->guest_primary.surface = d->ram->create_surface;
        qxl_create_guest_primary(d, 0, async);
        break;
    case QXL_IO_DESTROY_PRIMARY:
        if (val != 0) {
            qxl_set_guest_bug(d, "QXL_IO_DESTROY_PRIMARY (async=%d): val != 0",
                          async);
            goto cancel_async;
        }
        if (!qxl_destroy_primary(d, async)) {
            trace_qxl_io_destroy_primary_ignored(d->id,
                                                 qxl_mode_to_string(d->mode));
            goto cancel_async;
        }
        break;
    case QXL_IO_DESTROY_SURFACE_WAIT:
        if (val >= d->ssd.num_surfaces) {
            qxl_set_guest_bug(d, "QXL_IO_DESTROY_SURFACE (async=%d):"
                             "%" PRIu64 " >= NUM_SURFACES", async, val);
            goto cancel_async;
        }
        qxl_spice_destroy_surface_wait(d, val, async);
        break;
    case QXL_IO_FLUSH_RELEASE: {
        QXLReleaseRing *ring = &d->ram->release_ring;
        if (ring->prod - ring->cons + 1 == ring->num_items) {
            fprintf(stderr,
                "ERROR: no flush, full release ring [p%d,%dc]\n",
                ring->prod, ring->cons);
        }
        qxl_push_free_res(d, 1 /* flush */);
        break;
    }
    case QXL_IO_FLUSH_SURFACES_ASYNC:
        qxl_spice_flush_surfaces_async(d);
        break;
    case QXL_IO_DESTROY_ALL_SURFACES:
        d->mode = QXL_MODE_UNDEFINED;
        qxl_spice_destroy_surfaces(d, async);
        break;
    case QXL_IO_MONITORS_CONFIG_ASYNC:
        qxl_spice_monitors_config_async(d, 0);
        break;
    default:
        qxl_set_guest_bug(d, "%s: unexpected ioport=0x%x\n", __func__, io_port);
    }
    return;
cancel_async:
    if (async) {
        qxl_send_events(d, QXL_INTERRUPT_IO_CMD);
        qemu_mutex_lock(&d->async_lock);
        d->current_async = QXL_UNDEFINED_IO;
        qemu_mutex_unlock(&d->async_lock);
    }
}