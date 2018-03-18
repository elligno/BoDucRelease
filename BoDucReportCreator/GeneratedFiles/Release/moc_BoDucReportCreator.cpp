/****************************************************************************
** Meta object code from reading C++ file 'BoDucReportCreator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../BoDucReportCreator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BoDucReportCreator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_bdApp__BoDucReportCreator_t {
    QByteArrayData data[13];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_bdApp__BoDucReportCreator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_bdApp__BoDucReportCreator_t qt_meta_stringdata_bdApp__BoDucReportCreator = {
    {
QT_MOC_LITERAL(0, 0, 25), // "bdApp::BoDucReportCreator"
QT_MOC_LITERAL(1, 26, 8), // "parseCmd"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 13), // "saveCmdToFile"
QT_MOC_LITERAL(4, 50, 15), // "loadCmdFromFile"
QT_MOC_LITERAL(5, 66, 11), // "OpenCsvFile"
QT_MOC_LITERAL(6, 78, 8), // "savetest"
QT_MOC_LITERAL(7, 87, 4), // "open"
QT_MOC_LITERAL(8, 92, 14), // "currentUniteON"
QT_MOC_LITERAL(9, 107, 15), // "createBonReport"
QT_MOC_LITERAL(10, 123, 13), // "testItemClick"
QT_MOC_LITERAL(11, 137, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(12, 155, 5) // "aItem"

    },
    "bdApp::BoDucReportCreator\0parseCmd\0\0"
    "saveCmdToFile\0loadCmdFromFile\0OpenCsvFile\0"
    "savetest\0open\0currentUniteON\0"
    "createBonReport\0testItemClick\0"
    "QTableWidgetItem*\0aItem"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_bdApp__BoDucReportCreator[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    0,   60,    2, 0x0a /* Public */,
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    0,   64,    2, 0x0a /* Public */,
       8,    0,   65,    2, 0x0a /* Public */,
       9,    0,   66,    2, 0x0a /* Public */,
      10,    1,   67,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,

       0        // eod
};

void bdApp::BoDucReportCreator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BoDucReportCreator *_t = static_cast<BoDucReportCreator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->parseCmd(); break;
        case 1: _t->saveCmdToFile(); break;
        case 2: _t->loadCmdFromFile(); break;
        case 3: _t->OpenCsvFile(); break;
        case 4: _t->savetest(); break;
        case 5: _t->open(); break;
        case 6: _t->currentUniteON(); break;
        case 7: _t->createBonReport(); break;
        case 8: _t->testItemClick((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject bdApp::BoDucReportCreator::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_bdApp__BoDucReportCreator.data,
      qt_meta_data_bdApp__BoDucReportCreator,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *bdApp::BoDucReportCreator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *bdApp::BoDucReportCreator::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_bdApp__BoDucReportCreator.stringdata0))
        return static_cast<void*>(const_cast< BoDucReportCreator*>(this));
    return QWidget::qt_metacast(_clname);
}

int bdApp::BoDucReportCreator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
