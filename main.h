#ifndef MAIN_H
#define MAIN_H

#define PAGESIZE 8
#define TOTEXT(var) #var

#define RUS(text) QString::fromLocal8Bit(text)

#include <QString>
#include <QDate>

#include "patient.h"
#include "doctor.h"

enum class PageType{
    DoctorPage,
    PatientPage
};

enum class SelectType{
    Edit,
    Select
};

enum DiagnosisStatus {
    No,
    Diagnostics,
    Treatment,
    Completed,
    Closed,
    ErrorClosed
};

struct DiagnosisStruct {
    int             id;
    QString         complaints;
    QString         description;
    QDate           startTherapy;
    QDate           endTherapy;
    Patient         patient;
    Doctor          diagnostican;
    Doctor          doctor;
    DiagnosisStatus status;
};

#endif // MAIN_H
