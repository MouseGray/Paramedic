#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QToolButton>
#include <QSpinBox>
#include <QTimer>

#include "database.h"
#include "configsystem.h"

#define MAINPAGE            0
#define CREATEONPAGE        1
#define VIEWPAGE            2
#define EDITORPAGE          3
#define DIAGNOSISEDITORPAGE 4

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void toMainPage();
    void toCreationPage (PageType type);
    void toViewListPage (PageType pageType, SelectType selectType);
    void toEditorPage   (PageType type);
    void toDiagnosisEditorPage();

    bool fromCreationPage();
    bool fromEditorPage(PageType type);
    bool fromDiagnosisEditorPage();

    void updateSwitchPageButton(QToolButton& previous, QToolButton& next, QSpinBox& number);
    void updateSwitchPageButtonView();
    void updateSwitchPageButtonEditor();

    void updateViewTable(PageType pageType, int page);
    void updateEditorTable(PageType pageType, int id, int page);
    void updateDiagnosisStatus(DiagnosisStatus status);

    void structToEditor(DiagnosisStruct& diagnosis);
    void editorToStruct(DiagnosisStruct& diagnosis, DiagnosisStatus status);
    void clearDiagnosisEditor();

    bool checkData(PageType pageType, const QString& name, const QString& lastName, const QString& patronymic, const QString& number);

    void showMessage(const QString& text);
private slots:
    void hideMessage();

    void on_createDoctor_clicked();

    void on_createPatient_clicked();

    void on_showDoctors_clicked();

    void on_showPatients_clicked();

    void on_regApply_clicked();

    void on_regBack_clicked();

    void on_viewEdit_clicked();

    void on_viewBack_clicked();

    void on_editorBack_clicked();

    void on_diagnosisEditorBack_clicked();

    void on_editorEdit_clicked();

    void on_editorAdd_clicked();

    void on_diagnosisEditorDiagnosticianSelect_clicked();

    void on_diagnosisEditorDoctorSelect_clicked();

    void on_viewNumber_valueChanged(int arg1);

    void on_viewNext_clicked();

    void on_viewPrevious_clicked();

    void on_viewSearch_textChanged(const QString &arg1);

    void on_viewTable_clicked(const QModelIndex &index);

    void on_editorSave_clicked();

    void on_diagnosisEditorNext_clicked();

    void on_diagnosisEditorClose_clicked();

    void on_diagnosisEditorErrorClose_clicked();

    void on_editorTable_clicked(const QModelIndex &index);

    void on_editorPrevious_clicked();

    void on_editorNext_clicked();

    void on_editorExtraNumber_valueChanged(int arg1);

    void on_editorSearch_textChanged(const QString &arg1);

    void on_viewTable_doubleClicked(const QModelIndex &index);

    void on_editorTable_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    int m_personid = 0;

    Doctor    m_doctor;
    Patient   m_patient;
    DiagnosisStruct m_diagnosis;

    PageType m_currentPageType = PageType::DoctorPage;
    SelectType m_currentSelectType = SelectType::Edit;

    Database* m_database = nullptr;

    QSqlQueryModel* m_specializeDataModel = nullptr;
    QSqlQueryModel* m_viewDataModel = nullptr;
    QSqlQueryModel* m_editorDataModel = nullptr;

    QTimer m_timer;
};
#endif // MAINWINDOW_H
