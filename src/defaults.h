#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <QFont>

class Defaults
{
public:

    static QFont defaultFont() {
        QFont font("Calibri");
        font.setPointSize(15);
        font.setWeight(1);
        font.setHintingPreference(QFont::PreferFullHinting);
        font.setStyleHint(QFont::Times);
        return font;
    }

private:
    Defaults();
    ~Defaults();
};

#endif // DEFAULTS_H
