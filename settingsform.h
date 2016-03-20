#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>

struct ShaderParameters {
    // base
    float   specPower;
    float   specColor[4];
    // shaders
    // wrap
    float   wrap_factor;
    // iso-ward
    float   iso_ward_k;
    // oren
    float   oren_a;
    float   oren_b;
    // minnaert
    float   minnaert_k;
    // cartoon
    float   cartoon_edgePower;
    // gooch
    float   gooch_diffuseWarm;
    float   gooch_diffuseCool;
    // rim
    float   rim_rimPower;
    float   rim_bias;
    // subsurface
    float   subsurface_matThickness;
    float   subsurface_rimScalar;
    // bidirectional
    float   bidirect_color2[4];
    // hemispheric
    float   hemispheric_color2[4];
    // trilight
    float   trilight_color1[4];
    float   trilight_color2[4];
    // strauss
    float   strauss_smooth;
    float   strauss_metal;
    float   strauss_transp;
};

struct SetParameters {
};

class SettingsForm : public QDialog {
    Q_OBJECT
public:
    SettingsForm(const QString& title, SetParameters&, ShaderParameters&);
    static int dialog(const QString& title, SetParameters&, ShaderParameters&);
    
private slots:
    void getColor();
    void changeShader(int);
    void accept();
    void decline();
    
private:
    void clearLayout(QLayout *layout);
    
    QWidget * shadersTab;
    // base
    QDoubleSpinBox * specPower;
    QPushButton * specColorButton;
    float specColor[4];
    // shaders
    QComboBox * comboShader;
    // wrap
    QDoubleSpinBox * wrap_factor;
    // iso-ward
    QDoubleSpinBox * iso_ward_k;
    // oren
    QDoubleSpinBox * oren_a;
    QDoubleSpinBox * oren_b;
    // minnaert
    QDoubleSpinBox * minnaert_k;
    // cartoon
    QDoubleSpinBox * cartoon_edgePower;
    // gooch
    QDoubleSpinBox * gooch_diffuseWarm;
    QDoubleSpinBox * gooch_diffuseCool;
    // rim
    QDoubleSpinBox * rim_rimPower;
    QDoubleSpinBox * rim_bias;
    // subsurface
    QDoubleSpinBox * subsurface_matThickness;
    QDoubleSpinBox * subsurface_rimScalar;
    // bidirectional
    QPushButton * bidirectColor2Button;
    float bidirect_color2[4];
    // hemispheric
    QPushButton * hemisphericColor2Button;
    float hemispheric_color2[4];
    // trilight
    QPushButton * trilightColor1Button;
    QPushButton * trilightColor2Button;
    float trilight_color1[4];
    float trilight_color2[4];
    // strauss
    QDoubleSpinBox * strauss_smooth;
    QDoubleSpinBox * strauss_metal;
    QDoubleSpinBox * strauss_transp;
};

#endif /* SETTINGSFORM_H */
