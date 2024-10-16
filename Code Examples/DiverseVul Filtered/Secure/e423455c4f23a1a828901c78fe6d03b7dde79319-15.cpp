void load_elf_hdr(const char *filename, void *hdr, bool *is64, Error **errp)
{
    int fd;
    uint8_t e_ident_local[EI_NIDENT];
    uint8_t *e_ident;
    size_t hdr_size, off;
    bool is64l;

    if (!hdr) {
        hdr = e_ident_local;
    }
    e_ident = hdr;

    fd = open(filename, O_RDONLY | O_BINARY);
    if (fd < 0) {
        error_setg_errno(errp, errno, "Failed to open file: %s", filename);
        return;
    }
    if (read(fd, hdr, EI_NIDENT) != EI_NIDENT) {
        error_setg_errno(errp, errno, "Failed to read file: %s", filename);
        goto fail;
    }
    if (e_ident[0] != ELFMAG0 ||
        e_ident[1] != ELFMAG1 ||
        e_ident[2] != ELFMAG2 ||
        e_ident[3] != ELFMAG3) {
        error_setg(errp, "Bad ELF magic");
        goto fail;
    }

    is64l = e_ident[EI_CLASS] == ELFCLASS64;
    hdr_size = is64l ? sizeof(Elf64_Ehdr) : sizeof(Elf32_Ehdr);
    if (is64) {
        *is64 = is64l;
    }

    off = EI_NIDENT;
    while (hdr != e_ident_local && off < hdr_size) {
        size_t br = read(fd, hdr + off, hdr_size - off);
        switch (br) {
        case 0:
            error_setg(errp, "File too short: %s", filename);
            goto fail;
        case -1:
            error_setg_errno(errp, errno, "Failed to read file: %s",
                             filename);
            goto fail;
        }
        off += br;
    }

fail:
    close(fd);
}