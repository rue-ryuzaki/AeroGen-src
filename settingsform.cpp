#include "settingsform.h"

#include <QColorDialog>
#include <QFormLayout>
#include <QTabWidget>

SettingsForm::SettingsForm(const QString& title, SetParameters& /*params*/,
                           ShaderParameters& shaders)
    : m_shadersTab(nullptr),
      m_specPower(nullptr),
      m_specColorButton(nullptr),
      m_specColor(),
      m_comboShader(nullptr),
      m_wrapFactor(nullptr),
      m_isoWardK(nullptr),
      m_orenA(nullptr),
      m_orenB(nullptr),
      m_minnaertK(nullptr),
      m_cartoonEdgePower(nullptr),
      m_goochDiffuseWarm(nullptr),
      m_goochDiffuseCool(nullptr),
      m_rimRimPower(nullptr),
      m_rimBias(nullptr),
      m_subsurfaceMatThickness(nullptr),
      m_subsurfaceRimScalar(nullptr),
      m_bidirectColor2Button(nullptr),
      m_bidirectColor2(),
      m_hemisphericColor2Button(nullptr),
      m_hemisphericColor2(),
      m_trilightColor1Button(nullptr),
      m_trilightColor2Button(nullptr),
      m_trilightColor1(),
      m_trilightColor2(),
      m_straussSmooth(nullptr),
      m_straussMetal(nullptr),
      m_straussTransp(nullptr)
{
    this->setFixedSize(300, 350);
    this->setWindowTitle(title);
    QGridLayout* gridLayout = new QGridLayout;
    
    QTabWidget* tabWidget = new QTabWidget;
    // 1st tab
    QWidget* tab1 = new QWidget;
    QGridLayout* layout1 = new QGridLayout;
    tab1->setLayout(layout1);
    //tabWidget->addTab(tab1, tr("Settings"));
    // 2st tab - shaders
    m_shadersTab = new QWidget;
    // base
    m_specPower = new QDoubleSpinBox;
    m_specPower->setValue(shaders.specPower);
    m_specColorButton = new QPushButton(tr("Select"));
    for (uint32_t i = 0; i < 4; ++i) {
        m_specColor[i] = shaders.specColor[i];
    }
    m_specColorButton->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(m_specColor[0] * 255)).arg(int(m_specColor[1] * 255))
            .arg(int(m_specColor[2] * 255)));
    connect(m_specColorButton, SIGNAL(clicked()), this, SLOT(getColor()));
    // shaders
    m_comboShader = new QComboBox;
    m_comboShader->addItem(tr("Select shader"));
    //comboShader->addItem(tr("Lambert"));
    m_comboShader->addItem(tr("Wrap-around"));
    //comboShader->addItem(tr("Phong"));
    //comboShader->addItem(tr("Blinn"));
    m_comboShader->addItem(tr("Isotropic Ward"));
    m_comboShader->addItem(tr("Oren-Nayar"));
    //comboShader->addItem(tr("Cook-Torrance"));---
    //comboShader->addItem(tr("Anisotropic"));---
    //comboShader->addItem(tr("Anisotropic Ward"));---
    m_comboShader->addItem(tr("Minnaert"));
    //comboShader->addItem(tr("Ashikhmin-Shirley"));---
    m_comboShader->addItem(tr("Cartoon"));
    m_comboShader->addItem(tr("Gooch"));
    m_comboShader->addItem(tr("Rim"));
    m_comboShader->addItem(tr("Subsurface"));
    m_comboShader->addItem(tr("Bidirectional"));
    m_comboShader->addItem(tr("Hemispheric"));
    m_comboShader->addItem(tr("Trilight"));
    //comboShader->addItem(tr("Lommel-Seeliger"));
    m_comboShader->addItem(tr("Strauss"));
    connect(m_comboShader, SIGNAL(activated(int)), this, SLOT(changeShader(int)));
    // wrap
    m_wrapFactor = new QDoubleSpinBox;
    m_wrapFactor->setRange(0.0, 1.0);
    m_wrapFactor->setSingleStep(0.1);
    m_wrapFactor->setValue(shaders.wrap_factor);
    // iso-ward
    m_isoWardK = new QDoubleSpinBox;
    m_isoWardK->setValue(shaders.iso_ward_k);
    // oren
    m_orenA = new QDoubleSpinBox;
    m_orenA->setRange(0.0, 1.0);
    m_orenA->setSingleStep(0.1);
    m_orenA->setValue(shaders.oren_a);
    m_orenB = new QDoubleSpinBox;
    m_orenB->setRange(0.0, 1.0);
    m_orenB->setSingleStep(0.1);
    m_orenB->setValue(shaders.oren_b);
    // minnaert
    m_minnaertK = new QDoubleSpinBox;
    m_minnaertK->setRange(0.0, 1.0);
    m_minnaertK->setSingleStep(0.1);
    m_minnaertK->setValue(shaders.minnaert_k);
    // cartoon
    m_cartoonEdgePower = new QDoubleSpinBox;
    m_cartoonEdgePower->setValue(shaders.cartoon_edgePower);
    // gooch
    m_goochDiffuseWarm = new QDoubleSpinBox;
    m_goochDiffuseWarm->setRange(0.0, 1.0);
    m_goochDiffuseWarm->setSingleStep(0.1);
    m_goochDiffuseWarm->setValue(shaders.gooch_diffuseWarm);
    m_goochDiffuseCool = new QDoubleSpinBox;
    m_goochDiffuseCool->setRange(0.0, 1.0);
    m_goochDiffuseCool->setSingleStep(0.1);
    m_goochDiffuseCool->setValue(shaders.gooch_diffuseCool);
    // rim
    m_rimRimPower = new QDoubleSpinBox;
    m_rimRimPower->setValue(shaders.rim_rimPower);
    m_rimBias = new QDoubleSpinBox;
    m_rimBias->setRange(0.0, 1.0);
    m_rimBias->setSingleStep(0.1);
    m_rimBias->setValue(shaders.rim_bias);
    // subsurface
    m_subsurfaceMatThickness = new QDoubleSpinBox;
    m_subsurfaceMatThickness->setRange(0.0, 1.0);
    m_subsurfaceMatThickness->setSingleStep(0.1);
    m_subsurfaceMatThickness->setValue(shaders.subsurface_matThickness);
    m_subsurfaceRimScalar = new QDoubleSpinBox;
    m_subsurfaceRimScalar->setSingleStep(0.1);
    m_subsurfaceRimScalar->setValue(shaders.subsurface_rimScalar);
    // bidirectional
    m_bidirectColor2Button = new QPushButton(tr("Select"));
    for (uint32_t i = 0; i < 4; ++i) {
        m_bidirectColor2[i] = shaders.bidirect_color2[i];
    }
    m_bidirectColor2Button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(m_bidirectColor2[0] * 255)).arg(int(m_bidirectColor2[1] * 255))
            .arg(int(m_bidirectColor2[2] * 255)));
    connect(m_bidirectColor2Button, SIGNAL(clicked()), this, SLOT(getColor()));
    // hemispheric
    m_hemisphericColor2Button = new QPushButton(tr("Select"));
    for (uint32_t i = 0; i < 4; ++i) {
        m_hemisphericColor2[i] = shaders.hemispheric_color2[i];
    }
    m_hemisphericColor2Button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(m_hemisphericColor2[0] * 255)).arg(int(m_hemisphericColor2[1] * 255))
            .arg(int(m_hemisphericColor2[2] * 255)));
    connect(m_hemisphericColor2Button, SIGNAL(clicked()), this, SLOT(getColor()));
    // trilight
    m_trilightColor1Button = new QPushButton(tr("Select"));
    for (uint32_t i = 0; i < 4; ++i) {
        m_trilightColor1[i] = shaders.trilight_color1[i];
    }
    m_trilightColor1Button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(m_trilightColor1[0] * 255)).arg(int(m_trilightColor1[1] * 255))
            .arg(int(m_trilightColor1[2] * 255)));
    connect(m_trilightColor1Button, SIGNAL(clicked()), this, SLOT(getColor()));
    m_trilightColor2Button = new QPushButton(tr("Select"));
    for (uint32_t i = 0; i < 4; ++i) {
        m_trilightColor2[i] = shaders.trilight_color2[i];
    }
    m_trilightColor2Button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(m_trilightColor2[0] * 255)).arg(int(m_trilightColor2[1] * 255))
            .arg(int(m_trilightColor2[2] * 255)));
    connect(m_trilightColor2Button, SIGNAL(clicked()), this, SLOT(getColor()));
    // strauss
    m_straussSmooth = new QDoubleSpinBox;
    m_straussSmooth->setRange(0.0, 1.0);
    m_straussSmooth->setSingleStep(0.1);
    m_straussSmooth->setValue(shaders.strauss_smooth);
    m_straussMetal = new QDoubleSpinBox;
    m_straussMetal->setRange(0.0, 1.0);
    m_straussMetal->setSingleStep(0.1);
    m_straussMetal->setValue(shaders.strauss_metal);
    m_straussTransp = new QDoubleSpinBox;
    m_straussTransp->setRange(0.0, 1.0);
    m_straussTransp->setSingleStep(0.1);
    m_straussTransp->setValue(shaders.strauss_transp);
    QFormLayout* layout2 = new QFormLayout;
    // base
    layout2->addRow(new QLabel(tr("Spec power:")), m_specPower);
    layout2->addRow(new QLabel(tr("Spec color:")), m_specColorButton);
    // shaders
    layout2->addRow(new QLabel(tr("Shader:")), m_comboShader);
    m_shadersTab->setLayout(layout2);
    tabWidget->addTab(m_shadersTab, tr("Shaders"));
    gridLayout->addWidget(tabWidget, 0, 0);
    
    QWidget* btnWidget = new QWidget;
    btnWidget->setFixedHeight(60);
    QPushButton* yesButton = new QPushButton(tr("OK"));
    connect(yesButton, SIGNAL(clicked()), this, SLOT(accept()));
    QPushButton* noButton = new QPushButton(tr("Cancel"));
    connect(noButton, SIGNAL(clicked()), this, SLOT(decline()));
    QGridLayout* btnLayout = new QGridLayout;
    btnLayout->addWidget(yesButton, 0, 1);
    btnLayout->addWidget(noButton, 0, 2);
    btnWidget->setLayout(btnLayout);
    
    gridLayout->addWidget(btnWidget, 1, 0);
    this->setLayout(gridLayout);
    this->layout()->setMargin(0);
}

int SettingsForm::dialog(const QString& title, SetParameters& params, ShaderParameters& shaders)
{
    SettingsForm form(title, params, shaders);
    int ret = form.exec();
    
    if (ret == QDialogButtonBox::Yes) {
        // save
        // base
        shaders.specPower      = float(form.m_specPower->value());
        for (uint32_t i = 0; i < 4; ++i) {
            shaders.specColor[i] = form.m_specColor[i];
        }
        // wrap
        shaders.wrap_factor    = float(form.m_wrapFactor->value());
        // iso-ward
        shaders.iso_ward_k     = float(form.m_isoWardK->value());
        // oren
        shaders.oren_a         = float(form.m_orenA->value());
        shaders.oren_b         = float(form.m_orenB->value());
        // minnaert
        shaders.minnaert_k     = float(form.m_minnaertK->value());
        // cartoon
        shaders.cartoon_edgePower = float(form.m_cartoonEdgePower->value());
        // gooch
        shaders.gooch_diffuseWarm = float(form.m_goochDiffuseWarm->value());
        shaders.gooch_diffuseCool = float(form.m_goochDiffuseCool->value());
        // rim
        shaders.rim_rimPower   = float(form.m_rimRimPower->value());
        shaders.rim_bias       = float(form.m_rimBias->value());
        // subsurface
        shaders.subsurface_matThickness = float(form.m_subsurfaceMatThickness->value());
        shaders.subsurface_rimScalar    = float(form.m_subsurfaceRimScalar->value());
        for (uint32_t i = 0; i < 4; ++i) {
            // bidirectional
            shaders.bidirect_color2[i] = form.m_bidirectColor2[i];
            // hemispheric
            shaders.hemispheric_color2[i] = form.m_hemisphericColor2[i];
            // trilight
            shaders.trilight_color1[i] = form.m_trilightColor1[i];
            shaders.trilight_color2[i] = form.m_trilightColor2[i];
        }
        // strauss
        shaders.strauss_smooth = float(form.m_straussSmooth->value());
        shaders.strauss_metal  = float(form.m_straussMetal->value());
        shaders.strauss_transp = float(form.m_straussTransp->value());
    }
    
    return ret;
}

void SettingsForm::getColor()
{
    QColor colorGL;
    QPushButton* button = dynamic_cast<QPushButton*>(QObject::sender());
    float* colors = nullptr;
    
    if (QObject::sender() == m_specColorButton) {
        colors = m_specColor;
    }
    if (QObject::sender() == m_bidirectColor2Button) {
        colors = m_bidirectColor2;
    }
    if (QObject::sender() == m_hemisphericColor2Button) {
        colors = m_hemisphericColor2;
    }
    if (QObject::sender() == m_trilightColor1Button) {
        colors = m_trilightColor1;
    }
    if (QObject::sender() == m_trilightColor2Button) {
        colors = m_trilightColor2;
    }
    if (!colors) {
        return;
    }
    
    colorGL.setRgbF(colors[0], colors[1], colors[2], colors[3]);
    QColor color = QColorDialog::getColor(colorGL, this);
    
    if (color.isValid()) {
        colors[0] = float(color.redF());
        colors[1] = float(color.greenF());
        colors[2] = float(color.blueF());
        colors[3] = float(color.alphaF());

        button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(colors[0] * 255)).arg(int(colors[1] * 255))
            .arg(int(colors[2] * 255)));
    }
}

void SettingsForm::changeShader(int value)
{
    clearLayout(m_shadersTab->layout());
    QFormLayout* layout = new QFormLayout;
    // base
    m_specPower->show();
    m_specColorButton->show();
    layout->addRow(new QLabel(tr("Spec power:")), m_specPower);
    layout->addRow(new QLabel(tr("Spec color:")), m_specColorButton);
    // shaders
    m_comboShader->show();
    layout->addRow(new QLabel(tr("Shader:")), m_comboShader);
    switch (value) {
        case 0 : // select
            break;
        case 1 : // wrap
            m_wrapFactor->show();
            layout->addRow(new QLabel(tr("factor:")), m_wrapFactor);
            break;
        case 2 : // iso-ward
            m_isoWardK->show();
            layout->addRow(new QLabel(tr("K:")), m_isoWardK);
            break;
        case 3 : // oren
            m_orenA->show();
            m_orenB->show();
            layout->addRow(new QLabel(tr("A:")), m_orenA);
            layout->addRow(new QLabel(tr("B:")), m_orenB);
            break;
        case 4 : // minnaert
            m_minnaertK->show();
            layout->addRow(new QLabel(tr("K:")), m_minnaertK);
            break;
        case 5 : // cartoon
            m_cartoonEdgePower->show();
            layout->addRow(new QLabel(tr("edgePower:")), m_cartoonEdgePower);
            break;
        case 6 : // gooch
            m_goochDiffuseWarm->show();
            m_goochDiffuseCool->show();
            layout->addRow(new QLabel(tr("diffuseWarm:")), m_goochDiffuseWarm);
            layout->addRow(new QLabel(tr("diffuseCool:")), m_goochDiffuseCool);
            break;
        case 7 : // rim
            m_rimRimPower->show();
            m_rimBias->show();
            layout->addRow(new QLabel(tr("rimPower:")), m_rimRimPower);
            layout->addRow(new QLabel(tr("bias:")), m_rimBias);
            break;
        case 8 : // subsurface
            m_subsurfaceMatThickness->show();
            m_subsurfaceRimScalar->show();
            layout->addRow(new QLabel(tr("matThickness:")), m_subsurfaceMatThickness);
            layout->addRow(new QLabel(tr("rimScalar:")), m_subsurfaceRimScalar);
            break;
        case 9 : // bidirectional
            m_bidirectColor2Button->show();
            layout->addRow(new QLabel(tr("Color:")), m_bidirectColor2Button);
            break;
        case 10 : // hemispheric
            m_hemisphericColor2Button->show();
            layout->addRow(new QLabel(tr("Color:")), m_hemisphericColor2Button);
            break;
        case 11 : // trilight
            m_trilightColor1Button->show();
            m_trilightColor2Button->show();
            layout->addRow(new QLabel(tr("Color1:")), m_trilightColor1Button);
            layout->addRow(new QLabel(tr("Color2:")), m_trilightColor2Button);
            break;
        case 12 : // strauss
            m_straussSmooth->show();
            m_straussMetal->show();
            m_straussTransp->show();
            layout->addRow(new QLabel(tr("smooth:")), m_straussSmooth);
            layout->addRow(new QLabel(tr("metal:")),  m_straussMetal);
            layout->addRow(new QLabel(tr("transp:")), m_straussTransp);
            break;
    }
    m_shadersTab->setLayout(layout);
}

void SettingsForm::accept()
{
    this->done(QDialogButtonBox::Yes);
}

void SettingsForm::decline()
{
    this->done(QDialogButtonBox::No);
}

void SettingsForm::clearLayout(QLayout* layout)
{
    if (layout) {
        while (layout->count() > 0) {
            QLayoutItem* item = layout->takeAt(0);
            if (item->layout()) {
                clearLayout(item->layout());
                delete item->layout();
            }
            if (dynamic_cast<QLabel*>(item->widget())) {
                delete item->widget();
            }
            if (item->widget()) {
                item->widget()->hide();
            }
            delete item;
        }
        delete layout;
    }
}
