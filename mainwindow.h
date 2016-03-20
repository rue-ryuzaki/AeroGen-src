#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QTabWidget>
#include "structuregl.h"
#include "distributor/distributor.h"
#include "params.h"
#include "iniparser.h"
#include "settingsform.h"

enum generation_type {
    gen_mdla,
    gen_osm,
    gen_dlca,
    gen_none
};

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QProgressBar;
class QComboBox;
class QSpinBox;
class QLineEdit;
class StructureGL;
class QGroupBox;
class QPushButton;
class QDoubleSpinBox;
class QTabWidget;
class QCheckBox;
class QTextEdit;
class QTableView;
class QRadioButton;
class QButtonGroup;
class QFormLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    
private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void saveImage();
    void exportDLA();
    void importDLA();
    void about();
    void getColor();
    void start();
    void stop();
    void stopDistr();
    void closeWaitDialog();
    void distrFinished();
    void propCalc();
    void distCalc();
    void propLoad();
    void setProgress(int value);
    void settings();
    void updates();
    void updateUpdater();
    void feedback();
    void feedbackSend();
    void changeDrawGL();
    void restructGL();
    void axesGL();
    void borderGL();
    void switchShaders();
    void switchToLanguage();
    void switchToLanguageB();
    void changeType(int value);
    void openGen();
    void closeGen();
    
protected:
    virtual void closeEvent(QCloseEvent *e);
    
private:
    void selectLanguage();
    void saveSettings();
    bool loadSettings();
    void loadDefault();
    void defaultShaders();
    void updated();
    void retranslate();
    bool event(QEvent *event);
    void updateGenerator();
    static void threadGen(const Sizes & sizes, double por, int initial, int step, int hit, size_t cluster, double cellsize);
    static void threadRunDistr(double cellSize, double dFrom, double dTo, double dStep);
    void createActions();
    void createMenus();
    void createSettingsMenu();
    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void createPanel();
    void createProps();
    void createTab();
    void createLayout1();
    void createLayout2();
    void clearLayout(QLayout *layout);
    
    int panelWidth;
    SetParameters setParams;
    vector<distrib> distr;
    QStringList locales;
    QTranslator * appTranslator;
    static Distributor * distributor;
    generation_type current_type = gen_none;
    string settingsFile = "settings.ini";
    QWidget *centralWidget;
    QScrollArea *glArea;
    QGroupBox *panelBox;
    QGroupBox *propsBox;
    Parameters parameter;
    static StructureGL *glStructure;
    QFormLayout * genLayout1;
    QFormLayout * genLayout2;
    QCheckBox * drawGL;
    QCheckBox * showAxes;
    QCheckBox * showBorders;
    QTabWidget * tabProps;
    QString curFile;
    QLineEdit *feedbackProblem;
    QLineEdit *feedbackName;
    QTextEdit *feedbackDescription;
    QComboBox *structureType;
    QDoubleSpinBox *poreDLA;
    QSpinBox *initDLA;
    QSpinBox *stepDLA;
    QSpinBox *hitDLA;
    QSpinBox *clusterDLA;
    QSpinBox *distFrom;
    QSpinBox *distTo;
    QSpinBox *distStep;
    QDoubleSpinBox *density;
    QDoubleSpinBox *cellSize;
    QProgressBar *progressBar;
    QProgressBar *progressDistrBar;
    QLineEdit *currentMethod;
    QLineEdit *surfaceArea;
    QLineEdit *densityAero;
    QLineEdit *porosityAero;
    QLineEdit *sizesEdit;
    QPushButton *generateButton;
    QPushButton *colorButton;
    QPushButton *startButton;
    QPushButton *cancelButton;
    QPushButton *cancelDistrButton;
    QPushButton *stopButton;
    QPushButton *loadButton;
    QPushButton *propButton;
    QPushButton *distButton;
    QPushButton *buttonRu;
    QPushButton *buttonEn;
    QDialog * langDialog;
    QDialog * waitDialog;
    QWidget * surfaceAreaTab;
    QWidget * distributionTab;
    QLabel *currMethodLabel;
    QLabel *colorLabel;
    QLabel *statusLabel;
    QLabel *methodLabel;
    QLabel *sizesLabel;
    QLabel *poreLabel;
    QLabel *initLabel;
    QLabel *stepLabel;
    QLabel *hitLabel;
    QLabel *clusterLabel;
    QLabel *cellSizeLabel;
    QLabel *densityLabel;
    
    QLabel *sizesLabel2;
    QLabel *cellSizeLabel2;
    QLabel *poreLabel2;
    QLabel *initLabel2;
    QLabel *stepLabel2;
    QLabel *hitLabel2;
    QLabel *clusterLabel2;
    
    QLabel *surfaceAreaLabel;
    QLabel *densityAeroLabel;
    QLabel *porosityAeroLabel;
    
    QLabel *distFromLabel;
    QLabel *distToLabel;
    QLabel *distStepLabel;
    QMenu *fileMenu;
    QMenu *settingsMenu;
    QMenu *languageMenu;
    QMenu *effectsMenu;
    QMenu *helpMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *saveImageAct;
    QAction *exportDLAAct;
    QAction *importDLAAct;
    QAction *exitAct;
    QAction *languageRuAct;
    QAction *languageEnAct;
    QAction *effectsDisableAct;
    QAction *effectsLambertAct;
    QAction *effectsWrapAroundAct;
    QAction *effectsPhongAct;
    QAction *effectsBlinnAct;
    QAction *effectsIsotropWardAct;
    QAction *effectsOrenNayarAct;
    QAction *effectsCookTorranceAct;
    QAction *effectsAnisotropAct;
    QAction *effectsAnisotropWardAct;
    QAction *effectsMinnaertAct;
    QAction *effectsAshikhminShirleyAct;
    QAction *effectsCartoonAct;
    QAction *effectsGoochAct;
    QAction *effectsRimAct;
    QAction *effectsSubsurfaceAct;
    QAction *effectsBidirectionalAct;
    QAction *effectsHemisphericAct;
    QAction *effectsTrilightAct;
    QAction *effectsLommelSeeligerAct;
    QAction *effectsStraussAct;
    QAction *settingsAct;
    QAction *updateAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *feedbackAct;
};

#endif MAINWINDOW_H
