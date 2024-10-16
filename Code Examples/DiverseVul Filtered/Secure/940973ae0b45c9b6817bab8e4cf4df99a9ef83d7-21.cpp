static void ide_rw_error(IDEState *s) {
    ide_abort_command(s);
    ide_set_irq(s->bus);
}