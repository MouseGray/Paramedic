#include "doctor.h"

bool Doctor::selectNSP(QSqlDatabase &datebase)
{
    QSqlQuery query(datebase);
    QString queryText = QString("SELECT " DOCTOR_NAME ", " DOCTOR_LASTNAME ", " DOCTOR_PATRONYMIC
                                " FROM "  TABLE_DOCTOR
                                " WHERE " DOCTOR_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id", m_id);

    if(!query.exec()) return false;
    if(!query.next()) return false;

    m_fName = query.value(0).toString();
    m_sName = query.value(1).toString();
    m_pName = query.value(2).toString();

    return true;
}

bool Doctor::selectNSPN(QSqlDatabase &datebase)
{
    QSqlQuery query(datebase);
    QString queryText = QString("SELECT " DOCTOR_NAME ", " DOCTOR_LASTNAME ", " DOCTOR_PATRONYMIC ", " DOCTOR_PHONENUMBER
                                " FROM "  TABLE_DOCTOR
                                " WHERE " DOCTOR_ID " = :id");
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

bool Doctor::selectAll(QSqlDatabase &datebase)
{
    QSqlQuery query(datebase);
    QString queryText = QString("SELECT " DOCTOR_NAME ", " DOCTOR_LASTNAME ", " DOCTOR_PATRONYMIC ", " DOCTOR_PHONENUMBER ", " SPECIALIZATION_NAME
                                " FROM "  TABLE_DOCTOR
                                " LEFT JOIN " TABLE_SPECIALIZATION " ON " DOCTOR_SPECIALIZATION " = " SPECIALIZATION_ID
                                " WHERE " DOCTOR_ID " = :id;");
    query.prepare(queryText);
    query.bindValue(":id", m_id);

    if(!query.exec()) return false;
    if(!query.next()) return false;

    m_fName  = query.value(0).toString();
    m_sName  = query.value(1).toString();
    m_pName  = query.value(2).toString();
    m_number = query.value(3).toString();
    m_spec   = query.value(4).toString();
    return true;
}

bool Doctor::updateAll(QSqlDatabase &datebase)
{
    QSqlQuery query(datebase);
    QString queryText = QString("UPDATE " TABLE_DOCTOR
                                " SET " DOCTOR_NAME           " = :name, "
                                        DOCTOR_LASTNAME       " = :lastName, "
                                        DOCTOR_PATRONYMIC     " = :patronymic, "
                                        DOCTOR_PHONENUMBER    " = :number, "
                                        DOCTOR_SPECIALIZATION " = "
                                    "(SELECT " SPECIALIZATION_ID
                                    " FROM " TABLE_SPECIALIZATION
                                    " WHERE " SPECIALIZATION_NAME " = :specialization)"
                                " WHERE " DOCTOR_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id",              m_id    );
    query.bindValue(":name",            m_fName );
    query.bindValue(":lastName",        m_sName );
    query.bindValue(":patronymic",      m_pName );
    query.bindValue(":number",          m_number);
    query.bindValue(":specialization",  m_spec  );

    if(!query.exec()) return false;

    return query.numRowsAffected() != 0;
}
