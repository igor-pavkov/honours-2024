static int mov_read_pitm(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    avio_rb32(pb);  // version & flags.
    c->primary_item_id = avio_rb16(pb);
    return atom.size;
}