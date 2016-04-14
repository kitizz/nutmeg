#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

#include <QVariantList>
#include <QDir>
#include <QThread>
#include <QQuickItem>
#include "../common/figurebase.h"
#include "../common/plotbase.h"

#include "../util/ndarray.h"
#include "binaryspec.h"

namespace ServerUtil {
    // Allow sleeping
    class ThreadTools : private QThread
    {
    public:
        using QThread::msleep;

    private:
        ThreadTools() {}
    };

    static void sleep(unsigned long msecs)
    {
        ThreadTools::msleep(msecs);
    }

    static void parseBinary(QVariantList &args, const QList<BinarySpec> &bin_specs, QByteArrayList &data)
    {
        for (int i=0; i<args.length(); ++i) {
            QVariant var = args[i];
            if (!var.canConvert(QMetaType::QString))
                continue;

            QString value = var.toString();
            if (value.startsWith("$bin") && value.endsWith('$')) {
                int n = value.length() - 5;
                bool ok;
                int index = value.midRef(4, n).toInt(&ok);
                if (!ok)
                    continue;

                BinarySpec spec = bin_specs[index];
                NDArray array(spec.type, spec.shape, data[index]);
                qDebug() << "Created array shape:" << array.shape();
                args[i] = QVariant::fromValue(array);
            }
        }
    }

    static QString adjustPath(const QString &path)
    {
    #if defined(Q_OS_IOS)
        if (!QDir::isAbsolutePath(path))
            return QString::fromLatin1("%1/%2")
                    .arg(QCoreApplication::applicationDirPath(), path);
    #elif defined(Q_OS_MAC)
        if (!QDir::isAbsolutePath(path))
            return QString::fromLatin1("%1/../Resources/%2")
                    .arg(QCoreApplication::applicationDirPath(), path);
    #elif defined(Q_OS_BLACKBERRY)
        if (!QDir::isAbsolutePath(path))
            return QString::fromLatin1("app/native/%1").arg(path);
    #elif !defined(Q_OS_ANDROID)
        QString pathInInstallDir =
                QString::fromLatin1("%1/../%2").arg(QCoreApplication::applicationDirPath(), path);
        if (QFileInfo(pathInInstallDir).exists())
            return pathInInstallDir;
        pathInInstallDir =
                QString::fromLatin1("%1/%2").arg(QCoreApplication::applicationDirPath(), path);
        if (QFileInfo(pathInInstallDir).exists())
            return pathInInstallDir;
    #elif defined(Q_OS_ANDROID_NO_SDK)
        return QLatin1String("/data/user/qt/") + path;
    #endif
        return path;
    }

    static QString getLineFromFile(QUrl path, int line)
    {
        QFile file(path.path());
        qDebug() << "Attempting to open:" << path.path();
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed..";
            return "";
        }

        QByteArray linestr;
        while (!file.atEnd() && line--) {
            linestr = file.readLine();
        }

        return linestr.trimmed();
    }
}

#endif // SERVER_UTIL_H
