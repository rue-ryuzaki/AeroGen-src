#ifndef AEROGEN_SETTINGSFORM_H
#define AEROGEN_SETTINGSFORM_H

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

struct ShaderParameters
{
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

class SettingsForm : public QDialog
{
    Q_OBJECT

public:
    SettingsForm(const QString& title, SetParameters&, ShaderParameters&);
    static int dialog(const QString& title, SetParameters&, ShaderParameters&);
    
private slots:
    void    getColor();
    void    changeShader(int);
    void    accept();
    void    decline();
    
private:
    void    clearLayout(QLayout* layout);
    
    QWidget*        m_shadersTab;
    // base
    QDoubleSpinBox* m_specPower;
    QPushButton*    m_specColorButton;
    float           m_specColor[4];
    // shaders
    QComboBox*      m_comboShader;
    // wrap
    QDoubleSpinBox* m_wrapFactor;
    // iso-ward
    QDoubleSpinBox* m_isoWardK;
    // oren
    QDoubleSpinBox* m_orenA;
    QDoubleSpinBox* m_orenB;
    // minnaert
    QDoubleSpinBox* m_minnaertK;
    // cartoon
    QDoubleSpinBox* m_cartoonEdgePower;
    // gooch
    QDoubleSpinBox* m_goochDiffuseWarm;
    QDoubleSpinBox* m_goochDiffuseCool;
    // rim
    QDoubleSpinBox* m_rimRimPower;
    QDoubleSpinBox* m_rimBias;
    // subsurface
    QDoubleSpinBox* m_subsurfaceMatThickness;
    QDoubleSpinBox* m_subsurfaceRimScalar;
    // bidirectional
    QPushButton*    m_bidirectColor2Button;
    float           m_bidirectColor2[4];
    // hemispheric
    QPushButton*    m_hemisphericColor2Button;
    float           m_hemisphericColor2[4];
    // trilight
    QPushButton*    m_trilightColor1Button;
    QPushButton*    m_trilightColor2Button;
    float           m_trilightColor1[4];
    float           m_trilightColor2[4];
    // strauss
    QDoubleSpinBox* m_straussSmooth;
    QDoubleSpinBox* m_straussMetal;
    QDoubleSpinBox* m_straussTransp;

    SettingsForm(const SettingsForm&) = delete;
    SettingsForm& operator =(const SettingsForm&) = delete;
};

#endif // AEROGEN_SETTINGSFORM_H
