/****************************************************************************
** Meta object code from reading C++ file 'WindowTest.hpp'
**
** Created: Wed 23. Nov 03:05:32 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "WindowTest.hpp"

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WindowTest.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WindowTest[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

	   // slots: signature, parameters, type, tag, flags
	   12,    10,    10,    10, 0x0a,
	   33,    10,    10,    10, 0x0a,
	   55,    10,    10,    10, 0x0a,
	   76,    10,    10,    10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WindowTest[] = {
	"WindowTest\0\0"
	"toggleReference(int)\0"
	"slaveToReference(int)\0"
	"changeReference(int)\0"
	"addReference()\0"
};

const QMetaObject WindowTest::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_WindowTest,
      qt_meta_data_WindowTest, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WindowTest::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WindowTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WindowTest::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WindowTest))
        return static_cast<void*>(const_cast< WindowTest*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int WindowTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
			case 0: toggleReference(*reinterpret_cast<int*>(_a[1])); break;
			case 1: slaveToReference(*reinterpret_cast<int*>(_a[1])); break;
			case 2: changeReference(*reinterpret_cast<int*>(_a[1])); break;
			case 3: addReference(); break;
			default: break;
        }
        //_id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
