/****************************************************************************
** Meta object code from reading C++ file 'EventQtSlotConnect.h'
**
** Created: Thu Jun 6 17:23:59 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../EventQtSlotConnect.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EventQtSlotConnect.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EventQtSlotConnect[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   20,   19,   19, 0x0a,
      73,   67,   19,   19, 0x0a,
     109,   19,   19,   19, 0x0a,
     116,   19,   19,   19, 0x0a,
     123,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EventQtSlotConnect[] = {
    "EventQtSlotConnect\0\0,,,\0"
    "slot_clicked(vtkObject*,ulong,void*,void*)\0"
    "x,y,z\0slot_position(double,double,double)\0"
    "open()\0save()\0executeCmrepVskel()\0"
};

void EventQtSlotConnect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EventQtSlotConnect *_t = static_cast<EventQtSlotConnect *>(_o);
        switch (_id) {
        case 0: _t->slot_clicked((*reinterpret_cast< vtkObject*(*)>(_a[1])),(*reinterpret_cast< ulong(*)>(_a[2])),(*reinterpret_cast< void*(*)>(_a[3])),(*reinterpret_cast< void*(*)>(_a[4]))); break;
        case 1: _t->slot_position((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 2: _t->open(); break;
        case 3: _t->save(); break;
        case 4: _t->executeCmrepVskel(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EventQtSlotConnect::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EventQtSlotConnect::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_EventQtSlotConnect,
      qt_meta_data_EventQtSlotConnect, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EventQtSlotConnect::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EventQtSlotConnect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EventQtSlotConnect::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EventQtSlotConnect))
        return static_cast<void*>(const_cast< EventQtSlotConnect*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int EventQtSlotConnect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
