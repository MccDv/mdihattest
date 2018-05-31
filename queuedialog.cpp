#include "queuedialog.h"
#include "ui_queuedialog.h"

QueueDialog::QueueDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueueDialog)
{
    ui->setupUi(this);

    ui->lstQueue->setStyleSheet("QListWidget { background-color : white; color : blue; }" );

    /*
    inputFunctions = (utFuncGroup == FUNC_GROUP_AIN);
    ui->cmbRange->addItem("BIP20VOLTS", BIP20VOLTS);
    ui->cmbRange->addItem("BIP10VOLTS", BIP10VOLTS);
    ui->cmbRange->addItem("BIPPT5VOLTS", BIP5VOLTS);
    ui->cmbRange->addItem("BIP4VOLTS", BIP4VOLTS);
    ui->cmbRange->addItem("BIP2PT5VOLTS", BIP2PT5VOLTS);
    ui->cmbRange->addItem("BIP2VOLTS", BIP2VOLTS);
    ui->cmbRange->addItem("BIP1PT25VOLTS", BIP1PT25VOLTS);
    ui->cmbRange->addItem("BIP1VOLTS", BIP1VOLTS);
    ui->cmbRange->addItem("BIPPT078VOLTS", BIPPT078VOLTS);
    ui->cmbRange->addItem("UNI10VOLTS", UNI10VOLTS);
    ui->cmbRange->addItem("UNI5VOLTS", UNI5VOLTS);

    if (inputFunctions) {
        ui->cmbChanType->addItem("Standard", DaqInChanType(0));
        ui->cmbChanType->addItem("AnalogDiff", DAQI_ANALOG_DIFF);
        ui->cmbChanType->addItem("AnalogSE", DAQI_ANALOG_SE);
        ui->cmbChanType->addItem("Digital", DAQI_DIGITAL);
        ui->cmbChanType->addItem("Counter16", DAQI_CTR16);
        ui->cmbChanType->addItem("Counter32", DAQI_CTR32);
        ui->cmbChanType->addItem("Counter48", DAQI_CTR48);

        ui->cmbMode->setVisible(true);
        ui->cmbMode->addItem("Differential", AI_DIFFERENTIAL);
        ui->cmbMode->addItem("Single Ended", AI_SINGLE_ENDED);
    } else {
        ui->cmbChanType->addItem("AnalogOut", DAQO_ANALOG);
        ui->cmbChanType->addItem("DigitalOut", DAQO_DIGITAL);
        ui->cmbMode->setVisible(false);
    }
    */

    connect(ui->spnQElements, SIGNAL(valueChanged(int)), this, SLOT(updateNumElements(int)));
    connect(ui->lstQueue, SIGNAL(currentRowChanged(int)), this, SLOT(lstQueueRowSelected(int)));
    connect(ui->cmdLoad, SIGNAL(clicked(bool)), this, SLOT(onLoadQueue()));
    connect(this, SIGNAL(numQueueElementsChanged(int)), this, SLOT(updateNumElements(int)));
    //connect(ui->cmbChanType, SIGNAL(currentIndexChanged(int)), this, SLOT(chanTypeSelection(int)));
    //connect(ui->cmdAutoFill, SIGNAL(clicked(bool)), this, SLOT(autoFill()));
    connect(ui->cmdDelElement, SIGNAL(clicked(bool)), this, SLOT(deleteElement()));
    connect(ui->cmdSave, SIGNAL(clicked(QAbstractButton*)), this, SLOT(done()));

    ui->cmdLoad->setEnabled(false);
    ui->cmdDelElement->setEnabled(false);
    //ui->cmdAutoFill->setEnabled(false);
}

QueueDialog::~QueueDialog()
{
    delete ui;
}

void QueueDialog::updateNumElements(int newNumElements)
{
    int elementsShown = ui->spnQElements->value();
    if (!elementsShown) {
        //ui->cmbChanType->setEnabled(true);
        //ui->cmbMode->setEnabled(true);
        ui->cmdLoad->setEnabled(false);
    }
    if (!(elementsShown == newNumElements))
        ui->spnQElements->setValue(newNumElements);
    updateQueueList(newNumElements);
    //ui->cmdAutoFill->setEnabled(newNumElements > mRangeList.count());
    mNumQueueElements = newNumElements;
}

void QueueDialog::updateQueueList(int newNumElements)
{
    int numElements = mChanList.count();
    bool itemsRemoved;
    QString elementDescription;

    itemsRemoved = (newNumElements < numElements);
    //daqParams = true;
    //if (inputFunctions)
    //    daqParams = (!(ui->cmbChanType->currentData()).toInt());
    ui->lstQueue->clear();
    for (int i = 0; i < newNumElements; i++) {
        if (i < numElements) {
            if (mChanList.value(i) < 0)
                elementDescription = QString("%1: channel -1").arg(i);
            else {
                elementDescription = QString("%1: channel %2")
                    .arg(i)
                    .arg(mChanList.value(i));
            }
            ui->lstQueue->addItem(elementDescription);
        } else {
            elementDescription = QString("%1: channel -1").arg(i);
            ui->lstQueue->addItem(elementDescription);
            mChanList.insert(i, -1);
        }
    }
    for (int i = newNumElements; i < numElements; i++) {
        mChanList.remove(i);
    }
    if (itemsRemoved && newNumElements) {
        mNumQueueElements = newNumElements;
        ui->lstQueue->setCurrentRow(newNumElements - 1);
        mCurElement = newNumElements - 1;
    }
    //removed elements with key of "-1"
    mChanList.remove(-1);
}

void QueueDialog::lstQueueRowSelected(int rowSelected)
{
    mCurElement = rowSelected;
    ui->cmdLoad->setEnabled(!(rowSelected < 0));
    if (!isAutoSet) {
        if (mChanList.count()) {
            if (!(mChanList[mCurElement]<0)) {
                ui->spnQChan->setValue(mChanList[mCurElement]);
            }
        }
    }
}

void QueueDialog::onLoadQueue()
{
    int chanVal = ui->spnQChan->value();

    QString curElementStr = QString("%1: channel %2")
            .arg(mCurElement)
            .arg(chanVal);
    ui->lstQueue->currentItem()->setText(curElementStr);
    mChanList.insert(mCurElement, chanVal);
}

void QueueDialog::autoFill()
{
    int curQSize, curElements, startChan;
    int chanIndex;

    isAutoSet = true;
    int increment = ui->spnAutoInc->value();
    chanIndex = 0;
    curQSize = 0;
    curElements = ui->spnQElements->value();
    startChan = ui->spnQChan->value();
    for (int element = curQSize; element < curElements; element++) {
        ui->spnQChan->setValue(startChan + chanIndex);
        ui->lstQueue->setCurrentRow(element);
        onLoadQueue();
        chanIndex += increment;
    }
    isAutoSet = false;
    ui->cmdAutoFill->setEnabled(false);
}

void QueueDialog::deleteElement()
{
    int selElement;
    int elementCount;

    elementCount = ui->spnQElements->value();
    selElement = ui->lstQueue->currentIndex().row();
    if (!(selElement<0)) {
        mChanList.remove(selElement);
    }
    elementCount -= 1;
    updateNumElements(elementCount);
}

void QueueDialog::done()
{
    int numSet = ui->spnQElements->value();
    mNumQueueElements = numSet;
    mQueueEnabled = true;
    if(numSet == 0)
        mQueueEnabled = false;
    mChanList.remove(-1);
}
