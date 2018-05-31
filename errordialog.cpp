#include "errordialog.h"
#include "ui_errordialog.h"

int mError;

ErrorDialog::ErrorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorDialog)
{
    ui->setupUi(this);
    ui->txtErrList->setFont(QFont ("Courier", 10));
    ui->txtErrList->setStyleSheet("QTextEdit { background-color : white; color : blue; }" );
    connect(ui->cmdClearHistory, SIGNAL(clicked(bool)), this, SLOT(clearHistory()));
    mHistorySize = 50;
    mHistoryCleared = false;
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}

void ErrorDialog::setError(int curError, QString funcText)
{
    //Not used
    QString alertHtml = "<font color=\"Red\">";
    QString notifyHtml = "<font color=\"Blue\">";
    QString infoHtml = "<font color=\"Green\">";
    QString endHtml = "</font><br>";
    QString curFuncText, argsText, errMsg;
    QStringList splitText;

    splitText = funcText.split("\n");
    curFuncText = splitText.first();
    argsText = splitText.last();
    mError = curError;
    errMsg = getErrorDescription(mError);
    curFuncText = notifyHtml + curFuncText + endHtml;
    curFuncText.append(infoHtml + argsText + endHtml);
    curFuncText.append(alertHtml + QString("%1").arg(errMsg) + endHtml);
    ui->txtErrList->setHtml(curFuncText);
}

void ErrorDialog::showHistory(QStringList historyList)
{
    QStringList funcParts;
    QString fStr, aStr, bStr, eStr, tStr, errText;
    QString alertHtml = "<font color=\"Red\">";
    QString notifyHtml = "<font color=\"Blue\">";
    QString infoHtml = "<font color=\"Green\">";
    QString endHtml = "</font><br>";
    QString str;

    ui->txtErrList->clear();
    for (int i = (historyList.size()- 1); i >= 0; i--) {
        QString curHist = historyList.value(i);
        funcParts = curHist.split("\n");
        aStr = funcParts.first();
        errText = "";
        if (aStr.startsWith("Error")) {
            //error occurred
            eStr = aStr.left(aStr.indexOf("_"));
            aStr.remove(QRegularExpression(eStr + "_"));
            eStr.remove(QRegularExpression("Error"));
            errText = getErrorText(eStr.toInt());
        }
        tStr = aStr.left(aStr.indexOf("~"));
        bStr = aStr.right(aStr.length() - (aStr.indexOf("~") +1));
        fStr.append(notifyHtml + tStr + endHtml);
        fStr.append(notifyHtml + bStr + endHtml);
        fStr.append(infoHtml + funcParts.last() + endHtml);
        if (errText.length())
            fStr.append(errText);
        //ui->txtErrList->append(str.setnum(historyList.value(i));
    }
    ui->txtErrList->setHtml(fStr);
    ui->lblNumListed->setText(QString("List size: %1").arg(historyList.size()));
}

void ErrorDialog::clearHistory()
{
    ui->txtErrList->clear();
    mHistoryCleared = true;
}

void ErrorDialog::setHistSize(int histSize)
{
    mHistorySize = histSize;
}

int ErrorDialog::getHistSize()
{
    return mHistorySize;
}

QString ErrorDialog::getErrorText(int errorNumber)
{
    QString alertHtml = "<font color=\"Red\">";
    QString endHtml = "</font><br>";
    QString errText;

    errText = "";
    QString errMsg;
    errMsg = getErrorDescription(errorNumber);
    errText = alertHtml + QString("[Error %1] %2" + endHtml)
            .arg(errorNumber)
            .arg(errMsg);
    return errText;
}

void ErrorDialog::addFunction(QString funcString)
{
    mFuncHistoryList.append(funcString);
    if (mFuncHistoryList.size() > mHistorySize)
        mFuncHistoryList.removeFirst();
}

void ErrorDialog::on_cmdOK_clicked()
{
    mHistorySize = ui->leHistSize->text().toInt();
    this->close();
}
