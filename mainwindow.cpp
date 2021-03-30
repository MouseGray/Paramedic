#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Paramedic 1.0");

    toMainPage();

    QValidator *nameValidator = new QRegExpValidator(QRegExp(QString::fromLocal8Bit("[А-ЩЭ-Я][а-я]{1,29}")), this);
    QValidator *numberValidator = new QRegExpValidator(QRegExp(QString::fromLocal8Bit("[1-9][0-9]{5,19}")), this);

    ui->regName->setValidator(nameValidator);
    ui->regLastName->setValidator(nameValidator);
    ui->regPatronymic->setValidator(nameValidator);

    ui->regNumber->setValidator(numberValidator);

    ui->editorName->setValidator(nameValidator);
    ui->editorLastName->setValidator(nameValidator);
    ui->editorPatronymic->setValidator(nameValidator);

    ui->editorNumber->setValidator(numberValidator);

    ui->viewTable   ->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->editorTable ->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ConfigSystem configSystem;
    configSystem.Load();

    m_database = new Database();
    m_database->initialize(configSystem.host(), configSystem.database(), configSystem.login(), configSystem.password());
    ui->dataBaseErrorMessage->setVisible(!m_database->open());

    m_specializeDataModel = new QSqlQueryModel(m_database);
    m_viewDataModel       = new QSqlQueryModel(m_database);
    m_editorDataModel     = new QSqlQueryModel(m_database);

    ui->regSpecialization   ->setModel(m_specializeDataModel);
    ui->editorSpecialization->setModel(m_specializeDataModel);
    ui->editorTable         ->setModel(m_editorDataModel);

    ui->viewTable->setModel(m_viewDataModel);

    m_database->fillSpecialzation(m_specializeDataModel);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(hideMessage()));
    hideMessage();


}

MainWindow::~MainWindow()
{
    m_database->close();
    delete ui;
}

void MainWindow::toMainPage()
{
    ui->widgetStack->setCurrentIndex(MAINPAGE);
}

void MainWindow::toCreationPage(PageType type)
{
    m_currentPageType = type;

    bool doctor_patient = type == PageType::DoctorPage ? true : false;

    QDate date(2000, 0, 0);

    ui->regTitle->setText(RUS(type == PageType::DoctorPage ? "Регистрация врача" : "Регистрация пациента"));

    ui->regSpecializationLabel  ->setVisible(doctor_patient);
    ui->regSpecialization       ->setVisible(doctor_patient);

    ui->regGenderLabel      ->setVisible(!doctor_patient);
    ui->regMale             ->setVisible(!doctor_patient);
    ui->regFemale           ->setVisible(!doctor_patient);
    ui->regDateOfBirthLabel ->setVisible(!doctor_patient);
    ui->regDateOfBirth      ->setVisible(!doctor_patient);

    ui->regName        ->clear();
    ui->regLastName    ->clear();
    ui->regPatronymic  ->clear();
    ui->regNumber      ->clear();
    ui->regMale        ->setChecked(true);
    ui->regDateOfBirth ->setDate(date);

    ui->widgetStack->setCurrentIndex(CREATEONPAGE);
}

void MainWindow::toViewListPage(PageType pageType, SelectType selectType)
{
    m_currentPageType   = pageType;
    m_currentSelectType = selectType;

    ui->viewTitle->setText(RUS(pageType   == PageType::DoctorPage ? "Список врачей" : "Список пациентов"));
    ui->viewEdit ->setText(RUS(selectType == SelectType::Edit ? "Редактировать" : "Выбрать"));
    ui->viewEdit->setEnabled(false);

    updateViewTable(pageType, 1);

    ui->widgetStack->setCurrentIndex(VIEWPAGE);
}

void MainWindow::toEditorPage(PageType type)
{
    m_currentPageType = type;

    bool doctor_patient = (type == PageType::DoctorPage);

    ui->editorTitle     ->setText(RUS(type == PageType::DoctorPage ? "Редактирование врача" : "Редактирование пациента"));
    ui->editorExtraTitle->setText(RUS(type == PageType::DoctorPage ? "Пациенты" : "Диагнозы"));

    ui->editorSpecializationLabel  ->setVisible(doctor_patient);
    ui->editorSpecialization       ->setVisible(doctor_patient);

    ui->editorGenderLabel      ->setVisible(!doctor_patient);
    ui->editorMale             ->setVisible(!doctor_patient);
    ui->editorFemale           ->setVisible(!doctor_patient);
    ui->editorDateOfBirthLabel ->setVisible(!doctor_patient);
    ui->editorDateOfBirth      ->setVisible(!doctor_patient);

    ui->editorAdd              ->setVisible(!doctor_patient);
    ui->editorEdit             ->setVisible(!doctor_patient);
    ui->editorEdit             ->setEnabled(false);

    updateEditorTable(type, m_personid, 1);

    switch(type)
    {
    case PageType::DoctorPage:
        m_doctor.setID(m_personid);
        m_doctor.selectAll(m_database->m_db);

        ui->editorName              ->setText       (m_doctor.name()          );
        ui->editorLastName          ->setText       (m_doctor.lastName()      );
        ui->editorPatronymic        ->setText       (m_doctor.patronymic()    );
        ui->editorNumber            ->setText       (m_doctor.number()        );
        ui->editorSpecialization    ->setCurrentText(m_doctor.specialization());
        break;
    case PageType::PatientPage:
        m_patient.setID(m_personid);
        m_patient.selectAll(m_database->m_db);

        ui->editorName              ->setText   (m_patient.name()             );
        ui->editorLastName          ->setText   (m_patient.lastName()         );
        ui->editorPatronymic        ->setText   (m_patient.patronymic()       );
        ui->editorNumber            ->setText   (m_patient.number()           );
        ui->editorMale              ->setChecked(m_patient.gender() == Patient::Male   );
        ui->editorFemale            ->setChecked(m_patient.gender() == Patient::Female );
        ui->editorDateOfBirth       ->setDate   (m_patient.dateOfBirth()      );
        break;
    }
    ui->widgetStack->setCurrentIndex(EDITORPAGE);
}

void MainWindow::toDiagnosisEditorPage()
{
    ui->widgetStack->setCurrentIndex(DIAGNOSISEDITORPAGE);
}

bool MainWindow::fromCreationPage()
{
    bool isEmpty =
            ui->regName->text().isEmpty()       &&
            ui->regLastName->text().isEmpty()   &&
            ui->regPatronymic->text().isEmpty() &&
            ui->regNumber->text().isEmpty();

    if(isEmpty) return true;
    return QMessageBox::StandardButton::Ok == QMessageBox::warning(this,
                                                                   RUS("Предупреждение"),
                                                                   RUS("В плях находятся несохранённые данные.\nПри преходе они будут потеряны."),
                                                                   QMessageBox::StandardButton::Ok,
                                                                   QMessageBox::StandardButton::Cancel);
}

bool MainWindow::fromEditorPage(PageType type)
{
    bool isEmpty = true;
    switch(type){
    case PageType::DoctorPage:
        isEmpty =
            ui->editorName          ->text()        == m_doctor.name()           &&
            ui->editorLastName      ->text()        == m_doctor.lastName()       &&
            ui->editorPatronymic    ->text()        == m_doctor.patronymic()     &&
            ui->editorNumber        ->text()        == m_doctor.number()         &&
            ui->editorSpecialization->currentText() == m_doctor.specialization();
        break;
    case PageType::PatientPage:
        isEmpty =
            ui->editorName          ->text()        == m_patient.name()           &&
            ui->editorLastName      ->text()        == m_patient.lastName()       &&
            ui->editorPatronymic    ->text()        == m_patient.patronymic()     &&
            ui->editorNumber        ->text()        == m_patient.number()         &&
            ui->editorDateOfBirth   ->date()        == m_patient.dateOfBirth();
        break;
    }
    if(isEmpty) return true;
    return QMessageBox::StandardButton::Ok == QMessageBox::warning(this,
                                                                   RUS("Предупреждение"),
                                                                   RUS("В плях находятся несохранённые данные.\nПри преходе они будут потеряны."),
                                                                   QMessageBox::StandardButton::Ok,
                                                                   QMessageBox::StandardButton::Cancel);
}

bool MainWindow::fromDiagnosisEditorPage()
{
    bool isEmpty =
            ui->diagnosisEditorComplaints->toPlainText() == m_diagnosis.complaints    &&
            ui->diagnosisEditorDiagnosis ->toPlainText() == m_diagnosis.description   &&
            ui->diagnosisEditorDiagnostician->text()     == m_diagnosis.diagnostican.fullName() &&
            ui->diagnosisEditorDoctor       ->text()     == m_diagnosis.doctor.fullName();

    if(isEmpty) return true;
    return QMessageBox::StandardButton::Ok == QMessageBox::warning(this,
                                                                   RUS("Предупреждение"),
                                                                   RUS("В плях находятся несохранённые данные.\nПри преходе они будут потеряны."),
                                                                   QMessageBox::StandardButton::Ok,
                                                                   QMessageBox::StandardButton::Cancel);
}

void MainWindow::updateSwitchPageButton(QToolButton &previous, QToolButton &next, QSpinBox &number)
{
    previous.setEnabled(number.value() != number.minimum());
    next    .setEnabled(number.value() != number.maximum());
}

void MainWindow::updateSwitchPageButtonView() {
    updateSwitchPageButton(*ui->viewPrevious, *ui->viewNext, *ui->viewNumber);
}

void MainWindow::updateSwitchPageButtonEditor() {
    updateSwitchPageButton(*ui->editorPrevious, *ui->editorNext, *ui->editorExtraNumber);
}

void MainWindow::updateViewTable(PageType pageType, int page)
{
    m_database->fillView(pageType, ui->viewSearch->text(), page, m_viewDataModel);
    ui->viewTable->setColumnHidden(0, true);

    ui->viewNumber->setValue(page);
    ui->viewNumber->setMaximum(m_database->getPageCount(pageType, ui->viewSearch->text()) + 1);

    updateSwitchPageButtonView();
}

void MainWindow::updateEditorTable(PageType pageType, int id, int page)
{
    m_database->fillEditor(pageType, ui->editorSearch->text(), id, page, m_editorDataModel);
    ui->editorTable->setColumnHidden(0, true);

    ui->editorExtraNumber->setValue(page);
    ui->editorExtraNumber->setMaximum(m_database->getEditorPageCount(pageType, id, "") + 1);

    updateSwitchPageButtonEditor();
}

void MainWindow::updateDiagnosisStatus(DiagnosisStatus status)
{
    switch(status){
    case DiagnosisStatus::No:
        ui->diagnosisEditorComplaints           ->setEnabled(true);
        ui->diagnosisEditorDiagnosis            ->setEnabled(false);
        ui->diagnosisEditorDiagnostician        ->setEnabled(true);
        ui->diagnosisEditorDiagnosticianSelect  ->setEnabled(true);
        ui->diagnosisEditorDoctor               ->setEnabled(false);
        ui->diagnosisEditorDoctorSelect         ->setEnabled(false);

        ui->diagnosisEditorStatus       ->setText(RUS("-"));
        ui->diagnosisEditorNext         ->setText(RUS("Назначить диагностику"));

        ui->diagnosisEditorNext         ->setVisible(true);
        ui->diagnosisEditorClose        ->setVisible(false);
        ui->diagnosisEditorErrorClose   ->setVisible(false);
        break;
    case DiagnosisStatus::Diagnostics:
        ui->diagnosisEditorComplaints           ->setEnabled(false);
        ui->diagnosisEditorDiagnosis            ->setEnabled(true);
        ui->diagnosisEditorDiagnostician        ->setEnabled(false);
        ui->diagnosisEditorDiagnosticianSelect  ->setEnabled(false);
        ui->diagnosisEditorDoctor               ->setEnabled(true);
        ui->diagnosisEditorDoctorSelect         ->setEnabled(true);

        ui->diagnosisEditorStatus       ->setText(RUS("Дигностика"));
        ui->diagnosisEditorNext         ->setText(RUS("Назначить лечение"));

        ui->diagnosisEditorNext         ->setVisible(true);
        ui->diagnosisEditorClose        ->setVisible(true);
        ui->diagnosisEditorErrorClose   ->setVisible(false);
        break;
    case DiagnosisStatus::Treatment:
        ui->diagnosisEditorComplaints           ->setEnabled(false);
        ui->diagnosisEditorDiagnosis            ->setEnabled(false);
        ui->diagnosisEditorDiagnostician        ->setEnabled(false);
        ui->diagnosisEditorDiagnosticianSelect  ->setEnabled(false);
        ui->diagnosisEditorDoctor               ->setEnabled(false);
        ui->diagnosisEditorDoctorSelect         ->setEnabled(false);

        ui->diagnosisEditorStatus       ->setText(RUS("Лечение"));
        ui->diagnosisEditorNext         ->setText(RUS("Завершить лечение"));

        ui->diagnosisEditorNext         ->setVisible(true);
        ui->diagnosisEditorClose        ->setVisible(true);
        ui->diagnosisEditorErrorClose   ->setVisible(true);
        break;
    case DiagnosisStatus::Completed:
        ui->diagnosisEditorComplaints           ->setEnabled(false);
        ui->diagnosisEditorDiagnosis            ->setEnabled(false);
        ui->diagnosisEditorDiagnostician        ->setEnabled(false);
        ui->diagnosisEditorDiagnosticianSelect  ->setEnabled(false);
        ui->diagnosisEditorDoctor               ->setEnabled(false);
        ui->diagnosisEditorDoctorSelect         ->setEnabled(false);

        ui->diagnosisEditorStatus       ->setText(RUS("Лечение завершено"));

        ui->diagnosisEditorNext         ->setVisible(false);
        ui->diagnosisEditorClose        ->setVisible(false);
        ui->diagnosisEditorErrorClose   ->setVisible(false);
        break;
    case DiagnosisStatus::Closed:
        ui->diagnosisEditorComplaints           ->setEnabled(false);
        ui->diagnosisEditorDiagnosis            ->setEnabled(false);
        ui->diagnosisEditorDiagnostician        ->setEnabled(false);
        ui->diagnosisEditorDiagnosticianSelect  ->setEnabled(false);
        ui->diagnosisEditorDoctor               ->setEnabled(false);
        ui->diagnosisEditorDoctorSelect         ->setEnabled(false);

        ui->diagnosisEditorStatus->setText(RUS("Диагноз закрыт"));

        ui->diagnosisEditorNext         ->setVisible(false);
        ui->diagnosisEditorClose        ->setVisible(false);
        ui->diagnosisEditorErrorClose   ->setVisible(false);
        break;
    case DiagnosisStatus::ErrorClosed:
        ui->diagnosisEditorComplaints           ->setEnabled(false);
        ui->diagnosisEditorDiagnosis            ->setEnabled(false);
        ui->diagnosisEditorDiagnostician        ->setEnabled(false);
        ui->diagnosisEditorDiagnosticianSelect  ->setEnabled(false);
        ui->diagnosisEditorDoctor               ->setEnabled(false);
        ui->diagnosisEditorDoctorSelect         ->setEnabled(false);

        ui->diagnosisEditorStatus->setText(RUS("Диагноз ошибочный"));

        ui->diagnosisEditorNext         ->setVisible(false);
        ui->diagnosisEditorClose        ->setVisible(false);
        ui->diagnosisEditorErrorClose   ->setVisible(false);
        break;
    }
}

void MainWindow::structToEditor(DiagnosisStruct &diagnosis)
{
    ui->diagnosisEditorPatient      ->setText(diagnosis.patient.fullName());

    ui->diagnosisEditorComplaints   ->setText(diagnosis.complaints);
    ui->diagnosisEditorDiagnosis    ->setText(diagnosis.description);
    ui->diagnosisEditorDiagnostician->setText(diagnosis.diagnostican.fullName());
    ui->diagnosisEditorDoctor       ->setText(diagnosis.doctor.fullName());

    ui->diagnosisEditorStartDate    ->setText(diagnosis.startTherapy.toString("dd-MM-yy"));
    ui->diagnosisEditorEndDate      ->setText(diagnosis.endTherapy == QDate(0, 0, 0) ? "-" : diagnosis.endTherapy.toString("dd-MM-yy"));

    updateDiagnosisStatus(diagnosis.status);
}

void MainWindow::editorToStruct(DiagnosisStruct &diagnosis, DiagnosisStatus status)
{
    diagnosis.complaints   = ui->diagnosisEditorComplaints->toPlainText();
    diagnosis.description  = ui->diagnosisEditorDiagnosis->toPlainText();
    diagnosis.status       = status;
}

void MainWindow::clearDiagnosisEditor()
{
    m_diagnosis.complaints   .clear();
    m_diagnosis.description  .clear();
    m_diagnosis.diagnostican .clear();
    m_diagnosis.doctor       .clear();
    m_diagnosis.patient      = m_patient;
    m_diagnosis.status       = DiagnosisStatus::No;

    m_diagnosis.startTherapy = QDate::currentDate();
    m_diagnosis.endTherapy   = QDate(0, 0, 0);
}

bool MainWindow::checkData(PageType pageType, const QString &name, const QString &lastName, const QString &patronymic, const QString &number)
{
    if(name.size() < 2){
        QMessageBox::warning(this, RUS("Ошибка"), RUS("Некорректные данные в поле 'Имя'. Должно быть не менее 2 симвлов."));
        return false;
    }
    if(lastName.size() < 2){
        QMessageBox::warning(this, RUS("Ошибка"), RUS("Некорректные данные в поле 'Фамилия'. Должно быть не менее 2 симвлов."));
        return false;
    }
    if(patronymic.size() == 1){
        QMessageBox::warning(this, RUS("Ошибка"), RUS("Некорректные данные в поле 'Отчество'. Отчество не может состоять из 1 симвла."));
        return false;
    }
    if(number.size() < 6 && pageType == PageType::DoctorPage){
        QMessageBox::warning(this, RUS("Ошибка"), RUS("Некорректные данные в поле 'Контактный телефон'. Должно быть не менее 6 цифр."));
        return false;
    }
    return true;
}

void MainWindow::showMessage(const QString &text)
{
    ui->message->setText(text);
    ui->message->setVisible(true);
    m_timer.start(3000);
}

void MainWindow::hideMessage()
{
    ui->message->setVisible(false);
}

void MainWindow::on_createDoctor_clicked()
{
    toCreationPage(PageType::DoctorPage);
}

void MainWindow::on_createPatient_clicked()
{
    toCreationPage(PageType::PatientPage);
}

void MainWindow::on_showDoctors_clicked()
{
    toViewListPage(PageType::DoctorPage, SelectType::Edit);
}

void MainWindow::on_showPatients_clicked()
{
    toViewListPage(PageType::PatientPage, SelectType::Edit);
}

void MainWindow::on_regApply_clicked()
{
    if(!checkData(m_currentPageType,
                  ui->regName           ->text(),
                  ui->regLastName       ->text(),
                  ui->regPatronymic     ->text(),
                  ui->regNumber         ->text())) return;

    switch(m_currentPageType){
    case PageType::DoctorPage:
        if(!m_database->addDoctor(ui->regName           ->text(),
                                  ui->regLastName       ->text(),
                                  ui->regPatronymic     ->text(),
                                  ui->regNumber         ->text(),
                                  ui->regSpecialization ->currentText())){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
            return;
        }
        showMessage(RUS("Врач успешно зарегистирован."));
        break;
    case PageType::PatientPage:
        if(!m_database->addPatient(ui->regName           ->text(),
                                   ui->regLastName       ->text(),
                                   ui->regPatronymic     ->text(),
                                   ui->regNumber         ->text(),
                                  !ui->regMale           ->isChecked(),
                                   ui->regDateOfBirth    ->text())){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
            return;
        }
        showMessage(RUS("Пациент успешно зарегистирован."));
        break;
    }

    toMainPage();
}

void MainWindow::on_regBack_clicked()
{
    if(!fromCreationPage()) return;
    toMainPage();
}

void MainWindow::on_viewEdit_clicked()
{
    switch(m_currentSelectType){
    case SelectType::Edit:
        m_personid = m_viewDataModel->data(m_viewDataModel->index(ui->viewTable->currentIndex().row(), 0)).toInt();
        toEditorPage(m_currentPageType);
        break;
    case SelectType::Select:
        if(m_diagnosis.status == DiagnosisStatus::No) {
            m_diagnosis.diagnostican.setID(m_viewDataModel->data(m_viewDataModel->index(ui->viewTable->currentIndex().row(), 0)).toInt());
            m_diagnosis.diagnostican.selectNSP(m_database->m_db);
            ui->diagnosisEditorDiagnostician->setText(m_diagnosis.diagnostican.fullName());
        }
        else {
            m_diagnosis.doctor.setID(m_viewDataModel->data(m_viewDataModel->index(ui->viewTable->currentIndex().row(), 0)).toInt());
            m_diagnosis.doctor.selectNSP(m_database->m_db);
            ui->diagnosisEditorDoctor->setText(m_diagnosis.doctor.fullName());
        }
        toDiagnosisEditorPage();
        break;
    }
}

void MainWindow::on_viewBack_clicked()
{
    m_currentSelectType == SelectType::Edit ? toMainPage() : toDiagnosisEditorPage();
}

void MainWindow::on_editorBack_clicked()
{
    if(!fromEditorPage(m_currentPageType)) return;
    toViewListPage(m_currentPageType, SelectType::Edit);
}

void MainWindow::on_diagnosisEditorBack_clicked()
{
    if(!fromDiagnosisEditorPage()) return;
    toEditorPage(PageType::PatientPage);
}

void MainWindow::on_editorEdit_clicked()
{
    m_diagnosis.id = m_editorDataModel->data(m_editorDataModel->index(ui->editorTable->currentIndex().row(), 0)).toInt();
    m_database->getDiagnosisStruct(&m_diagnosis);
    structToEditor(m_diagnosis);
    toDiagnosisEditorPage();
}

void MainWindow::on_editorAdd_clicked()
{
    clearDiagnosisEditor();
    structToEditor(m_diagnosis);
    toDiagnosisEditorPage();
}

void MainWindow::on_diagnosisEditorDiagnosticianSelect_clicked()
{
    toViewListPage(PageType::DoctorPage, SelectType::Select);
}

void MainWindow::on_diagnosisEditorDoctorSelect_clicked()
{
    toViewListPage(PageType::DoctorPage, SelectType::Select);
}

void MainWindow::on_viewNumber_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    updateSwitchPageButtonView();
}

void MainWindow::on_editorExtraNumber_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    updateSwitchPageButtonEditor();
}

void MainWindow::on_viewNext_clicked() {
    updateViewTable(m_currentPageType, ui->viewNumber->value() + 1);
}

void MainWindow::on_viewPrevious_clicked() {
    updateViewTable(m_currentPageType, ui->viewNumber->value() - 1);
}

void MainWindow::on_editorPrevious_clicked() {
    updateEditorTable(m_currentPageType, m_patient.ID(), ui->editorExtraNumber->value() - 1);
}

void MainWindow::on_editorNext_clicked() {
    updateEditorTable(m_currentPageType, m_patient.ID(), ui->editorExtraNumber->value() + 1);
}

void MainWindow::on_viewSearch_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    updateViewTable(m_currentPageType, 1);
}

void MainWindow::on_editorSearch_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    updateEditorTable(m_currentPageType, m_personid, 1);
}

void MainWindow::on_viewTable_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    ui->viewEdit->setEnabled(true);
}

void MainWindow::on_editorSave_clicked()
{
    if(!checkData(m_currentPageType,
                  ui->editorName           ->text(),
                  ui->editorLastName       ->text(),
                  ui->editorPatronymic     ->text(),
                  ui->editorNumber         ->text())) return;

    switch(m_currentPageType){
    case PageType::DoctorPage:
        m_doctor.name          () = ui->editorName           ->text();
        m_doctor.lastName      () = ui->editorLastName       ->text();
        m_doctor.patronymic    () = ui->editorPatronymic     ->text();
        m_doctor.number        () = ui->editorNumber         ->text();
        m_doctor.specialization() = ui->editorSpecialization ->currentText();

        if(!m_database->saveDoctor(m_doctor)){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
            return;
        }
        showMessage(RUS("Изменения успешно сохранены."));
        break;
    case PageType::PatientPage:
        m_patient.setName          (ui->editorName           ->text()      );
        m_patient.setLastName      (ui->editorLastName       ->text()      );
        m_patient.setPatronymic    (ui->editorPatronymic     ->text()      );
        m_patient.setNumber        (ui->editorNumber         ->text()      );
        m_patient.setGender        (Patient::Gender(!ui->editorMale   ->isChecked()));
        m_patient.setDateOfBirth   (ui->editorDateOfBirth    ->date()      );

        if(!m_database->savePatient(m_patient)){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
            return;
        }
        showMessage(RUS("Изменения успешно сохранены."));
        break;
    }
}

void MainWindow::on_diagnosisEditorNext_clicked()
{
    switch(m_diagnosis.status){
    case DiagnosisStatus::No:
        if(ui->diagnosisEditorComplaints->toPlainText().isEmpty()){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Поле 'Жалобы' не может быть пустым."));
            return;
        }
        if(ui->diagnosisEditorDiagnostician->text().isEmpty()){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Поле 'Диагнозист' не может быть пустым."));
            return;
        }
        editorToStruct(m_diagnosis, DiagnosisStatus::Diagnostics);
        if(!m_database->createDiagnosis(m_diagnosis)){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
            m_diagnosis.status = DiagnosisStatus::No;
            return;
        }
        break;
    case DiagnosisStatus::Diagnostics:
        if(ui->diagnosisEditorDiagnosis->toPlainText().isEmpty()){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Поле 'Диагноз' не может быть пустым."));
            return;
        }
        if(ui->diagnosisEditorDoctor->text().isEmpty()){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Поле 'Лечащий врач' не может быть пустым."));
            return;
        }
        editorToStruct(m_diagnosis, DiagnosisStatus::Treatment);
        if(!m_database->setDiagnosis(m_diagnosis)){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
            m_diagnosis.status = DiagnosisStatus::Diagnostics;
            return;
        }
        break;
    case DiagnosisStatus::Treatment:
        m_diagnosis.endTherapy = QDate::currentDate();
        editorToStruct(m_diagnosis, DiagnosisStatus::Completed);
        if(!m_database->releaseDiagnosis(m_diagnosis)){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
            m_diagnosis.status = DiagnosisStatus::Treatment;
            return;
        }
        ui->diagnosisEditorEndDate->setText(m_diagnosis.endTherapy.toString("dd-MM-yy"));
        break;
    default:
        return;
    }
    updateDiagnosisStatus(m_diagnosis.status);
}

void MainWindow::on_diagnosisEditorClose_clicked()
{
    DiagnosisStatus reserv = m_diagnosis.status;
    m_diagnosis.status = DiagnosisStatus::Closed;
    m_diagnosis.endTherapy = QDate::currentDate();
    if(!m_database->releaseDiagnosis(m_diagnosis)){
        QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
        m_diagnosis.status = reserv;
        return;
    }
    ui->diagnosisEditorEndDate->setText(m_diagnosis.endTherapy.toString("dd-MM-yy"));
    updateDiagnosisStatus(DiagnosisStatus::Closed);
}

void MainWindow::on_diagnosisEditorErrorClose_clicked()
{
    DiagnosisStatus reserv = m_diagnosis.status;
    m_diagnosis.status = DiagnosisStatus::ErrorClosed;
    m_diagnosis.endTherapy = QDate::currentDate();
    if(!m_database->releaseDiagnosis(m_diagnosis)){
        QMessageBox::warning(this, RUS("Ошибка"), RUS("Не удалось сохранить данные."));
        m_diagnosis.status = reserv;
        return;
    }
    ui->diagnosisEditorEndDate->setText(m_diagnosis.endTherapy.toString("dd-MM-yy"));
    updateDiagnosisStatus(DiagnosisStatus::ErrorClosed);
}

void MainWindow::on_editorTable_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    ui->editorEdit->setEnabled(true);
}



void MainWindow::on_viewTable_doubleClicked(const QModelIndex &index)
{
    switch(m_currentSelectType){
    case SelectType::Edit:
        m_personid = m_viewDataModel->data(m_viewDataModel->index(ui->viewTable->currentIndex().row(), 0)).toInt();
        toEditorPage(m_currentPageType);
        break;
    case SelectType::Select:
        if(m_diagnosis.status == DiagnosisStatus::No) {
            m_diagnosis.diagnostican.setID(m_viewDataModel->data(m_viewDataModel->index(ui->viewTable->currentIndex().row(), 0)).toInt());
            m_diagnosis.diagnostican.selectNSP(m_database->m_db);
            ui->diagnosisEditorDiagnostician->setText(m_diagnosis.diagnostican.fullName());
        }
        else {
            m_diagnosis.doctor.setID(m_viewDataModel->data(m_viewDataModel->index(ui->viewTable->currentIndex().row(), 0)).toInt());
            m_diagnosis.doctor.selectNSP(m_database->m_db);
            ui->diagnosisEditorDoctor->setText(m_diagnosis.doctor.fullName());
        }
        toDiagnosisEditorPage();
        break;
    }
}

void MainWindow::on_editorTable_doubleClicked(const QModelIndex &index)
{
    m_diagnosis.id = m_editorDataModel->data(m_editorDataModel->index(ui->editorTable->currentIndex().row(), 0)).toInt();
    m_database->getDiagnosisStruct(&m_diagnosis);
    structToEditor(m_diagnosis);
    toDiagnosisEditorPage();
}
