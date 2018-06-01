#include "hatinterface.h"

HatInterface::HatInterface()
{
    mMainWindow = getMainWindow();
}

MainWindow* HatInterface::getMainWindow()
{
    foreach (QWidget *w, QApplication::topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            return qobject_cast<MainWindow *>(mainWin);
    return nullptr;
}

int HatInterface::openDevice(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;

    funcArgs = "(mAddress)\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: Open";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc118_open(address);
        break;
    case 323:
        //to do: change to constant HAT_ID_MCC_134
        nameOfFunc = "134: Open";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        mResponse = mcc134_open(address);
        break;
    default:
        mResponse = RESULT_INVALID_DEVICE;
        break;
    }
    argVals = QString("(%1)").arg(address);
    mStatusString = nameOfFunc + argVals + QString(" [Error = %1]").arg(mResponse);

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    if (mResponse != RESULT_SUCCESS) {
        mMainWindow->setError(mResponse, sStartTime + funcStr);
    } else {
        mMainWindow->addFunction(sStartTime + funcStr);
    }

    return mResponse;
}

bool HatInterface::deviceIsOpen(uint16_t devType, uint8_t address)
{
    QString nameOfFunc, funcArgs, argVals, funcStr;
    QTime t;
    QString sStartTime;
    int isOpen;

    funcArgs = "(address) = result\n";
    switch (devType) {
    case HAT_ID_MCC_118:
        nameOfFunc = "118: IsOpen";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc118_is_open(address);
        break;
    case 323:
        //to do: change to constant
        nameOfFunc = "134: IsOpen";
        sStartTime = t.currentTime().toString("hh:mm:ss.zzz") + "~";
        isOpen = mcc134_is_open(address);
        break;
    default:
        isOpen = 0;
        break;
    }

    argVals = QStringLiteral("(%1) = %2")
                .arg(address).arg(isOpen);
    mStatusString = nameOfFunc + argVals;

    funcStr = nameOfFunc + funcArgs + "Arg vals: " + argVals;
    mMainWindow->addFunction(sStartTime + funcStr);
    return isOpen != 0;
}

int HatInterface::getFirmwareVersion(uint16_t devType, uint8_t address, double *version, double *boot)
{

}
