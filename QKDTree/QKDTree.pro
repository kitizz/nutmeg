TEMPLATE = subdirs

SUBDIRS += \
    QKDTree \
    Tests \
    QVectorND

QKDTree.depends += QVectorND
Tests.depends += QKDTree
