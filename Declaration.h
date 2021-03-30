#ifndef DECLARATION_H
#define DECLARATION_H

#define TABLE_DOCTOR                "DOCTOR"
#define TABLE_PATIENT               "PATIENT"
#define TABLE_DIAGNOSIS             "DIAGNOSIS"
#define TABLE_SPECIALIZATION        "SPECIALIZATION"
#define TABLE_THERAPY               "THERAPY"

#define DOCTOR_ID                   "DOCTOR.ID"
#define DOCTOR_NAME                 "DOCTOR.first_name"
#define DOCTOR_LASTNAME             "DOCTOR.last_name"
#define DOCTOR_PATRONYMIC           "DOCTOR.patronymic"
#define DOCTOR_PHONENUMBER          "DOCTOR.phone_number"
#define DOCTOR_SPECIALIZATION       "DOCTOR.SPECIALIZATION_ID"

#define PATIENT_ID                  "PATIENT.ID"
#define PATIENT_NAME                "PATIENT.first_name"
#define PATIENT_LASTNAME            "PATIENT.last_name"
#define PATIENT_PATRONYMIC          "PATIENT.patronymic"
#define PATIENT_PHONENUMBER         "PATIENT.phone_number"
#define PATIENT_DATEOFBIRTH         "PATIENT.date_of_birth"
#define PATIENT_GENDER              "PATIENT.gender"

#define SPECIALIZATION_ID           "SPECIALIZATION.ID"
#define SPECIALIZATION_NAME         "SPECIALIZATION.name"

#define DIAGNOSIS_ID                "DIAGNOSIS.ID"
#define DIAGNOSIS_COMPLAINTS        "DIAGNOSIS.complaints"
#define DIAGNOSIS_DESCRIPTION       "DIAGNOSIS.description"
#define DIAGNOSIS_STARTTHERAPY      "DIAGNOSIS.start_of_therapy"
#define DIAGNOSIS_ENDTHERAPY        "DIAGNOSIS.end_of_therapy"
#define DIAGNOSIS_STATUS            "DIAGNOSIS.status"
#define DIAGNOSIS_DOCTORID          "DIAGNOSIS.DOCTOR_ID"
#define DIAGNOSIS_DIACNOSTICANID    "DIAGNOSIS.DIAGNOSTICAN_ID"
#define DIAGNOSIS_PATIENTID         "DIAGNOSIS.PATIENT_ID"

#endif // DECLARATION_H
