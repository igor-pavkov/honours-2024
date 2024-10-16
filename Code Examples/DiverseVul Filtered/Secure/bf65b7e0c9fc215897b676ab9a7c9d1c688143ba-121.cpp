bool unit_type_supported(UnitType t) {
        if (_unlikely_(t < 0))
                return false;
        if (_unlikely_(t >= _UNIT_TYPE_MAX))
                return false;

        if (!unit_vtable[t]->supported)
                return true;

        return unit_vtable[t]->supported();
}