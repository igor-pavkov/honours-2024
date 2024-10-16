static void bdrv_emit_qmp_eject_event(BlockDriverState *bs, bool ejected)
{
    QObject *data;

    data = qobject_from_jsonf("{ 'device': %s, 'tray-open': %i }",
                              bdrv_get_device_name(bs), ejected);
    monitor_protocol_event(QEVENT_DEVICE_TRAY_MOVED, data);

    qobject_decref(data);
}