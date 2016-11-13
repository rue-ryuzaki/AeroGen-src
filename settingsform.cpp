#include "settingsform.h"
#include <QTabWidget>
#include <QFormLayout>
#include <QColorDialog>

SettingsForm::SettingsForm(const QString& title, SetParameters& params, ShaderParameters& shaders)
{
    this->setFixedSize(300, 350);
    this->setWindowTitle(title);
    QGridLayout * gridLayout = new QGridLayout;
    
    QTabWidget * tabWidget = new QTabWidget;
    // 1st tab
    QWidget * tab1 = new QWidget;
    QGridLayout *layout1 = new QGridLayout;
    tab1->setLayout(layout1);
    //tabWidget->addTab(tab1, tr("Settings"));
    // 2st tab - shaders
    shadersTab = new QWidget;
    // base
    specPower = new QDoubleSpinBox;
    specPower->setValue(shaders.specPower);
    specColorButton = new QPushButton(tr("Select"));
    for (int i = 0; i < 4; ++i) {
        specColor[i] = shaders.specColor[i];
    }
    specColorButton->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg((int)(specColor[0] * 255)).arg((int)(specColor[1] * 255))
            .arg((int)(specColor[2] * 255)));
    connect(specColorButton, SIGNAL(clicked()), this, SLOT(getColor()));
    // shaders
    comboShader = new QComboBox;
    comboShader->addItem(tr("Select shader"));
    //comboShader->addItem(tr("Lambert"));
    comboShader->addItem(tr("Wrap-around"));
    //comboShader->addItem(tr("Phong"));
    //comboShader->addItem(tr("Blinn"));
    comboShader->addItem(tr("Isotropic Ward"));
    comboShader->addItem(tr("Oren-Nayar"));
    //comboShader->addItem(tr("Cook-Torrance"));---
    //comboShader->addItem(tr("Anisotropic"));---
    //comboShader->addItem(tr("Anisotropic Ward"));---
    comboShader->addItem(tr("Minnaert"));
    //comboShader->addItem(tr("Ashikhmin-Shirley"));---
    comboShader->addItem(tr("Cartoon"));
    comboShader->addItem(tr("Gooch"));
    comboShader->addItem(tr("Rim"));
    comboShader->addItem(tr("Subsurface"));
    comboShader->addItem(tr("Bidirectional"));
    comboShader->addItem(tr("Hemispheric"));
    comboShader->addItem(tr("Trilight"));
    //comboShader->addItem(tr("Lommel-Seeliger"));
    comboShader->addItem(tr("Strauss"));
    connect(comboShader, SIGNAL(activated(int)), this, SLOT(changeShader(int)));
    // wrap
    wrap_factor = new QDoubleSpinBox;
    wrap_factor->setRange(0.0, 1.0);
    wrap_factor->setSingleStep(0.1);
    wrap_factor->setValue(shaders.wrap_factor);
    // iso-ward
    iso_ward_k = new QDoubleSpinBox;
    iso_ward_k->setValue(shaders.iso_ward_k);
    // oren
    oren_a = new QDoubleSpinBox;
    oren_a->setRange(0.0, 1.0);
    oren_a->setSingleStep(0.1);
    oren_a->setValue(shaders.oren_a);
    oren_b = new QDoubleSpinBox;
    oren_b->setRange(0.0, 1.0);
    oren_b->setSingleStep(0.1);
    oren_b->setValue(shaders.oren_b);
    // minnaert
    minnaert_k = new QDoubleSpinBox;
    minnaert_k->setRange(0.0, 1.0);
    minnaert_k->setSingleStep(0.1);
    minnaert_k->setValue(shaders.minnaert_k);
    // cartoon
    cartoon_edgePower = new QDoubleSpinBox;
    cartoon_edgePower->setValue(shaders.cartoon_edgePower);
    // gooch
    gooch_diffuseWarm = new QDoubleSpinBox;
    gooch_diffuseWarm->setRange(0.0, 1.0);
    gooch_diffuseWarm->setSingleStep(0.1);
    gooch_diffuseWarm->setValue(shaders.gooch_diffuseWarm);
    gooch_diffuseCool = new QDoubleSpinBox;
    gooch_diffuseCool->setRange(0.0, 1.0);
    gooch_diffuseCool->setSingleStep(0.1);
    gooch_diffuseCool->setValue(shaders.gooch_diffuseCool);
    // rim
    rim_rimPower = new QDoubleSpinBox;
    rim_rimPower->setValue(shaders.rim_rimPower);
    rim_bias = new QDoubleSpinBox;
    rim_bias->setRange(0.0, 1.0);
    rim_bias->setSingleStep(0.1);
    rim_bias->setValue(shaders.rim_bias);
    // subsurface
    subsurface_matThickness = new QDoubleSpinBox;
    subsurface_matThickness->setRange(0.0, 1.0);
    subsurface_matThickness->setSingleStep(0.1);
    subsurface_matThickness->setValue(shaders.subsurface_matThickness);
    subsurface_rimScalar = new QDoubleSpinBox;
    subsurface_rimScalar->setSingleStep(0.1);
    subsurface_rimScalar->setValue(shaders.subsurface_rimScalar);
    // bidirectional
    bidirectColor2Button = new QPushButton(tr("Select"));
    for (int i = 0; i < 4; ++i) {
        bidirect_color2[i] = shaders.bidirect_color2[i];
    }
    bidirectColor2Button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg((int)(bidirect_color2[0] * 255)).arg((int)(bidirect_color2[1] * 255))
            .arg((int)(bidirect_color2[2] * 255)));
    connect(bidirectColor2Button, SIGNAL(clicked()), this, SLOT(getColor()));
    // hemispheric
    hemisphericColor2Button = new QPushButton(tr("Select"));
    for (int i = 0; i < 4; ++i) {
        hemispheric_color2[i] = shaders.hemispheric_color2[i];
    }
    hemisphericColor2Button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg((int)(hemispheric_color2[0] * 255)).arg((int)(hemispheric_color2[1] * 255))
            .arg((int)(hemispheric_color2[2] * 255)));
    connect(hemisphericColor2Button, SIGNAL(clicked()), this, SLOT(getColor()));
    // trilight
    trilightColor1Button = new QPushButton(tr("Select"));
    for (int i = 0; i < 4; ++i) {
        trilight_color1[i] = shaders.trilight_color1[i];
    }
    trilightColor1Button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg((int)(trilight_color1[0] * 255)).arg((int)(trilight_color1[1] * 255))
            .arg((int)(trilight_color1[2] * 255)));
    connect(trilightColor1Button, SIGNAL(clicked()), this, SLOT(getColor()));
    trilightColor2Button = new QPushButton(tr("Select"));
    for (int i = 0; i < 4; ++i) {
        trilight_color2[i] = shaders.trilight_color2[i];
    }
    trilightColor2Button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg((int)(trilight_color2[0] * 255)).arg((int)(trilight_color2[1] * 255))
            .arg((int)(trilight_color2[2] * 255)));
    connect(trilightColor2Button, SIGNAL(clicked()), this, SLOT(getColor()));
    // strauss
    strauss_smooth = new QDoubleSpinBox;
    strauss_smooth->setRange(0.0, 1.0);
    strauss_smooth->setSingleStep(0.1);
    strauss_smooth->setValue(shaders.strauss_smooth);
    strauss_metal = new QDoubleSpinBox;
    strauss_metal->setRange(0.0, 1.0);
    strauss_metal->setSingleStep(0.1);
    strauss_metal->setValue(shaders.strauss_metal);
    strauss_transp = new QDoubleSpinBox;
    strauss_transp->setRange(0.0, 1.0);
    strauss_transp->setSingleStep(0.1);
    strauss_transp->setValue(shaders.strauss_transp);
    QFormLayout *layout2 = new QFormLayout;
    // base
    layout2->addRow(new QLabel(tr("Spec power:")), specPower);
    layout2->addRow(new QLabel(tr("Spec color:")), specColorButton);
    // shaders
    layout2->addRow(new QLabel(tr("Shader:")), comboShader);
    shadersTab->setLayout(layout2);
    tabWidget->addTab(shadersTab, tr("Shaders"));
    gridLayout->addWidget(tabWidget, 0, 0);
    
    QWidget * btnWidget = new QWidget;
    btnWidget->setFixedHeight(60);
    QPushButton * yesButton = new QPushButton(tr("OK"));
    connect(yesButton, SIGNAL(clicked()), this, SLOT(accept()));
    QPushButton * noButton = new QPushButton(tr("Cancel"));
    connect(noButton, SIGNAL(clicked()), this, SLOT(decline()));
    QGridLayout * btnLayout = new QGridLayout;
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
        shaders.specPower      = form.specPower->value();
        for (int i = 0; i < 4; ++i) {
            shaders.specColor[i] = form.specColor[i];
        }
        // wrap
        shaders.wrap_factor    = form.wrap_factor->value();
        // iso-ward
        shaders.iso_ward_k     = form.iso_ward_k->value();
        // oren
        shaders.oren_a         = form.oren_a->value();
        shaders.oren_b         = form.oren_b->value();
        // minnaert
        shaders.minnaert_k     = form.minnaert_k->value();   
        // cartoon
        shaders.cartoon_edgePower = form.cartoon_edgePower->value();
        // gooch
        shaders.gooch_diffuseWarm = form.gooch_diffuseWarm->value();
        shaders.gooch_diffuseCool = form.gooch_diffuseCool->value();
        // rim
        shaders.rim_rimPower   = form.rim_rimPower->value();
        shaders.rim_bias       = form.rim_bias->value();
        // subsurface
        shaders.subsurface_matThickness = form.subsurface_matThickness->value();
        shaders.subsurface_rimScalar    = form.subsurface_rimScalar->value();
        for (int i = 0; i < 4; ++i) {
            // bidirectional
            shaders.bidirect_color2[i] = form.bidirect_color2[i];
            // hemispheric
            shaders.hemispheric_color2[i] = form.hemispheric_color2[i];
            // trilight
            shaders.trilight_color1[i] = form.trilight_color1[i];
            shaders.trilight_color2[i] = form.trilight_color2[i];
        }
        // strauss
        shaders.strauss_smooth = form.strauss_smooth->value();
        shaders.strauss_metal  = form.strauss_metal->value();
        shaders.strauss_transp = form.strauss_transp->value();
    }
    
    return ret;
}

void SettingsForm::getColor()
{
    QColor colorGL;
    QPushButton * button = dynamic_cast<QPushButton *>(QObject::sender());
    float *colors = nullptr;
    
    if (QObject::sender() == specColorButton) {
        colors = specColor;
    }
    if (QObject::sender() == bidirectColor2Button) {
        colors = bidirect_color2;
    }
    if (QObject::sender() == hemisphericColor2Button) {
        colors = hemispheric_color2;
    }
    if (QObject::sender() == trilightColor1Button) {
        colors = trilight_color1;
    }
    if (QObject::sender() == trilightColor2Button) {
        colors = trilight_color2;
    }
    if (colors == nullptr) {
        return;
    }
    
    colorGL.setRgbF(colors[0], colors[1], colors[2], colors[3]);
    QColor color = QColorDialog::getColor(colorGL, this);
    
    if (color.isValid()) {
        colors[0] = color.redF();
        colors[1] = color.greenF();
        colors[2] = color.blueF();
        colors[3] = color.alphaF();

        button->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg((int)(colors[0] * 255)).arg((int)(colors[1] * 255))
            .arg((int)(colors[2] * 255)));
    }
}

void SettingsForm::changeShader(int value)
{
    clearLayout(shadersTab->layout());
    QFormLayout *layout = new QFormLayout;
    // base
    specPower->show();
    specColorButton->show();
    layout->addRow(new QLabel(tr("Spec power:")), specPower);
    layout->addRow(new QLabel(tr("Spec color:")), specColorButton);
    // shaders
    comboShader->show();
    layout->addRow(new QLabel(tr("Shader:")), comboShader);
    switch (value) {
        case 0: // select
            break;
        case 1: // wrap
            wrap_factor->show();
            layout->addRow(new QLabel(tr("factor:")), wrap_factor);
            break;
        case 2: // iso-ward
            iso_ward_k->show();
            layout->addRow(new QLabel(tr("K:")), iso_ward_k);
            break;
        case 3: // oren
            oren_a->show();
            oren_b->show();
            layout->addRow(new QLabel(tr("A:")), oren_a);
            layout->addRow(new QLabel(tr("B:")), oren_b);
            break;
        case 4: // minnaert
            minnaert_k->show();
            layout->addRow(new QLabel(tr("K:")), minnaert_k);
            break;
        case 5: // cartoon
            cartoon_edgePower->show();
            layout->addRow(new QLabel(tr("edgePower:")), cartoon_edgePower);
            break;
        case 6: // gooch
            gooch_diffuseWarm->show();
            gooch_diffuseCool->show();
            layout->addRow(new QLabel(tr("diffuseWarm:")), gooch_diffuseWarm);
            layout->addRow(new QLabel(tr("diffuseCool:")), gooch_diffuseCool);
            break;
        case 7: // rim
            rim_rimPower->show();
            rim_bias->show();
            layout->addRow(new QLabel(tr("rimPower:")), rim_rimPower);
            layout->addRow(new QLabel(tr("bias:")), rim_bias);
            break;
        case 8: // subsurface
            subsurface_matThickness->show();
            subsurface_rimScalar->show();
            layout->addRow(new QLabel(tr("matThickness:")), subsurface_matThickness);
            layout->addRow(new QLabel(tr("rimScalar:")), subsurface_rimScalar);
            break;
        case 9: // bidirectional
            bidirectColor2Button->show();
            layout->addRow(new QLabel(tr("Color:")), bidirectColor2Button);
            break;
        case 10: // hemispheric
            hemisphericColor2Button->show();
            layout->addRow(new QLabel(tr("Color:")), hemisphericColor2Button);
            break;
        case 11: // trilight
            trilightColor1Button->show();
            trilightColor2Button->show();
            layout->addRow(new QLabel(tr("Color1:")), trilightColor1Button);
            layout->addRow(new QLabel(tr("Color2:")), trilightColor2Button);
            break;
        case 12: // strauss
            strauss_smooth->show();
            strauss_metal->show();
            strauss_transp->show();
            layout->addRow(new QLabel(tr("smooth:")), strauss_smooth);
            layout->addRow(new QLabel(tr("metal:")),  strauss_metal);
            layout->addRow(new QLabel(tr("transp:")), strauss_transp);
            break;
    }
    shadersTab->setLayout(layout);
}

void SettingsForm::accept()
{
    this->done(QDialogButtonBox::Yes);
}

void SettingsForm::decline()
{
    this->done(QDialogButtonBox::No);
}

void SettingsForm::clearLayout(QLayout *layout)
{
    if (layout) {
        while (layout->count() > 0) {
            QLayoutItem *item = layout->takeAt(0);
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
