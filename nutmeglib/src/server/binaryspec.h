#ifndef BINARYSPEC_H
#define BINARYSPEC_H

#include <QJsonArray>
#include <QJsonObject>
#include "../util/ndarray.h"

class BinarySpec
{
public:
    BinarySpec (QJsonObject jsonSpec)
        : valid(false)
        , shape(QList<int>())
    {
        QJsonValue type_v = jsonSpec["type"];
        if (!type_v.isString() || type_v.isUndefined()) {
            qWarning() << "WARNING: Invalid Binary Json Spec: \"type\" must be specified.";
            return;
        }

        QJsonValue shape_v = jsonSpec["shape"];
        if (!shape_v.isArray() || shape_v.isUndefined()) {
            qWarning() << "WARNING: Invalid Binary Json Spec: \"shape\" must be specified.";
            return;
        }

        // Set type
        type = fromString(type_v.toString());
        if (type == NDArray::Unknown) {
            qWarning() << "WARNING: Unknown \"type\" spec for binary data -" << type_v.toString();
            return;
        }

        // Set shape
        QJsonArray shape_arr = shape_v.toArray();
        foreach(QJsonValue v, shape_arr) {
            int n = v.toDouble(-1);
            if (n < 0) {
                qWarning() << "WARNING: Invalid Binary Json Spec: \"shape\" must be list of numbers.";
                return;
            }
            shape << n;
        }

        // Made it this far, we have a valid spec
        valid = true;
        qDebug() << "Binary Header - type:" << type << "shape:" << shape;
    }

    static NDArray::Type fromString(const QString &typeName)
    {
        static QMap<QString, NDArray::Type> str2type;
        if (str2type.isEmpty()) {
            str2type["int64"] = NDArray::Int64;
            str2type["uint8"] = NDArray::Uint8;
            str2type["float32"] = NDArray::Float32;
            str2type["float64"] = NDArray::Float64;
            str2type["bool"] = NDArray::Bool;
        }
        return str2type.value(typeName, NDArray::Unknown);
    }

    bool valid;
    NDArray::Type type;
    QList<int> shape;
};

#endif // BINARYSPEC_H
