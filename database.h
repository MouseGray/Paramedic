#ifndef DATABASE_H
#define DATABASE_H

#include <QDebug>

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QtGlobal>

#include "main.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    void initialize(const QString &host, const QString &database, const QString &login, const QString &password);
    inline bool open() { return m_db.open(); };
    inline void close() { m_db.close(); }

    bool addDoctor (QString name, QString lastName, QString patronymic, QString number, QString specialization);
    bool addPatient(QString name, QString lastName, QString patronymic, QString number, bool gender, QString dateOfBirth);

    bool saveDoctor (const Doctor& doctor);
    bool savePatient(const Patient& patient);

    void fillSpecialzation(QSqlQueryModel* model);
    void fillView   (PageType type, QString searchText,               int page, QSqlQueryModel* model);
    void fillEditor (PageType type, QString searchText, int personid, int page, QSqlQueryModel* model);

    int getPageCount       (PageType type, QString searchText);
    int getEditorPageCount (PageType type, int id, QString searchText);

    bool getDoctorStruct    (Doctor*      doctor);
    bool getPatientStruct   (Patient*     patient);
    bool getDiagnosisStruct (DiagnosisStruct*   diagnosis);

    bool createDiagnosis    (DiagnosisStruct& diagnosis);
    bool setDiagnosis       (DiagnosisStruct& diagnosis);
    bool releaseDiagnosis   (DiagnosisStruct& diagnosis);

signals:

public:
    QSqlDatabase m_db;
};

#endif // DATABASE_H
