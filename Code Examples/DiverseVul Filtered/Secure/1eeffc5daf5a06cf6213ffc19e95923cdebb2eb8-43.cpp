void DL_Dxf::write3dFace(DL_WriterA& dw,
                   const DL_3dFaceData& data,
                   const DL_Attributes& attrib) {
    dw.entity("3DFACE");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbFace");
    }
    dw.coord(10, data.x[0], data.y[0], data.z[0]);
    dw.coord(11, data.x[1], data.y[1], data.z[1]);
    dw.coord(12, data.x[2], data.y[2], data.z[2]);
    dw.coord(13, data.x[3], data.y[3], data.z[3]);
}