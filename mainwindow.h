#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTableView>
#include <QTabWidget>
#include <QTextEdit>

#include "structuregl.h"
#include "distributor/distributor.h"
#include "params.h"

class StructureGL;

enum generation_type {
    gen_mdla,
    gen_osm,
    gen_dlca,
    gen_mxdla,
    gen_none
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    
private slots:
    void    newFile();
    void    open();
    void    save();
    void    saveAs();
    void    saveImage();
    void    exportDLA();
    void    importDLA();
    void    about();
    void    getColor();
    void    start();
    void    stop();
    void    stopDistr();
    void    closeWaitDialog();
    void    distrFinished();
    void    propCalc();
    void    distCalc();
    void    propLoad();
    void    setProgress(int value);
    void    settings();
    void    updates();
    void    updateUpdater();
    void    feedback();
    void    feedbackSend();
    void    changeDrawGL();
    void    restructGL();
    void    axesGL();
    void    borderGL();
    void    switchShaders();
    void    switchToLanguage();
    void    changeType(int value);
    void    openGen();
    void    closeGen();
    
protected:
    virtual void closeEvent(QCloseEvent* e);
    
private:
    void    selectLanguage();
    void    saveSettings();
    bool    loadSettings();
    void    loadDefault();
    void    defaultShaders();
    void    updated();
    void    retranslate();
    bool    event(QEvent* event);
    void    updateGenerator();
    static void threadGen(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                          uint32_t hit, uint32_t cluster, double cellsize);
    static void threadRunDistr(double cellSize, double dFrom, double dTo, double dStep);
    void    createActions();
    void    createMenus();
    void    createSettingsMenu();
    void    loadFile(const QString& fileName);
    void    saveFile(const QString& fileName);
    void    createPanel();
    void    createProps();
    void    createTab();
    void    createLayout1();
    void    createLayout2();
    void    clearLayout(QLayout *layout);

    static Distributor* m_distributor;
    static StructureGL* m_glStructure;
    uint32_t            m_panelWidth;
    SetParameters       m_setParams;
    QStringList         m_locales;
    QTranslator         m_translator;
    generation_type     m_currentType;
    QWidget             m_centralWidget;
    QScrollArea         m_glArea;
    QGroupBox*          m_panelBox;
    QGroupBox           m_propsBox;
    QFormLayout*        m_genLayout1;
    Parameters          m_parameter;
    QCheckBox           m_drawGL;
    QCheckBox           m_showAxes;
    QCheckBox           m_showBorders;
    QTabWidget          m_tabProps;
    QString             m_curFile;
    QLineEdit*          m_feedbackProblem;
    QLineEdit*          m_feedbackName;
    QTextEdit*          m_feedbackDescription;
    QComboBox*          m_structureType;
    QDoubleSpinBox*     m_poreDLA;
    QSpinBox*           m_initDLA;
    QSpinBox*           m_stepDLA;
    QSpinBox*           m_hitDLA;
    QSpinBox*           m_clusterDLA;
    QSpinBox            m_distFrom;
    QSpinBox            m_distTo;
    QSpinBox            m_distStep;
    QDoubleSpinBox      m_density;
    QDoubleSpinBox*     m_cellSize;
    QProgressBar*       m_progressBar;
    QProgressBar*       m_progressDistrBar;
    QLineEdit           m_currentMethod;
    QLineEdit           m_surfaceArea;
    QLineEdit           m_densityAero;
    QLineEdit           m_porosityAero;
    QLineEdit*          m_sizesEdit;
    QPushButton*        m_generateButton;
    QPushButton         m_colorButton;
    QPushButton*        m_startButton;
    QPushButton         m_cancelButton;
    QPushButton         m_cancelDistrButton;
    QPushButton*        m_stopButton;
    QPushButton         m_loadButton;
    QPushButton         m_propButton;
    QPushButton         m_distButton;
    QPushButton         m_buttonRu;
    QPushButton         m_buttonEn;
    QDialog             m_langDialog;
    QDialog*            m_waitDialog;
    QWidget             m_surfaceAreaTab;
    QWidget             m_distributionTab;
    QLabel              m_currMethodLabel;
    QLabel              m_colorLabel;
    QLabel*             m_statusLabel;
    QLabel*             m_methodLabel;
    QLabel*             m_sizesLabel;
    QLabel*             m_poreLabel;
    QLabel*             m_initLabel;
    QLabel*             m_stepLabel;
    QLabel*             m_hitLabel;
    QLabel*             m_clusterLabel;
    QLabel*             m_cellSizeLabel;
    QLabel              m_densityLabel;
    
    QLabel*             m_sizesLabel2;
    QLabel*             m_cellSizeLabel2;
    QLabel*             m_poreLabel2;
    QLabel*             m_initLabel2;
    QLabel*             m_stepLabel2;
    QLabel*             m_hitLabel2;
    QLabel*             m_clusterLabel2;
    
    QLabel              m_surfaceAreaLabel;
    QLabel              m_densityAeroLabel;
    QLabel              m_porosityAeroLabel;
    
    QLabel              m_distFromLabel;
    QLabel              m_distToLabel;
    QLabel              m_distStepLabel;
    QMenu*              m_fileMenu;
    QMenu*              m_settingsMenu;
    QMenu*              m_languageMenu;
    QMenu*              m_effectsMenu;
    QMenu*              m_helpMenu;
    QAction             m_newAct;
    QAction             m_openAct;
    QAction             m_saveAct;
    QAction             m_saveAsAct;
    QAction             m_saveImageAct;
    QAction             m_exportDLAAct;
    QAction             m_importDLAAct;
    QAction             m_exitAct;
    QAction             m_languageRuAct;
    QAction             m_languageEnAct;
    QAction             m_effectsDisableAct;
    QAction             m_effectsLambertAct;
    QAction             m_effectsWrapAroundAct;
    QAction             m_effectsPhongAct;
    QAction             m_effectsBlinnAct;
    QAction             m_effectsIsotropWardAct;
    QAction             m_effectsOrenNayarAct;
    QAction             m_effectsCookTorranceAct;
    QAction             m_effectsAnisotropAct;
    QAction             m_effectsAnisotropWardAct;
    QAction             m_effectsMinnaertAct;
    QAction             m_effectsAshikhminShirleyAct;
    QAction             m_effectsCartoonAct;
    QAction             m_effectsGoochAct;
    QAction             m_effectsRimAct;
    QAction             m_effectsSubsurfaceAct;
    QAction             m_effectsBidirectionalAct;
    QAction             m_effectsHemisphericAct;
    QAction             m_effectsTrilightAct;
    QAction             m_effectsLommelSeeligerAct;
    QAction             m_effectsStraussAct;
    QAction             m_settingsAct;
    QAction             m_updateAct;
    QAction             m_aboutAct;
    QAction             m_aboutQtAct;
    QAction             m_feedbackAct;

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator =(const MainWindow&) = delete;
};

#endif // MAINWINDOW_H
