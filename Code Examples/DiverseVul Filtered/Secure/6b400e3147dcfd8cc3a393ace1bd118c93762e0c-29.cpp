bool QPaintEngineEx::shouldDrawCachedGlyphs(QFontEngine *fontEngine, const QTransform &m) const
{
    if (fontEngine->glyphFormat == QFontEngine::Format_ARGB)
        return true;

    static const int maxCachedGlyphSizeSquared = std::pow([]{
        if (int env = qEnvironmentVariableIntValue("QT_MAX_CACHED_GLYPH_SIZE"))
            return env;
        return QT_MAX_CACHED_GLYPH_SIZE;
    }(), 2);

    qreal pixelSize = fontEngine->fontDef.pixelSize;
    return (pixelSize * pixelSize * qAbs(m.determinant())) <= maxCachedGlyphSizeSquared;
}