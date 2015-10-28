#ifndef XYZ_LOGGER_H
#define XYZ_LOGGER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class XYZLogger : public QObject
{
    Q_OBJECT
public:
    explicit QxyzLogger(QObject *parent, QString fileName, QPlainTextEdit *editor = 0);
    ~QxyzLogger();
    void setShowDateTime(bool value);

private:
    QFile *file;
    QPlainTextEdit *m_editor;
    bool m_showDate;

signals:

public slots:
    void write(const QString &value);

};

#endif // XYZ_LOGGER_H
