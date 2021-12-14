/****************************************************************************
** Meta object code from reading C++ file 'bouton.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/bouton.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bouton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Bouton_t {
    QByteArrayData data[9];
    char stringdata0[132];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Bouton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Bouton_t qt_meta_stringdata_Bouton = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Bouton"
QT_MOC_LITERAL(1, 7, 16), // "notifyMouseMoved"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 16), // "newMousePosition"
QT_MOC_LITERAL(4, 42, 24), // "notifyMouseButtonPressed"
QT_MOC_LITERAL(5, 67, 13), // "mousePosition"
QT_MOC_LITERAL(6, 81, 16), // "Qt::MouseButtons"
QT_MOC_LITERAL(7, 98, 7), // "buttons"
QT_MOC_LITERAL(8, 106, 25) // "notifyMouseButtonReleased"

    },
    "Bouton\0notifyMouseMoved\0\0newMousePosition\0"
    "notifyMouseButtonPressed\0mousePosition\0"
    "Qt::MouseButtons\0buttons\0"
    "notifyMouseButtonReleased"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Bouton[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    2,   32,    2, 0x06 /* Public */,
       8,    2,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPointF,    3,
    QMetaType::Void, QMetaType::QPointF, 0x80000000 | 6,    5,    7,
    QMetaType::Void, QMetaType::QPointF, 0x80000000 | 6,    5,    7,

       0        // eod
};

void Bouton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Bouton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->notifyMouseMoved((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 1: _t->notifyMouseButtonPressed((*reinterpret_cast< QPointF(*)>(_a[1])),(*reinterpret_cast< Qt::MouseButtons(*)>(_a[2]))); break;
        case 2: _t->notifyMouseButtonReleased((*reinterpret_cast< QPointF(*)>(_a[1])),(*reinterpret_cast< Qt::MouseButtons(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Bouton::*)(QPointF );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bouton::notifyMouseMoved)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Bouton::*)(QPointF , Qt::MouseButtons );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bouton::notifyMouseButtonPressed)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Bouton::*)(QPointF , Qt::MouseButtons );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Bouton::notifyMouseButtonReleased)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Bouton::staticMetaObject = { {
    &Sprite::staticMetaObject,
    qt_meta_stringdata_Bouton.data,
    qt_meta_data_Bouton,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Bouton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Bouton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Bouton.stringdata0))
        return static_cast<void*>(this);
    return Sprite::qt_metacast(_clname);
}

int Bouton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Sprite::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Bouton::notifyMouseMoved(QPointF _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Bouton::notifyMouseButtonPressed(QPointF _t1, Qt::MouseButtons _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Bouton::notifyMouseButtonReleased(QPointF _t1, Qt::MouseButtons _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
