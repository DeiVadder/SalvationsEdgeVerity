/****************************************************************************
** Meta object code from reading C++ file 'calculatesteps.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.16)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../calculatesteps.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calculatesteps.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.16. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CalculateSteps_t {
    QByteArrayData data[27];
    char stringdata0[286];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CalculateSteps_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CalculateSteps_t qt_meta_stringdata_CalculateSteps = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CalculateSteps"
QT_MOC_LITERAL(1, 15, 20), // "numberOfStepsChanged"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 5), // "reset"
QT_MOC_LITERAL(4, 43, 14), // "calculateSteps"
QT_MOC_LITERAL(5, 58, 11), // "SymbolTypes"
QT_MOC_LITERAL(6, 70, 12), // "innerStatue1"
QT_MOC_LITERAL(7, 83, 12), // "innerStatue2"
QT_MOC_LITERAL(8, 96, 12), // "innerStatue3"
QT_MOC_LITERAL(9, 109, 12), // "outerStatue1"
QT_MOC_LITERAL(10, 122, 12), // "outerStatue2"
QT_MOC_LITERAL(11, 135, 12), // "outerStatue3"
QT_MOC_LITERAL(12, 148, 13), // "numberOfSteps"
QT_MOC_LITERAL(13, 162, 21), // "getInstructionForStep"
QT_MOC_LITERAL(14, 184, 4), // "step"
QT_MOC_LITERAL(15, 189, 6), // "statue"
QT_MOC_LITERAL(16, 196, 12), // "checkIsValid"
QT_MOC_LITERAL(17, 209, 9), // "Undefined"
QT_MOC_LITERAL(18, 219, 7), // "Dreieck"
QT_MOC_LITERAL(19, 227, 7), // "Viereck"
QT_MOC_LITERAL(20, 235, 5), // "Kreis"
QT_MOC_LITERAL(21, 241, 5), // "Kegel"
QT_MOC_LITERAL(22, 247, 7), // "Würfel"
QT_MOC_LITERAL(23, 255, 8), // "Zylinder"
QT_MOC_LITERAL(24, 264, 8), // "Pyramide"
QT_MOC_LITERAL(25, 273, 6), // "Prisma"
QT_MOC_LITERAL(26, 280, 5) // "Kugel"

    },
    "CalculateSteps\0numberOfStepsChanged\0"
    "\0reset\0calculateSteps\0SymbolTypes\0"
    "innerStatue1\0innerStatue2\0innerStatue3\0"
    "outerStatue1\0outerStatue2\0outerStatue3\0"
    "numberOfSteps\0getInstructionForStep\0"
    "step\0statue\0checkIsValid\0Undefined\0"
    "Dreieck\0Viereck\0Kreis\0Kegel\0Würfel\0"
    "Zylinder\0Pyramide\0Prisma\0Kugel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CalculateSteps[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       1,   78, // properties
       1,   82, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   45,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
       4,    6,   46,    2, 0x02 /* Public */,
      12,    0,   59,    2, 0x02 /* Public */,
      13,    2,   60,    2, 0x02 /* Public */,
      16,    6,   65,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 5,    6,    7,    8,    9,   10,   11,
    QMetaType::Int,
    0x80000000 | 5, QMetaType::Int, QMetaType::Int,   14,   15,
    QMetaType::Bool, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 5,    6,    7,    8,    9,   10,   11,

 // properties: name, type, flags
      12, QMetaType::Int, 0x00495801,

 // properties: notify_signal_id
       0,

 // enums: name, alias, flags, count, data
       5,    5, 0x0,   10,   87,

 // enum data: key, value
      17, uint(CalculateSteps::Undefined),
      18, uint(CalculateSteps::Dreieck),
      19, uint(CalculateSteps::Viereck),
      20, uint(CalculateSteps::Kreis),
      21, uint(CalculateSteps::Kegel),
      22, uint(CalculateSteps::Würfel),
      23, uint(CalculateSteps::Zylinder),
      24, uint(CalculateSteps::Pyramide),
      25, uint(CalculateSteps::Prisma),
      26, uint(CalculateSteps::Kugel),

       0        // eod
};

void CalculateSteps::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CalculateSteps *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->numberOfStepsChanged(); break;
        case 1: _t->reset(); break;
        case 2: _t->calculateSteps((*reinterpret_cast< SymbolTypes(*)>(_a[1])),(*reinterpret_cast< SymbolTypes(*)>(_a[2])),(*reinterpret_cast< SymbolTypes(*)>(_a[3])),(*reinterpret_cast< SymbolTypes(*)>(_a[4])),(*reinterpret_cast< SymbolTypes(*)>(_a[5])),(*reinterpret_cast< SymbolTypes(*)>(_a[6]))); break;
        case 3: { int _r = _t->numberOfSteps();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 4: { SymbolTypes _r = _t->getInstructionForStep((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< SymbolTypes*>(_a[0]) = std::move(_r); }  break;
        case 5: { bool _r = _t->checkIsValid((*reinterpret_cast< SymbolTypes(*)>(_a[1])),(*reinterpret_cast< SymbolTypes(*)>(_a[2])),(*reinterpret_cast< SymbolTypes(*)>(_a[3])),(*reinterpret_cast< SymbolTypes(*)>(_a[4])),(*reinterpret_cast< SymbolTypes(*)>(_a[5])),(*reinterpret_cast< SymbolTypes(*)>(_a[6])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CalculateSteps::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CalculateSteps::numberOfStepsChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CalculateSteps *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->numberOfSteps(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject CalculateSteps::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CalculateSteps.data,
    qt_meta_data_CalculateSteps,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CalculateSteps::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalculateSteps::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CalculateSteps.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CalculateSteps::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void CalculateSteps::numberOfStepsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
