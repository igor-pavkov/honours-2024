void qemu_input_queue_abs(QemuConsole *src, InputAxis axis, int value,
                          int min_in, int max_in)
{
    InputEvent *evt;
    int scaled = qemu_input_scale_axis(value, min_in, max_in,
                                       INPUT_EVENT_ABS_MIN,
                                       INPUT_EVENT_ABS_MAX);
    evt = qemu_input_event_new_move(INPUT_EVENT_KIND_ABS, axis, scaled);
    qemu_input_event_send(src, evt);
    qapi_free_InputEvent(evt);
}