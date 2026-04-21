/****************************************************************************
** Meta object code from reading C++ file 'Dashboard.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/Dashboard.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Dashboard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9DashboardE_t {};
} // unnamed namespace

template <> constexpr inline auto Dashboard::qt_create_metaobjectdata<qt_meta_tag_ZN9DashboardE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Dashboard",
        "simulationStartRequested",
        "",
        "simulationPaused",
        "simulationStopped",
        "simulationReset",
        "stepRequested",
        "speedChanged",
        "steps_per_second",
        "schedulerChanged",
        "algorithm",
        "memoryAlgorithmChanged",
        "timeQuantumChanged",
        "quantum",
        "onStartSimulation",
        "onPauseSimulation",
        "onStopSimulation",
        "onResetSimulation",
        "onStepSimulation",
        "onSpeedChanged",
        "speed",
        "onSchedulerChanged",
        "index",
        "onMemoryAlgorithmChanged",
        "onTimeQuantumChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'simulationStartRequested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'simulationPaused'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'simulationStopped'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'simulationReset'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stepRequested'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'speedChanged'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'schedulerChanged'
        QtMocHelpers::SignalData<void(int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Signal 'memoryAlgorithmChanged'
        QtMocHelpers::SignalData<void(int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Signal 'timeQuantumChanged'
        QtMocHelpers::SignalData<void(int)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'onStartSimulation'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onPauseSimulation'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onStopSimulation'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onResetSimulation'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onStepSimulation'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSpeedChanged'
        QtMocHelpers::SlotData<void(int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 20 },
        }}),
        // Slot 'onSchedulerChanged'
        QtMocHelpers::SlotData<void(int)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 22 },
        }}),
        // Slot 'onMemoryAlgorithmChanged'
        QtMocHelpers::SlotData<void(int)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 22 },
        }}),
        // Slot 'onTimeQuantumChanged'
        QtMocHelpers::SlotData<void(int)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Dashboard, qt_meta_tag_ZN9DashboardE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Dashboard::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9DashboardE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9DashboardE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9DashboardE_t>.metaTypes,
    nullptr
} };

void Dashboard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Dashboard *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->simulationStartRequested(); break;
        case 1: _t->simulationPaused(); break;
        case 2: _t->simulationStopped(); break;
        case 3: _t->simulationReset(); break;
        case 4: _t->stepRequested(); break;
        case 5: _t->speedChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->schedulerChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->memoryAlgorithmChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->timeQuantumChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onStartSimulation(); break;
        case 10: _t->onPauseSimulation(); break;
        case 11: _t->onStopSimulation(); break;
        case 12: _t->onResetSimulation(); break;
        case 13: _t->onStepSimulation(); break;
        case 14: _t->onSpeedChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->onSchedulerChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->onMemoryAlgorithmChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->onTimeQuantumChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)()>(_a, &Dashboard::simulationStartRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)()>(_a, &Dashboard::simulationPaused, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)()>(_a, &Dashboard::simulationStopped, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)()>(_a, &Dashboard::simulationReset, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)()>(_a, &Dashboard::stepRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)(int )>(_a, &Dashboard::speedChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)(int )>(_a, &Dashboard::schedulerChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)(int )>(_a, &Dashboard::memoryAlgorithmChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dashboard::*)(int )>(_a, &Dashboard::timeQuantumChanged, 8))
            return;
    }
}

const QMetaObject *Dashboard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Dashboard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9DashboardE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Dashboard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void Dashboard::simulationStartRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Dashboard::simulationPaused()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Dashboard::simulationStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Dashboard::simulationReset()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Dashboard::stepRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Dashboard::speedChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void Dashboard::schedulerChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void Dashboard::memoryAlgorithmChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void Dashboard::timeQuantumChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}
QT_WARNING_POP
