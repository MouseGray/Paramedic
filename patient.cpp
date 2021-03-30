#include "patient.h"

bool Patient::selectNSP(QSqlDatabase &datebase)
{
    QSqlQuery query(datebase);
    QString queryText = QString("SELECT " PATIENT_NAME ", " PATIENT_LASTNAME ", " PATIENT_PATRONYMIC
                                " FROM "  TABLE_PATIENT
                                " WHERE " PATIENT_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id", m_id);

    if(!query.exec()) return false;
    if(!query.next()) return false;

    m_fName = query.value(0).toString();
    m_sName = query.value(1).toString();
    m_pName = query.value(2).toString();

    return true;
}

bool Patient::selectNSPN(QSqlDatabase &datebase)
{
    QSqlQuery query(datebase);
    QString queryText = QString("SELECT " PATIENT_NAME ", " PATIENT_LASTNAME ", " PATIENT_PATRONYMIC ", " PATIENT_PHONENUMBER
                                " FROM "  TABLE_PATIENT
                                " WHERE " PATIENT_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id", m_id);

    if(!query.exec()) return false;
    if(!query.next()) return false;

    m_fName  = query.value(0).toString();
    m_sName  = query.value(1).toString();
    m_pName  = query.value(2).toString();
    m_number = query.value(3).toString();
    return true;
}

bool Patient::selectAll(QSqlDatabase &datebase)
{
    QSqlQuery query(datebase);
    QString queryText = QString("SELECT " PATIENT_NAME ", " PATIENT_LASTNAME ", " PATIENT_PATRONYMIC ", " PATIENT_PHONENUMBER ", " PATIENT_GENDER ", " PATIENT_DATEOFBIRTH
                                " FROM "  TABLE_PATIENT
                                " WHERE " PATIENT_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id", m_id);

    if(!query.exec()) return false;
    if(!query.next()) return false;

    m_fName       =                     query.value(0).toString();
    m_sName       =                     query.value(1).toString();
    m_pName       =                     query.value(2).toString();
    m_number      =                     query.value(3).toString();
    m_gender      = static_cast<Gender>(query.value(4).toInt());
    m_dateOfBirth =                     query.value(5).toDate();
    return true;
}

bool Patient::updateAll(QSqlDatabase &datebase)
{
    QSqlQuery query(datebase);
    QString queryText = QString("UPDATE " TABLE_PATIENT
                                " SET " PATIENT_NAME        " = :name, "
                                        PATIENT_LASTNAME    " = :lastName, "
                                        PATIENT_PATRONYMIC  " = :patronymic, "
                                        PATIENT_PHONENUMBER " = :number, "
                                        PATIENT_GENDER      " = :gender, "
                                        PATIENT_DATEOFBIRTH " = :date"
                                " WHERE " PATIENT_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id",                          m_id                              );
    query.bindValue(":name",                        m_fName                           );
    query.bindValue(":lastName",                    m_sName                           );
    query.bindValue(":patronymic",                  m_pName                           );
    query.bindValue(":number",                      m_number                          );
    query.bindValue(":gender",     static_cast<int>(m_gender)                         );
    query.bindValue(":date",                        m_dateOfBirth.toString("dd-MM-yy"));

    if(!query.exec()) return false;

    return query.numRowsAffected() != 0;
}
