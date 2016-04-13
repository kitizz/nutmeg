#ifndef NUTMEGLIB_PLUGIN_H
#define NUTMEGLIB_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class NutmegPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Nutmeg")

public:
    void registerTypes(const char *uri);
};

#endif // NUTMEGLIB_PLUGIN_H
