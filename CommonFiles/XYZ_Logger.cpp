#include "XYZ_Logger.h"

/*!
    \class XYZLogger
    \inmodule XYZ_Logger.h
    \brief The XYZLogger class is a object
           helps to log custom messages. Messages
           can save to a file or displays at QPlainTextEdit.
 */

/*!
 \fn XYZLogger::XYZLogger(QObject *parent, QString fileName,
               QPlainTextEdit *editor) : QObject(parent)
    Constructs a XYZLogger object with the given \a parent.
*/
XYZLogger::XYZLogger(QObject *parent, QString fileName,
               QPlainTextEdit *editor) : QObject(parent) {

    m_editor = editor;
    m_showDate = true;
    if (!fileName.isEmpty()) {
        file = new QFile;
        file->setFileName(fileName);
        file->open(QIODevice::Append | QIODevice::Text);
    }
}

/*!
 \fn XYZLogger::write(const QString &value)
  XYZLogger::write write a \a value to the logger
 */
void XYZLogger::write(const QString &value) {
    QString text = value;// + "\n";
    if (m_showDate)
        text = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + text;
    QTextStream out(file);
    out.setCodec("UTF-8");
    if (file != 0) {
        out << text;
    }
    if (m_editor != 0)
        m_editor->appendPlainText(text);
}

/*!
 \fn XYZLogger::setShowDateTime(bool value)
 XYZLogger::setShowDateTime sets \a value show or not date and time during logging
 */
void XYZLogger::setShowDateTime(bool value) {
    m_showDate = value;
}

//! Destructor
XYZLogger::~XYZLogger() {
    if (file != 0)
        file->close();
}
