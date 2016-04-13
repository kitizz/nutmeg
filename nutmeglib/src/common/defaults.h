#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "../nutmeglibdecl.h"
#include <QFont>
#include <QFontDatabase>

class NUTMEGLIB_EXPORT Defaults
{
public:

    static QFont defaultFont() {
        static int id = QFontDatabase::addApplicationFont(":/fonts/cmunbmr.ttf");
//        static int id = QFontDatabase::addApplicationFont(":/fonts/latinmodern.otf");
        static QString family = QFontDatabase::applicationFontFamilies(id).at(0);

        QFont font(family);
        font.setPointSize(15);
        font.setWeight(QFont::Thin);
//        font.setHintingPreference(QFont::PreferFullHinting);
        font.setStyleHint(QFont::Times);
        return font;
    }

private:
    Defaults();
    ~Defaults();
};

#endif // DEFAULTS_H
