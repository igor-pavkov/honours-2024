int vmstate_load_state(QEMUFile *f, const VMStateDescription *vmsd,
                       void *opaque, int version_id)
{
    VMStateField *field = vmsd->fields;
    int ret;

    if (version_id > vmsd->version_id) {
        return -EINVAL;
    }
    if (version_id < vmsd->minimum_version_id_old) {
        return -EINVAL;
    }
    if  (version_id < vmsd->minimum_version_id) {
        return vmsd->load_state_old(f, opaque, version_id);
    }
    if (vmsd->pre_load) {
        int ret = vmsd->pre_load(opaque);
        if (ret) {
            return ret;
        }
    }
    while (field->name) {
        if ((field->field_exists &&
             field->field_exists(opaque, version_id)) ||
            (!field->field_exists &&
             field->version_id <= version_id)) {
            void *base_addr = vmstate_base_addr(opaque, field);
            int i, n_elems = vmstate_n_elems(opaque, field);
            int size = vmstate_size(opaque, field);

            for (i = 0; i < n_elems; i++) {
                void *addr = base_addr + size * i;

                if (field->flags & VMS_ARRAY_OF_POINTER) {
                    addr = *(void **)addr;
                }
                if (field->flags & VMS_STRUCT) {
                    ret = vmstate_load_state(f, field->vmsd, addr,
                                             field->vmsd->version_id);
                } else {
                    ret = field->info->get(f, addr, size);

                }
                if (ret < 0) {
                    trace_vmstate_load_field_error(field->name, ret);
                    return ret;
                }
            }
        } else if (field->flags & VMS_MUST_EXIST) {
            fprintf(stderr, "Input validation failed: %s/%s\n",
                    vmsd->name, field->name);
            return -1;
        }
        field++;
    }
    ret = vmstate_subsection_load(f, vmsd, opaque);
    if (ret != 0) {
        return ret;
    }
    if (vmsd->post_load) {
        return vmsd->post_load(opaque, version_id);
    }
    return 0;
}