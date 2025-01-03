static void vnc_dpy_switch(DisplayChangeListener *dcl,
                           DisplaySurface *surface)
{
    VncDisplay *vd = container_of(dcl, VncDisplay, dcl);
    VncState *vs;

    vnc_abort_display_jobs(vd);
    vd->ds = surface;

    /* server surface */
    vnc_update_server_surface(vd);

    /* guest surface */
    qemu_pixman_image_unref(vd->guest.fb);
    vd->guest.fb = pixman_image_ref(surface->image);
    vd->guest.format = surface->format;

    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_colordepth(vs);
        vnc_desktop_resize(vs);
        if (vs->vd->cursor) {
            vnc_cursor_define(vs);
        }
        memset(vs->dirty, 0x00, sizeof(vs->dirty));
        vnc_set_area_dirty(vs->dirty, vd, 0, 0,
                           vnc_width(vd),
                           vnc_height(vd));
    }
}