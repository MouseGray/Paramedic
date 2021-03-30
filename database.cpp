#include "database.h"

Database::Database(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QODBC");
}

void Database::initialize(const QString &host, const QString &database, const QString &login, const QString &password)
{
    QString settings = QString("DRIVER={SQL Server}; "
                               "SERVER=%1;"
                               "DATABASE=%2;")
            .arg(host)
            .arg(database);
    m_db.setDatabaseName(settings);
    m_db.setUserName(login);
    m_db.setPassword(password);
}

bool Database::addDoctor(QString name, QString lastName, QString patronymic, QString number, QString specialization)
{
    QSqlQuery query(m_db);
    QString queryText = QString("INSERT INTO " TABLE_DOCTOR
                                "(" DOCTOR_NAME ", " DOCTOR_LASTNAME ", " DOCTOR_PATRONYMIC ", " DOCTOR_PHONENUMBER ", " DOCTOR_SPECIALIZATION")"
                                " VALUES (:name, :lastName, :patronymic, :number, "
                                    "(SELECT " SPECIALIZATION_ID
                                    " FROM " TABLE_SPECIALIZATION
                                    " WHERE " SPECIALIZATION_NAME " = :specialization)"
                                ")");
    query.prepare(queryText);
    query.bindValue(":name",            name);
    query.bindValue(":lastName",        lastName);
    query.bindValue(":patronymic",      patronymic);
    query.bindValue(":number",          number);
    query.bindValue(":specialization",  specialization);

    if(!query.exec()) return false;

    return query.numRowsAffected() != 0;
}

bool Database::addPatient(QString name, QString lastName, QString patronymic, QString number, bool gender, QString dateOfBirth)
{
    QSqlQuery query(m_db);
    QString queryText = QString("INSERT INTO " TABLE_PATIENT
                                "(" PATIENT_NAME ", " PATIENT_LASTNAME ", " PATIENT_PATRONYMIC ", " PATIENT_PHONENUMBER ", " PATIENT_GENDER ", " PATIENT_DATEOFBIRTH")"
                                " VALUES (:name, :lastName, :patronymic, :number, :gender, :date)");
    query.prepare(queryText);
    query.bindValue(":name",            name);
    query.bindValue(":lastName",        lastName);
    query.bindValue(":patronymic",      patronymic);
    query.bindValue(":number",          number);
    query.bindValue(":gender",          int(gender));
    query.bindValue(":date",            dateOfBirth);

    if(!query.exec()) return false;

    return query.numRowsAffected() != 0;
}

bool Database::saveDoctor(const Doctor &doctor)
{
    QSqlQuery query(m_db);
    QString queryText = QString("UPDATE " TABLE_DOCTOR
                                " SET " DOCTOR_NAME " = :name, "
                                        DOCTOR_LASTNAME " = :lastName, "
                                        DOCTOR_PATRONYMIC " = :patronymic, "
                                        DOCTOR_PHONENUMBER " = :number, "
                                        DOCTOR_SPECIALIZATION " = "
                                    "(SELECT " SPECIALIZATION_ID
                                    " FROM " TABLE_SPECIALIZATION
                                    " WHERE " SPECIALIZATION_NAME " = :specialization)"
                                " WHERE " DOCTOR_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id",              doctor.ID());
    query.bindValue(":name",            doctor.name());
    query.bindValue(":lastName",        doctor.lastName());
    query.bindValue(":patronymic",      doctor.patronymic());
    query.bindValue(":number",          doctor.number());
    query.bindValue(":specialization",  doctor.specialization());

    if(!query.exec()) return false;

    return query.numRowsAffected() != 0;
}

bool Database::savePatient(const Patient &patient)
{
    QSqlQuery query(m_db);
    QString queryText = QString("UPDATE " TABLE_PATIENT
                                " SET " PATIENT_NAME " = :name, "
                                        PATIENT_LASTNAME " = :lastName, "
                                        PATIENT_PATRONYMIC " = :patronymic, "
                                        PATIENT_PHONENUMBER " = :number, "
                                        PATIENT_GENDER " = :gender, "
                                        PATIENT_DATEOFBIRTH " = :date"
                                " WHERE " PATIENT_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id",              patient.ID());
    query.bindValue(":name",            patient.name());
    query.bindValue(":lastName",        patient.lastName());
    query.bindValue(":patronymic",      patient.patronymic());
    query.bindValue(":number",          patient.number());
    query.bindValue(":gender",      int(patient.gender()));
    query.bindValue(":date",            patient.dateOfBirth().toString("dd-MM-yy"));

    if(!query.exec()) return false;

    return query.numRowsAffected() != 0;
}

void Database::fillSpecialzation(QSqlQueryModel *model)
{
     model->setQuery("SELECT " SPECIALIZATION_NAME
                     " FROM " TABLE_SPECIALIZATION ";");
}

void Database::fillView(PageType type, QString searchText, int page, QSqlQueryModel *model)
{
    QString queryText;
    switch(type){
    case PageType::DoctorPage:
        queryText = QString("SELECT " DOCTOR_ID ", CONCAT(" DOCTOR_LASTNAME ", ' ', " DOCTOR_NAME ", ' ', " DOCTOR_PATRONYMIC ") [N], " SPECIALIZATION_NAME
                                    " FROM " TABLE_DOCTOR ", " TABLE_SPECIALIZATION
                                    " WHERE " DOCTOR_SPECIALIZATION " = " SPECIALIZATION_ID " AND " DOCTOR_LASTNAME " LIKE '%1%'"
                                    " ORDER BY N"
                                    " OFFSET %2 ROWS"
                                    " FETCH NEXT %3 ROWS ONLY")
                .arg(searchText)
                .arg((page - 1)*PAGESIZE)
                .arg(PAGESIZE);

        model->setQuery(queryText);
        model->setHeaderData(0, Qt::Horizontal, RUS("ID"));
        model->setHeaderData(1, Qt::Horizontal, RUS("ФИО"));
        model->setHeaderData(2, Qt::Horizontal, RUS("Специальность"));
        break;
    case PageType::PatientPage:
        queryText = QString("SELECT " PATIENT_ID ", CONCAT(" PATIENT_LASTNAME ", ' ', " PATIENT_NAME ", ' ', " PATIENT_PATRONYMIC ") [N], " PATIENT_DATEOFBIRTH
                                    " FROM " TABLE_PATIENT
                                    " WHERE " PATIENT_LASTNAME " LIKE '%1%'"
                                    " ORDER BY N"
                                    " OFFSET %2 ROWS"
                                    " FETCH NEXT %3 ROWS ONLY")
                .arg(searchText)
                .arg((page - 1)*PAGESIZE)
                .arg(PAGESIZE);

        model->setQuery(queryText);
        model->setHeaderData(0, Qt::Horizontal, RUS("ID"));
        model->setHeaderData(1, Qt::Horizontal, RUS("ФИО"));
        model->setHeaderData(2, Qt::Horizontal, RUS("Дата рождения"));
        break;
    }
}

void Database::fillEditor(PageType type, QString searchText, int personid, int page, QSqlQueryModel *model)
{
    QString queryText;
    switch(type){
    case PageType::DoctorPage:
        queryText = QString("SELECT " PATIENT_ID ", CONCAT(" PATIENT_LASTNAME ", ' ', " PATIENT_NAME ", ' ', " PATIENT_PATRONYMIC ") [NSP], " PATIENT_DATEOFBIRTH
                            " FROM " TABLE_PATIENT
                            " JOIN "
                                " (SELECT " DIAGNOSIS_PATIENTID
                                " FROM " TABLE_DIAGNOSIS
                                " WHERE " DIAGNOSIS_DIACNOSTICANID " = %1 OR " DIAGNOSIS_DOCTORID " = %1"
                                " GROUP BY " DIAGNOSIS_PATIENTID ") AS T"
                            " ON " PATIENT_ID " = T.PATIENT_ID"
                            " WHERE " PATIENT_LASTNAME " LIKE '%2%'"
                            " ORDER BY NSP"
                            " OFFSET %3 ROWS"
                            " FETCH NEXT %4 ROWS ONLY")
                .arg(personid)
                .arg(searchText)
                .arg((page - 1)*PAGESIZE)
                .arg(PAGESIZE);

        model->setQuery(queryText);

        model->setHeaderData(0, Qt::Horizontal, RUS("ID"));
        model->setHeaderData(1, Qt::Horizontal, RUS("ФИО"));
        model->setHeaderData(2, Qt::Horizontal, RUS("Дата рождения"));

        break;
    case PageType::PatientPage:
        queryText = QString("SELECT " DIAGNOSIS_ID ", " DIAGNOSIS_COMPLAINTS ", " DIAGNOSIS_STARTTHERAPY
                                    " FROM " TABLE_DIAGNOSIS
                                    " WHERE " DIAGNOSIS_PATIENTID " = %1"
                                        " AND " DIAGNOSIS_COMPLAINTS " LIKE '%%2%'"
                                    " ORDER BY " DIAGNOSIS_STARTTHERAPY
                                    " OFFSET %3 ROWS"
                                    " FETCH NEXT %4 ROWS ONLY")
                .arg(personid)
                .arg(searchText)
                .arg((page - 1)*PAGESIZE)
                .arg(PAGESIZE);

        model->setQuery(queryText);

        model->setHeaderData(0, Qt::Horizontal, RUS("ID"));
        model->setHeaderData(1, Qt::Horizontal, RUS("Жалобы"));
        model->setHeaderData(2, Qt::Horizontal, RUS("Дата"));

        break;
    }
}

int Database::getPageCount(PageType type, QString searchText)
{
    QString queryText = QString("SELECT COUNT(*)"
                                " FROM %1"
                                " WHERE %2 LIKE :search;")
            .arg((type == PageType::DoctorPage ? TABLE_DOCTOR : TABLE_PATIENT))
            .arg((type == PageType::DoctorPage ? DOCTOR_LASTNAME : PATIENT_LASTNAME));

    QSqlQuery query(m_db);
    query.prepare(queryText);
    query.bindValue(":search", searchText + "%");

    if(!query.exec()) return 0;
    if(!query.next()) return 0;

    return qMax(query.value(0).toInt() - 1, 0)/PAGESIZE;
}

int Database::getEditorPageCount(PageType type, int id, QString searchText)
{
    QString queryText;
    switch(type)
    {
    case PageType::DoctorPage:
        queryText = QString("SELECT COUNT(*) "
                            " FROM " TABLE_PATIENT ", " TABLE_DIAGNOSIS
                            " WHERE " PATIENT_ID " = " DIAGNOSIS_PATIENTID
                                " AND (" DIAGNOSIS_DOCTORID " = :id OR " DIAGNOSIS_DIACNOSTICANID " = :id)"
                                " AND " PATIENT_LASTNAME " LIKE :search;");
        break;
    case PageType::PatientPage:
        queryText = QString("SELECT COUNT(*) "
                            " FROM " TABLE_DIAGNOSIS
                            " WHERE " DIAGNOSIS_PATIENTID " = :id"
                                " AND " DIAGNOSIS_COMPLAINTS " LIKE :search;");
        break;
    }
    QSqlQuery query(m_db);
    query.prepare(queryText);
    query.bindValue(":id", id);
    query.bindValue(":search", "%" + searchText + "%");

    if(!query.exec()) return 0;
    if(!query.next()) return 0;

    return qMax(query.value(0).toInt() - 1, 0)/PAGESIZE;
}

bool Database::getDoctorStruct(Doctor *doctor)
{
    QSqlQuery query(m_db);
    QString queryText = QString("SELECT " DOCTOR_NAME ", " DOCTOR_LASTNAME ", " DOCTOR_PATRONYMIC ", " SPECIALIZATION_NAME ", " DOCTOR_PHONENUMBER
                                " FROM "  TABLE_DOCTOR ", " TABLE_SPECIALIZATION
                                " WHERE " DOCTOR_SPECIALIZATION " = " SPECIALIZATION_ID " AND " DOCTOR_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id", doctor->ID());

    if(!query.exec()) return false;
    if(!query.next()) return false;

    doctor->setName            (query.value(0).toString());
    doctor->setLastName        (query.value(1).toString());
    doctor->setPatronymic      (query.value(2).toString());
    doctor->setSpecialization  (query.value(3).toString());
    doctor->setNumber          (query.value(4).toString());

    return true;
}

bool Database::getPatientStruct(Patient *patient)
{
    QSqlQuery query(m_db);
    QString queryText = QString("SELECT " PATIENT_NAME ", " PATIENT_LASTNAME ", " PATIENT_PATRONYMIC ", " PATIENT_PHONENUMBER ", " PATIENT_GENDER ", " PATIENT_DATEOFBIRTH
                                " FROM "  TABLE_PATIENT
                                " WHERE " PATIENT_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id", patient->ID());

    if(!query.exec()) return false;
    if(!query.next()) return false;

    patient->setName            (query.value(0).toString()     );
    patient->setLastName        (query.value(1).toString()     );
    patient->setPatronymic      (query.value(2).toString()     );
    patient->setNumber          (query.value(3).toString()     );
    patient->setGender          (Patient::Gender(query.value(4).toInt()));
    patient->setDateOfBirth     (query.value(5).toDate()       );

    return true;
}

bool Database::getDiagnosisStruct(DiagnosisStruct *diagnosis)
{
    QSqlQuery query(m_db);
    QString queryText = QString("SELECT " DIAGNOSIS_COMPLAINTS ", " DIAGNOSIS_DESCRIPTION ", " DIAGNOSIS_STARTTHERAPY ", " DIAGNOSIS_ENDTHERAPY ", "
                                DIAGNOSIS_STATUS ", " DIAGNOSIS_PATIENTID ", " DIAGNOSIS_DIACNOSTICANID ", " DIAGNOSIS_DOCTORID
                                " FROM "  TABLE_DIAGNOSIS
                                " WHERE " DIAGNOSIS_ID " = :id");
    query.prepare(queryText);
    query.bindValue(":id", diagnosis->id);

    if(!query.exec()) return false;
    if(!query.next()) return false;

    diagnosis->complaints      = query.value(0).toString();
    diagnosis->description     = query.value(1).toString();
    diagnosis->startTherapy    = query.value(2).toDate();
    diagnosis->endTherapy      = query.value(3).toDate();
    diagnosis->status          = DiagnosisStatus(query.value(4).toInt());
    diagnosis->patient.setID      (query.value(5).toInt());
    diagnosis->diagnostican.setID (query.value(6).toInt());
    diagnosis->doctor.setID       (query.value(7).toInt());

    getPatientStruct(&diagnosis->patient);
    getDoctorStruct (&diagnosis->diagnostican);
    getDoctorStruct (&diagnosis->doctor);

    return true;
}

bool Database::createDiagnosis(DiagnosisStruct &diagnosis)
{
    QSqlQuery query(m_db);
    QString queryText = QString("DECLARE @id AS INT;"
                                "EXEC createDiagnosis :complaints, :starttherapy, :status, :diagnostican, :patient, @id OUTPUT;"
                                "SELECT @id;");
    query.prepare(queryText);
    query.bindValue(":complaints",   diagnosis.complaints);
    query.bindValue(":starttherapy", diagnosis.startTherapy.toString("dd.MM.yy"));
    query.bindValue(":diagnostican", diagnosis.diagnostican.ID());
    query.bindValue(":status",       int(diagnosis.status));
    query.bindValue(":patient",      diagnosis.patient.ID());

    if(!query.exec()) return false;
    if(!query.next()) return false;

    diagnosis.id = query.value(0).toInt();

    return true;
}

bool Database::setDiagnosis(DiagnosisStruct &diagnosis)
{
    QSqlQuery query(m_db);
    QString queryText = QString("UPDATE DIAGNOSIS "
                                "SET description = :description, DOCTOR_ID = :doctor, status = :status "
                                "WHERE id = :id;");
    query.prepare(queryText);
    query.bindValue(":description",  diagnosis.description);
    query.bindValue(":doctor",       diagnosis.doctor.ID());
    query.bindValue(":status",       int(diagnosis.status));
    query.bindValue(":id",           diagnosis.id);

    if(!query.exec()) return false;

    return query.numRowsAffected() != 0;
}

bool Database::releaseDiagnosis(DiagnosisStruct &diagnosis)
{
    QSqlQuery query(m_db);
    QString queryText = QString("UPDATE DIAGNOSIS "
                                "SET end_of_therapy = :date, status = :status "
                                "WHERE id = :id;");
    query.prepare(queryText);
    query.bindValue(":date",         diagnosis.endTherapy.toString("dd.MM.yy"));
    query.bindValue(":status",       int(diagnosis.status));
    query.bindValue(":id",           diagnosis.id);

    if(!query.exec()) return false;

    return query.numRowsAffected() != 0;
}
