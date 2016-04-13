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
        if (!type_v.isDouble() || type_v.isUndefined()) {
            qWarning() << "Invalid Binary Json Spec: \"type\" must be specified.";
            return;
        }

        QJsonValue shape_v = jsonSpec["shape"];
        if (!shape_v.isArray() || shape_v.isUndefined()) {
            qWarning() << "Invalid Binary Json Spec: \"shape\" must be specified.";
            return;
        }

        // Set type
        type = (NDArray::Type)type_v.toDouble();

        // Set shape
        QJsonArray shape_arr = shape_v.toArray();
        foreach(QJsonValue v, shape_arr) {
            int n = v.toDouble(-1);
            if (n < 0) {
                qWarning() << "Invalid Binary Json Spec: \"shape\" must be list of numbers.";
                return;
            }
            shape << n;
        }

        // Made it this far, we have a valid spec
        valid = true;
    }

    bool valid;
    NDArray::Type type;
    QList<int> shape;
};

#endif // BINARYSPEC_H
