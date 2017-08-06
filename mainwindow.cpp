#include "mainwindow.h"

#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "baseformats.h"
#include "checker.h"
#include "functions.h"

StructureGL* MainWindow::glStructure;
Distributor* MainWindow::distributor;

MainWindow::MainWindow()
{
    appTranslator = new QTranslator(this);
    qApp->installTranslator(appTranslator);
    setWindowIcon(QIcon(":/icon.png"));
    buttonRu = new QPushButton;
    buttonEn = new QPushButton;
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    // create GL widget
    glStructure = new StructureGL;
    distributor = new Distributor(this);
    glArea = new QScrollArea;
    glArea->setWidget(glStructure);
    glArea->setWidgetResizable(true);
    glArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    glArea->setMinimumSize(250, 250);
    
    //create settings Panel
#ifdef _WIN32
// windows
    panelWidth = 260;
#else
    panelWidth = 300;
#endif
    createPanel();
    createProps();
    createTab();
    createActions();
    createMenus();

    QGridLayout* centralLayout = new QGridLayout;
    {
        int hmax = 45;
        QWidget* widget1 = new QWidget;
        QFormLayout* layout1 = new QFormLayout;
        currentMethod = new QLineEdit;
        currentMethod->setToolTip(QString::fromStdString("MultiDLA\n\nOSM\n\nDLCA\n\n") + tr("Undefined"));
        updateGenerator();
        currentMethod->setFixedWidth(100);
        widget1->setMaximumHeight(hmax);
        currentMethod->setReadOnly(true);
        currMethodLabel = new QLabel;
        layout1->addRow(currMethodLabel, currentMethod);
        widget1->setLayout(layout1);
        centralLayout->addWidget(widget1, 0, 0);
        
        QWidget* widget2 = new QWidget;
        //widget2->setMaximumHeight(hmax);
        QFormLayout* layout2 = new QFormLayout;
        drawGL = new QCheckBox;
        drawGL->setChecked(true);
        connect(drawGL, SIGNAL(clicked()), this, SLOT(changeDrawGL()));
        layout2->addRow(drawGL);
        
        showAxes = new QCheckBox;
        showAxes->setChecked(false);
        showAxes->setEnabled(drawGL->isChecked());
        connect(showAxes, SIGNAL(clicked()), this, SLOT(axesGL()));
        layout2->addRow(showAxes);
        
        showBorders = new QCheckBox;
        showBorders->setChecked(false);
        showBorders->setEnabled(drawGL->isChecked());
        connect(showBorders, SIGNAL(clicked()), this, SLOT(borderGL()));
        layout2->addRow(showBorders);
        
        widget2->setLayout(layout2);
        centralLayout->addWidget(widget2, 0, 1);
        
        QWidget* widget3 = new QWidget;
        QFormLayout* layout3 = new QFormLayout;
        colorButton = new QPushButton;
        colorButton->setAutoFillBackground(true);
        widget3->setMaximumHeight(hmax);
        connect(colorButton, SIGNAL(clicked()), this, SLOT(getColor()));
        colorLabel = new QLabel;
        layout3->addRow(colorLabel, colorButton);
        widget3->setLayout(layout3);
        centralLayout->addWidget(widget3, 0, 2);

        progressDistrBar = new QProgressBar;
        progressDistrBar->setMinimum(0);
        progressDistrBar->setMaximum(0);
        progressDistrBar->setValue(0);
        cancelDistrButton = new QPushButton;
        connect(cancelDistrButton, SIGNAL(clicked()), this, SLOT(stopDistr()));
        waitDialog = new QDialog(this, Qt::Window | Qt::WindowTitleHint |
                Qt::WindowSystemMenuHint);
        waitDialog->setFixedSize(200, 85);
        QFormLayout* waitlayout = new QFormLayout;
        waitlayout->addRow(progressDistrBar);
        waitlayout->addRow(cancelDistrButton);
        waitDialog->setLayout(waitlayout);
    }
    centralLayout->addWidget(glArea, 1, 0, 4, 3);
    centralLayout->addWidget(panelBox, 0, 3, 2, 1);
    centralLayout->addWidget(propsBox, 2, 3);
    centralLayout->addWidget(tabProps, 3, 3);
    QWidget* widgetE = new QWidget;
    centralLayout->addWidget(widgetE, 4, 3);
    centralWidget->setLayout(centralLayout);

    statusBar();
    setWindowTitle(tr("AeroGen"));
    colorButton->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
        .arg(int(glStructure->colors[0] * 255)).arg(int(glStructure->colors[1] * 255))
        .arg(int(glStructure->colors[2] * 255)));
    if (!loadSettings()) {
        loadDefault();
    }
    // check for updates!
    try {
        if (md5UpdaterHash() != updaterFileHash()) {
            if (DownloadUpdater()) {
                sleep(1);
            } else {
                std::cout << "Hhmm... updater old, new version not found ..." << std::endl;
            }
        }
        if (checkUpdate()) {
            updated();
        }
    } catch (...) {
        statusBar()->showMessage(tr("Update error!"), 5000);
    }
    retranslate();
#ifdef _WIN32
// windows
    resize(800, 550);
#else
    resize(840, 600);
#endif
}

void MainWindow::selectLanguage()
{
    langDialog = new QDialog(this);
    langDialog->setFixedSize(170, 200);
    
    QFormLayout* layout = new QFormLayout;
    layout->addRow(new QLabel(tr("Select language:")));
    int w = 150;
    int h = 80;

    buttonRu->setFlat(true);
    buttonRu->setStyleSheet("border-image: url(:/ru.png) 0 0 0 0 stretch stretch;");
    buttonRu->setFixedSize(w, h);
    buttonRu->setAutoFillBackground(true);
    connect(buttonRu, SIGNAL(clicked()), this, SLOT(switchToLanguageB()));
    layout->addRow(buttonRu);

    buttonEn->setFlat(true);
    buttonEn->setStyleSheet("border-image: url(:/en.png) 0 0 0 0 stretch stretch;");
    buttonEn->setFixedSize(w, h);
    buttonEn->setAutoFillBackground(true);
    connect(buttonEn, SIGNAL(clicked()), this, SLOT(switchToLanguageB()));
    layout->addRow(buttonEn);

    langDialog->setLayout(layout);
    langDialog->exec();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    QMessageBox::StandardButton resBtn =
            QMessageBox::question(this, tr("Exit"), tr("Are you sure?\n"),
                                  QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        saveSettings();
        e->accept();
    } else {
        e->ignore();
    }
}

void MainWindow::saveSettings()
{
    int language = -1;
    int shaders = glStructure->shadersStatus();
    if (languageRuAct->isChecked()) {
        language = 0;
    }
    if (languageEnAct->isChecked()) {
        language = 1;
    }
    if (language == -1) {
        std::cerr << "Error language" << std::endl;
        return;
    }
    std::ofstream out;
    out.open(settingsFile.c_str(), std::ios_base::trunc);
    out << "lang    = " << language << "\n";
    out << "color   = " << glStructure->colors[0] << ";" << glStructure->colors[1] << ";" << glStructure->colors[2] << "\n";
    out << "shaders = " << shaders << "\n";
    
    out.close();
}

bool MainWindow::loadSettings()
{
    if (!fileExists(settingsFile.c_str())) {
        return false;
    }
    
    IniParser parser(settingsFile.c_str());
    int language = -1;
    std::string slang = parser.property("lang");
    try {
        language = stoi(slang);
    } catch (...) {
        std::cerr << "Error parsing lang" << std::endl;
        return false;
    }
    
    int shaders = -1;
    std::string sshad = parser.property("shaders");
    try {
        shaders = stoi(sshad);
    } catch (...) {
        std::cerr << "Error parsing shaders" << std::endl;
        return false;
    }
    std::string scolors = parser.property("color");
    std::vector<std::string> vcolors = split(scolors, ';');
    if (vcolors.size() != 3) {
        std::cout << "Parse error. Expect 3 color values" << std::endl;
        return false;
    }
    GLfloat colors[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    try {
        for (int i = 0; i < 3; ++i) {
            colors[i] = QString::fromStdString(vcolors[i]).toFloat();// stof(vcolors[i], &sz);
        }
    } catch (...) {
        std::cerr << "Error parsing colors" << std::endl;
        return false;
    }
    // alpha not used
    for (int i = 0; i < 3; ++i) {
        if (colors[i] < 0 || colors[i] > 1) {
            std::cerr << "Error parsing colors. Expect values between 0 & 1." << std::endl;;
            return false;
        }
        glStructure->colors[i] = colors[i];
    }
    colorButton->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(glStructure->colors[0] * 255)).arg(int(glStructure->colors[1] * 255))
            .arg(int(glStructure->colors[2] * 255)));
    defaultShaders();
    glStructure->needInit = shaders;
    if (shaders != 0 && !glStructure->supportShaders()) {
        if (glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shaders = 0;
        }
    }
    glStructure->enableShader(shaders);
    //shaders = glStructure->ShadersStatus();
    effectsDisableAct->setChecked(shaders == 0);
    effectsLambertAct->setChecked(shaders == 1);
    effectsWrapAroundAct->setChecked(shaders == 2);
    effectsPhongAct->setChecked(shaders == 3);
    effectsBlinnAct->setChecked(shaders == 4);
    effectsIsotropWardAct->setChecked(shaders == 5);
    effectsOrenNayarAct->setChecked(shaders == 6);
    effectsCookTorranceAct->setChecked(shaders == 7);
    effectsAnisotropAct->setChecked(shaders == 8);
    effectsAnisotropWardAct->setChecked(shaders == 9);
    effectsMinnaertAct->setChecked(shaders == 10);
    effectsAshikhminShirleyAct->setChecked(shaders == 11);
    effectsCartoonAct->setChecked(shaders == 12);
    effectsGoochAct->setChecked(shaders == 13);
    effectsRimAct->setChecked(shaders == 14);
    effectsSubsurfaceAct->setChecked(shaders == 15);
    effectsBidirectionalAct->setChecked(shaders == 16);
    effectsHemisphericAct->setChecked(shaders == 17);
    effectsTrilightAct->setChecked(shaders == 18);
    effectsLommelSeeligerAct->setChecked(shaders == 19);
    effectsStraussAct->setChecked(shaders == 20);
    if (shaders < 0 || shaders > 20 || shaders == 7 || shaders == 8 ||
            shaders == 9 || shaders == 11) {
        std::cout << "Error shaders value" << std::endl;
        return false;
    }
    restructGL();

    if (language == 0 || language == 1) {
        appTranslator->load(locales[language]); 
        languageRuAct->setChecked(language == 0);
        languageEnAct->setChecked(language == 1);
        return true;
    }
    return false;
}

void MainWindow::loadDefault()
{
    defaultShaders();
    // structure color
    glStructure->colors[0] = 0.0f;
    glStructure->colors[1] = 1.0f;
    glStructure->colors[2] = 0.0f;
    glStructure->colors[3] = 1.0f;
    colorButton->setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(glStructure->colors[0] * 255)).arg(int(glStructure->colors[1] * 255))
            .arg(int(glStructure->colors[2] * 255)));
    // shaders
    int shaders = 0; // default disabled
    glStructure->needInit = shaders;
    if (shaders != 0 && !glStructure->supportShaders()) {
        if (glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shaders = 0;
        }
    }
    glStructure->enableShader(shaders);
    //shaders = glStructure->ShadersStatus();
    effectsDisableAct->setChecked(shaders == 0);
    effectsLambertAct->setChecked(shaders == 1);
    effectsWrapAroundAct->setChecked(shaders == 2);
    effectsPhongAct->setChecked(shaders == 3);
    effectsBlinnAct->setChecked(shaders == 4);
    effectsIsotropWardAct->setChecked(shaders == 5);
    effectsOrenNayarAct->setChecked(shaders == 6);
    effectsCookTorranceAct->setChecked(shaders == 7);
    effectsAnisotropAct->setChecked(shaders == 8);
    effectsAnisotropWardAct->setChecked(shaders == 9);
    effectsMinnaertAct->setChecked(shaders == 10);
    effectsAshikhminShirleyAct->setChecked(shaders == 11);
    effectsCartoonAct->setChecked(shaders == 12);
    effectsGoochAct->setChecked(shaders == 13);
    effectsRimAct->setChecked(shaders == 14);
    effectsSubsurfaceAct->setChecked(shaders == 15);
    effectsBidirectionalAct->setChecked(shaders == 16);
    effectsHemisphericAct->setChecked(shaders == 17);
    effectsTrilightAct->setChecked(shaders == 18);
    effectsLommelSeeligerAct->setChecked(shaders == 19);
    effectsStraussAct->setChecked(shaders == 20);
    
    restructGL();
    // language
    selectLanguage();
}

void MainWindow::defaultShaders()
{
    // base
    glStructure->params.specPower      = 30.0f;
    glStructure->params.specColor[0]   = 0.7f;
    glStructure->params.specColor[1]   = 0.7f;
    glStructure->params.specColor[2]   = 0.0f;
    glStructure->params.specColor[3]   = 1.0f;
    // shaders
    // wrap
    glStructure->params.wrap_factor    = 0.5f;
    // iso-ward
    glStructure->params.iso_ward_k     = 10.0f;
    // oren
    glStructure->params.oren_a         = 1.0f;
    glStructure->params.oren_b         = 0.45f;
    // minnaert
    glStructure->params.minnaert_k     = 0.8f;
    // cartoon
    glStructure->params.cartoon_edgePower = 3.0f;
    // gooch
    glStructure->params.gooch_diffuseWarm = 0.45f;
    glStructure->params.gooch_diffuseCool = 0.45f;
    // rim
    glStructure->params.rim_rimPower   = 8.0f;
    glStructure->params.rim_bias       = 0.3f;
    // subsurface
    glStructure->params.subsurface_matThickness = 0.56f;
    glStructure->params.subsurface_rimScalar    = 1.38f;
    // bidirectional
    glStructure->params.bidirect_color2[0] = 0.5f;
    glStructure->params.bidirect_color2[1] = 0.5f;
    glStructure->params.bidirect_color2[2] = 0.5f;
    glStructure->params.bidirect_color2[3] = 1.0f;
    // hemispheric
    glStructure->params.hemispheric_color2[0] = 0.5f;
    glStructure->params.hemispheric_color2[1] = 0.5f;
    glStructure->params.hemispheric_color2[2] = 0.5f;
    glStructure->params.hemispheric_color2[3] = 1.0f;
    // trilight
    glStructure->params.trilight_color1[0] = 0.25f;
    glStructure->params.trilight_color1[1] = 0.25f;
    glStructure->params.trilight_color1[2] = 0.25f;
    glStructure->params.trilight_color1[3] = 1.0f;
    glStructure->params.trilight_color2[0] = 0.75f;
    glStructure->params.trilight_color2[1] = 0.75f;
    glStructure->params.trilight_color2[2] = 0.75f;
    glStructure->params.trilight_color2[3] = 1.0f;
    // strauss
    glStructure->params.strauss_metal  = 0.7f;
    glStructure->params.strauss_smooth = 0.7f;
    glStructure->params.strauss_transp = 0.1f;
}

void MainWindow::clearLayout(QLayout* layout)
{
    if (layout) {
        while (layout->count() > 0) {
            QLayoutItem* item = layout->takeAt(0);
            if (item->layout()) {
                clearLayout(item->layout());
                delete item->layout();
            }
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }
}

void MainWindow::openGen()
{
    clearLayout(genLayout1);
    createLayout2();
    retranslate();
    genLayout1->addRow(methodLabel, structureType);
    genLayout1->addRow(sizesLabel, sizesEdit);
    genLayout1->addRow(cellSizeLabel, cellSize);
    genLayout1->addRow(poreLabel, poreDLA);
    genLayout1->addRow(initLabel, initDLA);
    genLayout1->addRow(stepLabel, stepDLA);
    genLayout1->addRow(hitLabel, hitDLA);
    genLayout1->addRow(clusterLabel, clusterDLA);
    genLayout1->addRow(startButton);
    //panelBox->setLayout(genLayout1);
}

void MainWindow::closeGen()
{
    //generateDialog->close();
}

void MainWindow::createLayout1()
{
     // layout 1
    structureType = new QComboBox;
    QStringList typeList;
    //
    // pH < 7 - MultiDLA Organic
    // pH > 7 - Spheres Inorganic
    // Cluster DLCA ?
    //
    typeList.push_back(tr("(MultiDLA)"));
    typeList.push_back(tr("(OSM)"));
    typeList.push_back(tr("(DLCA)"));
    structureType->addItems(typeList);
    structureType->setCurrentIndex(parameter.method);
    
    connect(structureType, SIGNAL(activated(int)), this, SLOT(changeType(int)));
    methodLabel = new QLabel;
    
    sizesEdit = new QLineEdit(QString::fromStdString(parameter.sizes));
    sizesLabel = new QLabel;
    
    cellSize = new QDoubleSpinBox;
    cellSize->setMinimum(0.3);
    cellSize->setMaximum(50);
    cellSize->setValue(parameter.cellSize);
    cellSizeLabel = new QLabel;
    
    poreDLA = new QDoubleSpinBox;
    poreDLA->setMinimum(0);
    poreDLA->setMaximum(100);
    poreDLA->setValue(parameter.porosity);
    poreLabel = new QLabel;
    
    initDLA = new QSpinBox;
    initDLA->setMinimum(1);
    initDLA->setMaximum(1000);
    initDLA->setValue(parameter.init);
    initLabel = new QLabel;
    
    stepDLA = new QSpinBox;
    stepDLA->setMinimum(1);
    stepDLA->setValue(parameter.step);
    stepLabel = new QLabel;
    
    hitDLA = new QSpinBox;
    hitDLA->setMinimum(1);
    hitDLA->setValue(parameter.hit);
    hitLabel = new QLabel;
    
    clusterDLA = new QSpinBox;
    clusterDLA->setMinimum(1);
    clusterDLA->setValue(parameter.cluster);
    clusterLabel = new QLabel;
    
    startButton = new QPushButton;
    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    
    bool enabled = (parameter.method == 0);
    initDLA->setEnabled(enabled);
    stepDLA->setEnabled(enabled);
    hitDLA->setEnabled(enabled);
    initLabel->setEnabled(enabled);
    stepLabel->setEnabled(enabled);
    hitLabel->setEnabled(enabled);
}

void MainWindow::createLayout2()
{
    // layout 2
    sizesLabel2    = new QLabel;
    cellSizeLabel2 = new QLabel;
    poreLabel2     = new QLabel;
    initLabel2     = new QLabel;
    stepLabel2     = new QLabel;
    hitLabel2      = new QLabel;
    clusterLabel2  = new QLabel;
    
    progressBar = new QProgressBar;
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setValue(0);
    
    statusLabel = new QLabel;
    
    generateButton = new QPushButton;
    connect(generateButton, SIGNAL(clicked()), this, SLOT(openGen()));
    
    stopButton = new QPushButton;
    stopButton->setAutoFillBackground(true);
    stopButton->setStyleSheet("* { background-color: rgb(255, 0, 0); }");
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
}

void MainWindow::createPanel()
{
    panelBox = new QGroupBox;
#ifdef _WIN32
// windows
    panelBox->setFixedSize(panelWidth, 270);
#elif defined __linux__
//linux
    panelBox->setFixedSize(panelWidth, 320);
#elif defined __APPLE__
// apple
    panelBox->setFixedSize(panelWidth, 320);
#endif
    // default parameters
    parameter.method   = 0;
    parameter.sizes    = "50x50x50";
    parameter.porosity = 95.0;
    parameter.init     = 10;
    parameter.step     = 1;
    parameter.hit      = 1;
    parameter.cluster  = 1;
    parameter.cellSize = 2.0;
    //
    createLayout1();
    createLayout2();
    genLayout1 = new QFormLayout;
    genLayout1->addRow(methodLabel, structureType);
    genLayout1->addRow(sizesLabel, sizesEdit);
    genLayout1->addRow(cellSizeLabel, cellSize);
    genLayout1->addRow(poreLabel, poreDLA);
    genLayout1->addRow(initLabel, initDLA);
    genLayout1->addRow(stepLabel, stepDLA);
    genLayout1->addRow(hitLabel, hitDLA);
    genLayout1->addRow(clusterLabel, clusterDLA);
    genLayout1->addRow(startButton);
    
    panelBox->setLayout(genLayout1);
}

void MainWindow::createProps()
{
    propsBox = new QGroupBox;
    propsBox->setFixedSize(panelWidth, 65);
    
    QFormLayout* layout = new QFormLayout;
    
    //loadButton = new QPushButton(""));
    //connect(loadButton, SIGNAL(clicked()), this, SLOT(propLoad()));
    //layout->addRow(loadButton);
    
    density = new QDoubleSpinBox;
    density->setMinimum(1);
    density->setMaximum(100000);
    density->setValue(2200);
    densityLabel = new QLabel;
    layout->addRow(densityLabel, density);

    propsBox->setLayout(layout);
}

void MainWindow::createTab()
{
    tabProps = new QTabWidget;
#ifdef _WIN32
// windows
    tabProps->setFixedSize(panelWidth, 160);
#elif defined __linux__
//linux
    tabProps->setFixedSize(panelWidth, 180);
#elif defined __APPLE__
// apple
    tabProps->setFixedSize(panelWidth, 180);
#endif
    // Specific surface & Aerogel density
    {
        QFormLayout* layout = new QFormLayout;

        surfaceArea = new QLineEdit;
        surfaceArea->setReadOnly(true);
        surfaceAreaLabel = new QLabel;
        surfaceAreaLabel->setStyleSheet("* sup{margin-left: -0.3em;}");
        layout->addRow(surfaceAreaLabel, surfaceArea);

        densityAero = new QLineEdit;
        densityAero->setReadOnly(true);
        densityAeroLabel = new QLabel;
        layout->addRow(densityAeroLabel, densityAero);
        
        porosityAero = new QLineEdit;
        porosityAero->setReadOnly(true);
        porosityAeroLabel = new QLabel;
        layout->addRow(porosityAeroLabel, porosityAero);
        
        propButton = new QPushButton;
        connect(propButton, SIGNAL(clicked()), this, SLOT(propCalc()));
        layout->addRow(propButton);
        
        surfaceAreaTab = new QWidget;
        surfaceAreaTab->setLayout(layout);
        tabProps->addTab(surfaceAreaTab, "");
    }
    // Pore distribution
    {
        QFormLayout* layout = new QFormLayout;
        
        distFrom = new QSpinBox;
        distFrom->setMinimum(1);
        distFrom->setMaximum(100);
        distFrom->setValue(2);
        distFromLabel = new QLabel;
        layout->addRow(distFromLabel, distFrom);
        
        distTo = new QSpinBox;
        distTo->setMinimum(2);
        distTo->setMaximum(100);
        distTo->setValue(10);
        distToLabel = new QLabel;
        layout->addRow(distToLabel, distTo);
        
        distStep = new QSpinBox;
        distStep->setMinimum(1);
        distStep->setMaximum(20);
        distStep->setValue(2);
        distStepLabel = new QLabel;
        layout->addRow(distStepLabel, distStep);
        
        distButton = new QPushButton;
        connect(distButton, SIGNAL(clicked()), this, SLOT(distCalc()));
        layout->addRow(distButton);
        
        distributionTab = new QWidget;
        distributionTab->setLayout(layout);
        tabProps->addTab(distributionTab, "");
    }
}

void MainWindow::getColor()
{
    QColor colorGL;
    colorGL.setRgbF(glStructure->colors[0], glStructure->colors[1], glStructure->colors[2], glStructure->colors[3]);
    
    QColor color = QColorDialog::getColor(colorGL, this);
    if (color.isValid()) {
        glStructure->colors[0] = color.redF();
        glStructure->colors[1] = color.greenF();
        glStructure->colors[2] = color.blueF();
        glStructure->colors[3] = color.alphaF();
        colorButton->setStyleSheet(tr("* { background-color: rgb(%1, %2, %3); }")
            .arg(color.red()).arg(color.green())
            .arg(color.blue()));
        restructGL();
    }
}

void MainWindow::newFile()
{
    std::string command = "./" + app;
    int i = system(command.c_str());
    std::cout << "command out: " << i << std::endl;
    //MainWindow* other = new MainWindow;
    //other->show();
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

void MainWindow::save()
{
    if (curFile.isEmpty()) {
        saveAs();
    } else {
        saveFile(curFile);
    }
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty()) {
        return;
    }

    saveFile(fileName);
}

void MainWindow::saveImage()
{
    static std::vector<pImgF> imgs;
    if (imgs.empty()) {
        imgs.push_back(new ImagePNG);
        imgs.push_back(new ImageJPG);
    }
    QString filters = imgs[0]->Filter();
    for (uint i = 1; i < imgs.size(); ++i) {
        filters += ";;" + imgs[i]->Filter();
    }
    
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle(tr("Save image"));
    fileDialog.setNameFilter(filters);
    fileDialog.setViewMode(QFileDialog::List);
    fileDialog.setDirectory(QDir::currentPath());
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    
    if (fileDialog.exec() != QDialog::Accepted) {
        statusBar()->showMessage(tr("Error saving image!"), 5000);
        return;
    }
    
    QString fileName = fileDialog.selectedFiles()[0];
    
    if (fileName.isEmpty()) {
        statusBar()->showMessage(tr("Empty filename"), 5000);
        return;
    }
    pImgF pImg = nullptr;
    for (pImgF& img : imgs) {
        if (img->Filter() == fileDialog.selectedNameFilter()) {
            pImg = img;
            if (!fileName.endsWith(img->Ex())) {
                fileName.append(img->Ex());
            }
        }
    }
    if (!pImg) {
        statusBar()->showMessage(tr("No such file format!"), 5000);
        return;
    }
    uint w = glArea->width();
    uint h = glArea->height();
    uchar* imageData = (uchar*)malloc(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    QImage image(imageData, w, h, QImage::Format_RGB888);
    
    if (image.mirrored().save(fileName.toStdString().c_str())) {
        statusBar()->showMessage(tr("Image %1 saved").arg(fileName), 5000);
    } else {
        statusBar()->showMessage(tr("Error saving image!"), 5000);
    }
}

void MainWindow::setProgress(int value)
{
    progressBar->setValue(value);
    if (value >= progressBar->maximum()) {
        generateButton->setEnabled(true);
    }
}

void MainWindow::settings()
{
    int res = SettingsForm::dialog(tr("Settings"), setParams, glStructure->params);
    if (res == QDialogButtonBox::Yes) {
        // save settings && update ?
        if (drawGL->isChecked()) {
            glStructure->restruct();
        }
    } else {
        // do nothing
    }
}

void MainWindow::exportDLA()
{
    if (!glStructure->gen || !glStructure->gen->finished()) {
        statusBar()->showMessage(tr("Structure has not generated yet!"));
        return;
    }
    
    static std::vector<pTxtF> txts;
    if (txts.empty()) {
        txts.push_back(new TextDLA);
        txts.push_back(new TextTXT);
        txts.push_back(new TextDAT);
    }
    QString filters = txts[0]->Filter();
    for (uint i = 1; i < txts.size(); ++i) {
        filters += ";;" + txts[i]->Filter();
    }
    
    QString fileName;
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle(tr("Save structure"));
    fileDialog.setNameFilter(filters);
    fileDialog.setViewMode(QFileDialog::List);
    fileDialog.setDirectory(QDir::currentPath());
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    
    if (fileDialog.exec() == QDialog::Accepted) {
        fileName = fileDialog.selectedFiles()[0];
    } else {
        statusBar()->showMessage(tr("Error saving structure!"), 5000);
        return;
    }

    if (fileName.isEmpty()) {
        statusBar()->showMessage(tr("Empty filename"), 5000);
        return;
    }
    
    pTxtF pTxt = nullptr;
    for (pTxtF& txt : txts) {
        if (txt->Filter() == fileDialog.selectedNameFilter()) {
            pTxt = txt;
            if (!fileName.endsWith(txt->Ex())) {
                fileName.append(txt->Ex());
            }
        }
    }
    if (!pTxt) {
        statusBar()->showMessage(tr("No such file format!"), 5000);
        return;
    }

    glStructure->gen->save(fileName.toStdString(), pTxt->Format());

    statusBar()->showMessage(tr("Structure saved"), 5000);
}

void MainWindow::importDLA()
{
    surfaceArea->clear();
    densityAero->clear();
    porosityAero->clear();
    
    std::vector<QString> fltr;
    fltr.push_back("MultiDLA (*.dla *.txt *.dat)");
    fltr.push_back("OSM (*.dla *.txt *.dat)");
    fltr.push_back("DLCA (*.dla *.txt *.dat)");
    QString filters = fltr[0];
    for (uint i = 1; i < fltr.size(); ++i) {
        filters += ";;" + fltr[i];
    }
    
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle(tr("Load structure"));
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilter(filters);
    fileDialog.setViewMode(QFileDialog::List);
    fileDialog.setDirectory(QDir::currentPath());
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    
    QString fileName;

    if (fileDialog.exec() == QDialog::Accepted) {
        fileName = fileDialog.selectedFiles()[0];
    } else {
        statusBar()->showMessage(tr("Error loading structure!"), 5000);
        return;
    }
    
    int idx = -1;
    for (uint i = 0; i < fltr.size(); ++i) {
        if (fltr[i] == fileDialog.selectedNameFilter()) {
            idx = i;
        }
    }
    if (glStructure->gen) {
        delete glStructure->gen;
        glStructure->gen = nullptr;
    }
    if (!fileName.isEmpty()) {
        switch (idx) {
            case 0 : // pH < 7 - MultiDLA
                currentType = gen_mdla;
                glStructure->gen = new MultiDLA(this);
                break;
            case 1 : // OSM
                currentType = gen_osm;
                glStructure->gen = new OSM(this);
                break;
            case 2 : // DLCA
                currentType = gen_dlca;
                glStructure->gen = new DLCA(this);
                break;
            default :
                statusBar()->showMessage(tr("Some error!"), 5000);
                return;
        }
        if (fileName.endsWith(".dla")) {
            std::string fName = fileName.toStdString();
            glStructure->gen->load(fName, txt_dla);
        }
        if (fileName.endsWith(".txt")) {
            std::string fName = fileName.toStdString();
            glStructure->gen->load(fName, txt_txt);
        }
        if (fileName.endsWith(".dat")) {
            std::string fName = fileName.toStdString();
            glStructure->gen->load(fName, txt_dat);
        }
        updateGenerator();
        restructGL();
        statusBar()->showMessage(tr("Structure loaded"), 5000);
    } else {
        statusBar()->showMessage(tr("Error loading structure! Filename empty!"), 5000);
    }
}

void MainWindow::about()
{
    QDialog* aboutDialog = new QDialog(this);
    aboutDialog->setFixedSize(450, 400);
    aboutDialog->setWindowTitle(tr("About AeroGen"));
    QGridLayout* gLayout = new QGridLayout;
    QLabel* imageLabel = new QLabel;
    imageLabel->setPixmap(QPixmap::fromImage(QImage(":/icon.png").scaled(64, 64)));
    gLayout->addWidget(imageLabel, 0, 0);
    
    QLabel* label1 = new QLabel(tr("<p>The <b>AeroGen</b> demonstrates generation aerogel structures "
               "using OpenGL.</p></p>Version: %1<p><p>Authors:</p><p>Kolnoochenko Andrey</p>"
               "<p>Golubchikov Mihail</p><p> </p><p>Changelog:</p>").arg((QString)program_version));
    label1->setWordWrap(true);
    gLayout->addWidget(label1, 0, 1, 2, 1);
    //create changelog
    QStringList versions;
    versions.push_back(tr("<p>1.1.4 - Add shaders settings</p>"));
    versions.push_back(tr("<p>1.1.3 - Visualization fixes, add pore distribution & settings support</p>"));
    versions.push_back(tr("<p>1.1.2 - Codebase merge</p>"));
    versions.push_back(tr("<p>1.1.1 - Update updater, more settings, fix</p>"));
    versions.push_back(tr("<p>1.1.0 - Optimized algorithms, fix bugs</p>"));
    versions.push_back(tr("<p>1.0.7 - Add support for shaders</p>"));
    versions.push_back(tr("<p>1.0.6 - Add language support</p>"));
    versions.push_back(tr("<p>1.0.5 - Add OSM, fix some algorithms</p>"));
    versions.push_back(tr("<p>1.0.4 - Some fix, add checkbox to draw GL</p>"));
    versions.push_back(tr("<p>1.0.3 - Add DLCA generation, fix MultiDLA & many bugs</p>"));
    versions.push_back(tr("<p>1.0.2 - Add autoupdate, calculate aerogel properties, fix memory leak</p>"));
    versions.push_back(tr("<p>1.0.1 - Fix visualization</p>"));
    versions.push_back(tr("<p>1.0.0 - Initial program</p>"));
    
    QString vers;
    for (const QString& str : versions) {
        vers += str;
    }
    QTextEdit* changelog = new QTextEdit(vers);
    changelog->setReadOnly(true);
    //
    gLayout->addWidget(changelog, 2, 1);
    gLayout->addWidget(new QLabel(tr("<p>RS-Pharmcenter (c) 2016</p>")), 3, 1);
    QLabel* gitLabel = new QLabel;
    gitLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    gitLabel->setOpenExternalLinks(true);
    gitLabel->setText("<a href=\"https://github.com/rue-ryuzaki/AeroGen\">GitHub binary repository</a>");
    gLayout->addWidget(gitLabel, 4, 1);
    aboutDialog->setLayout(gLayout);
    aboutDialog->exec();
    
    delete aboutDialog;
}

void MainWindow::createActions()
{
    newAct = new QAction(this);
    newAct->setShortcuts(QKeySequence::New);
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
    
    saveImageAct = new QAction(this);
    //saveImageAct->setShortcuts(QKeySequence::Save);setShortcut(tr("Ctrl+G"));
    connect(saveImageAct, SIGNAL(triggered()), this, SLOT(saveImage()));
    
    exportDLAAct = new QAction(this);
    //exportingAct->setDisabled(true);//->isEnabled() = false;
    //exportingAct->setShortcuts(QKeySequence::SaveAs);setShortcut(tr("Ctrl+G"));
    connect(exportDLAAct, SIGNAL(triggered()), this, SLOT(exportDLA()));

    importDLAAct = new QAction(this);
    connect(importDLAAct, SIGNAL(triggered()), this, SLOT(importDLA()));
    
    exitAct = new QAction(this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    
    settingsAct = new QAction(this);
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(settings()));

    updateAct = new QAction(this);
    connect(updateAct, SIGNAL(triggered()), this, SLOT(updates()));

    aboutAct = new QAction(this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    
    feedbackAct = new QAction(this);
    connect(feedbackAct, SIGNAL(triggered()), this, SLOT(feedback()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveImageAct);
    fileMenu->addAction(exportDLAAct);
    fileMenu->addAction(importDLAAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    
    settingsMenu = menuBar()->addMenu("");
    languageMenu = settingsMenu->addMenu("");
    effectsMenu = settingsMenu->addMenu("");
    settingsMenu->addAction(settingsAct);
    
    helpMenu = menuBar()->addMenu("");
    helpMenu->addAction(updateAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addSeparator();
    helpMenu->addAction(feedbackAct);
    
    createSettingsMenu();
}

void MainWindow::createSettingsMenu()
{
    // language
    locales.push_back(":/lang/aerogen_ru.qm");
    locales.push_back(":/lang/aerogen_en.qm");
    
    languageRuAct = new QAction(tr("&Русский"), this);
    languageRuAct->setIcon(QIcon(":/ru.png"));
    languageRuAct->setCheckable(true);
    connect(languageRuAct, SIGNAL(triggered()), this, SLOT(switchToLanguage()));
    
    languageEnAct = new QAction(tr("&English"), this);
    languageEnAct->setIcon(QIcon(":/en.png"));
    languageEnAct->setCheckable(true);
    languageEnAct->setChecked(true);
    connect(languageEnAct, SIGNAL(triggered()), this, SLOT(switchToLanguage()));
    
    languageMenu->addAction(languageRuAct);
    languageMenu->addAction(languageEnAct);
    
    // shaders
    effectsDisableAct = new QAction(this);
    effectsDisableAct->setCheckable(true);
    effectsDisableAct->setChecked(true);
    connect(effectsDisableAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsLambertAct = new QAction(this);
    effectsLambertAct->setCheckable(true);
    connect(effectsLambertAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsWrapAroundAct = new QAction(this);
    effectsWrapAroundAct->setCheckable(true);
    connect(effectsWrapAroundAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    effectsPhongAct = new QAction(this);
    effectsPhongAct->setCheckable(true);
    connect(effectsPhongAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsBlinnAct = new QAction(this);
    effectsBlinnAct->setCheckable(true);
    connect(effectsBlinnAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsIsotropWardAct = new QAction(this);
    effectsIsotropWardAct->setCheckable(true);
    connect(effectsIsotropWardAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsOrenNayarAct = new QAction(this);
    effectsOrenNayarAct->setCheckable(true);
    connect(effectsOrenNayarAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    effectsCookTorranceAct = new QAction(this);
    effectsCookTorranceAct->setCheckable(true);
    effectsCookTorranceAct->setEnabled(false);
    connect(effectsCookTorranceAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsAnisotropAct = new QAction(this);
    effectsAnisotropAct->setCheckable(true);
    effectsAnisotropAct->setEnabled(false);
    connect(effectsAnisotropAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsAnisotropWardAct = new QAction(this);
    effectsAnisotropWardAct->setCheckable(true);
    effectsAnisotropWardAct->setEnabled(false);
    connect(effectsAnisotropWardAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsMinnaertAct = new QAction(this);
    effectsMinnaertAct->setCheckable(true);
    connect(effectsMinnaertAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsAshikhminShirleyAct = new QAction(this);
    effectsAshikhminShirleyAct->setCheckable(true);
    effectsAshikhminShirleyAct->setEnabled(false);
    connect(effectsAshikhminShirleyAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsCartoonAct = new QAction(this);
    effectsCartoonAct->setCheckable(true);
    connect(effectsCartoonAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsGoochAct = new QAction(this);
    effectsGoochAct->setCheckable(true);
    connect(effectsGoochAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsRimAct = new QAction(this);
    effectsRimAct->setCheckable(true);
    connect(effectsRimAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    effectsSubsurfaceAct = new QAction(this);
    effectsSubsurfaceAct->setCheckable(true);
    connect(effectsSubsurfaceAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsBidirectionalAct = new QAction(this);
    effectsBidirectionalAct->setCheckable(true);
    connect(effectsBidirectionalAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsHemisphericAct = new QAction(this);
    effectsHemisphericAct->setCheckable(true);
    connect(effectsHemisphericAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    effectsTrilightAct = new QAction(this);
    effectsTrilightAct->setCheckable(true);
    connect(effectsTrilightAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsLommelSeeligerAct = new QAction(this);
    effectsLommelSeeligerAct->setCheckable(true);
    connect(effectsLommelSeeligerAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    effectsStraussAct = new QAction(this);
    effectsStraussAct->setCheckable(true);
    connect(effectsStraussAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    effectsMenu->addAction(effectsDisableAct);
    effectsMenu->addSeparator();
    effectsMenu->addAction(effectsLambertAct);
    effectsMenu->addAction(effectsWrapAroundAct);
    effectsMenu->addAction(effectsPhongAct);
    effectsMenu->addAction(effectsBlinnAct);
    effectsMenu->addAction(effectsIsotropWardAct);
    effectsMenu->addAction(effectsOrenNayarAct);
    effectsMenu->addAction(effectsCookTorranceAct);
    effectsMenu->addAction(effectsAnisotropAct);
    effectsMenu->addAction(effectsAnisotropWardAct);
    effectsMenu->addAction(effectsMinnaertAct);
    effectsMenu->addAction(effectsAshikhminShirleyAct);
    effectsMenu->addAction(effectsCartoonAct);
    effectsMenu->addAction(effectsGoochAct);
    effectsMenu->addAction(effectsRimAct);
    effectsMenu->addAction(effectsSubsurfaceAct);
    effectsMenu->addAction(effectsBidirectionalAct);
    effectsMenu->addAction(effectsHemisphericAct);
    effectsMenu->addAction(effectsTrilightAct);
    effectsMenu->addAction(effectsLommelSeeligerAct);
    effectsMenu->addAction(effectsStraussAct);
}

void MainWindow::switchShaders()
{
    int shaders = -1;
    if (QObject::sender() == effectsDisableAct) {
        shaders = 0;
    }
    if (QObject::sender() == effectsLambertAct) {
        shaders = 1;
    }
    if (QObject::sender() == effectsWrapAroundAct) {
        shaders = 2;
    }
    if (QObject::sender() == effectsPhongAct) {
        shaders = 3;
    }
    if (QObject::sender() == effectsBlinnAct) {
        shaders = 4;
    }
    if (QObject::sender() == effectsIsotropWardAct) {
        shaders = 5;
    }
    if (QObject::sender() == effectsOrenNayarAct) {
        shaders = 6;
    }
    if (QObject::sender() == effectsCookTorranceAct) {
        shaders = 7;
    }
    if (QObject::sender() == effectsAnisotropAct) {
        shaders = 8;
    }
    if (QObject::sender() == effectsAnisotropWardAct) {
        shaders = 9;
    }
    if (QObject::sender() == effectsMinnaertAct) {
        shaders = 10;
    }
    if (QObject::sender() == effectsAshikhminShirleyAct) {
        shaders = 11;
    }
    if (QObject::sender() == effectsCartoonAct) {
        shaders = 12;
    }
    if (QObject::sender() == effectsGoochAct) {
        shaders = 13;
    }
    if (QObject::sender() == effectsRimAct) {
        shaders = 14;
    }
    if (QObject::sender() == effectsSubsurfaceAct) {
        shaders = 15;
    }
    if (QObject::sender() == effectsBidirectionalAct) {
        shaders = 16;
    }
    if (QObject::sender() == effectsHemisphericAct) {
        shaders = 17;
    }
    if (QObject::sender() == effectsTrilightAct) {
        shaders = 18;
    }
    if (QObject::sender() == effectsLommelSeeligerAct) {
        shaders = 19;
    }
    if (QObject::sender() == effectsStraussAct) {
        shaders = 20;
    }
    if (shaders == -1) {
        return;
    }
    glStructure->needInit = shaders;
    if (shaders != 0 && !glStructure->supportShaders()) {
        if (glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shaders = 0;
        }
    }
    glStructure->enableShader(shaders);
    shaders = glStructure->shadersStatus();
    effectsDisableAct->setChecked(shaders == 0);
    effectsLambertAct->setChecked(shaders == 1);
    effectsWrapAroundAct->setChecked(shaders == 2);
    effectsPhongAct->setChecked(shaders == 3);
    effectsBlinnAct->setChecked(shaders == 4);
    effectsIsotropWardAct->setChecked(shaders == 5);
    effectsOrenNayarAct->setChecked(shaders == 6);
    effectsCookTorranceAct->setChecked(shaders == 7);
    effectsAnisotropAct->setChecked(shaders == 8);
    effectsAnisotropWardAct->setChecked(shaders == 9);
    effectsMinnaertAct->setChecked(shaders == 10);
    effectsAshikhminShirleyAct->setChecked(shaders == 11);
    effectsCartoonAct->setChecked(shaders == 12);
    effectsGoochAct->setChecked(shaders == 13);
    effectsRimAct->setChecked(shaders == 14);
    effectsSubsurfaceAct->setChecked(shaders == 15);
    effectsBidirectionalAct->setChecked(shaders == 16);
    effectsHemisphericAct->setChecked(shaders == 17);
    effectsTrilightAct->setChecked(shaders == 18);
    effectsLommelSeeligerAct->setChecked(shaders == 19);
    effectsStraussAct->setChecked(shaders == 20);
    
    restructGL();
}

void MainWindow::switchToLanguage()
{
    int language = -1;
    if (QObject::sender() == languageRuAct) {
        language = 0;
    }
    if (QObject::sender() == languageEnAct) {
        language = 1;
    }
    if (language == -1) {
        return;
    }
    appTranslator->load(locales[language]); 
    languageRuAct->setChecked(language == 0);
    languageEnAct->setChecked(language == 1);
    retranslate();
}

void MainWindow::switchToLanguageB()
{
    int language = -1;
    if (QObject::sender() == buttonRu) {
        language = 0;
    }
    if (QObject::sender() == buttonEn) {
        language = 1;
    }
    if (language == -1) {
        return;
    }
    appTranslator->load(locales[language]); 
    languageRuAct->setChecked(language == 0);
    languageEnAct->setChecked(language == 1);
    retranslate();
    delete langDialog;
}

void MainWindow::updates()
{
    try {
        if (checkUpdate()) {
            updated();
        } else {
            QMessageBox::warning(this, tr("Update AeroGen"), tr("Your program is up to date!"));
        }
    } catch (...) {
        QMessageBox::warning(this, tr("Update AeroGen"), tr("Update Error!"));
    }
}

void MainWindow::updated()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Update AeroGen"),
            tr("Update available!\nWould you like to update application now?"),
            QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (fileExists(updaterFile.c_str())) {
            QMessageBox::warning(this, tr("Update AeroGen"), tr("Application will be closed!"));

            std::thread t(startUpdater);
            t.detach();
            sleep(1);
            exit(0);
        } else {
            QMessageBox::warning(this, tr("Update AeroGen"), tr("Update canceled! updater not found!"));
        }
        //QApplication::quit();
    } else {
        statusBar()->showMessage(tr("Update canceled!"));
    }
}

void MainWindow::updateUpdater()
{
    if (!DownloadUpdater()) {
        QMessageBox::warning(this, tr("Update updater"), tr("Update error! updater not found!"));
    }
}

void MainWindow::feedback()
{
    QDialog* feedbackDialog = new QDialog(this);
    feedbackDialog->setFixedSize(450, 350);
    feedbackDialog->setWindowTitle(tr("Feedback"));
    
    QFormLayout*layout = new QFormLayout;
    feedbackProblem     = new QLineEdit;
    feedbackName        = new QLineEdit;
    feedbackDescription = new QTextEdit;
    layout->addRow(new QLabel(tr("Problem:")), feedbackProblem);
    layout->addRow(new QLabel(tr("Your name:")), feedbackName);
    layout->addRow(new QLabel(tr("Description:")), feedbackDescription);
    QPushButton* fbSend = new QPushButton(tr("Send"));
    connect(fbSend, SIGNAL(clicked()), this, SLOT(feedbackSend()));
    layout->addRow(fbSend);
    feedbackDialog->setLayout(layout);
    feedbackDialog->exec();
    delete feedbackProblem;
    delete feedbackName;
    delete feedbackDescription;
    delete feedbackDialog;
}

void MainWindow::feedbackSend()
{
    if (feedbackProblem->text().isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Problem empty"));
        return;
    } else if (feedbackName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Name empty"));
        return;
    } else if (feedbackDescription->toPlainText().isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Description empty"));
        return;
    }
    QMessageBox::warning(this, tr("Feedback"), tr("Not supported."));
}

void MainWindow::loadFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }
    curFile = fileName;
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    structureType->setCurrentIndex(in.readLine().toInt());
    sizesEdit->setText  (in.readLine());
    poreDLA->setValue   (in.readLine().toDouble());
    initDLA->setValue   (in.readLine().toInt());
    stepDLA->setValue   (in.readLine().toInt());
    hitDLA->setValue    (in.readLine().toInt());
    clusterDLA->setValue(in.readLine().toInt());
    cellSize->setValue  (in.readLine().toDouble());
    parameter.method   = structureType->currentIndex();
    parameter.sizes    = sizesEdit->text().toStdString();
    parameter.porosity = poreDLA->value();
    parameter.init     = initDLA->value();
    parameter.step     = stepDLA->value();
    parameter.hit      = hitDLA->value();
    parameter.cluster  = clusterDLA->value();
    parameter.cellSize = cellSize->value();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("File loaded"), 5000);
}

void MainWindow::saveFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }
    curFile = fileName;
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << parameter.method    << endl;
    out << QString::fromStdString(parameter.sizes) << endl;
    out << parameter.porosity  << endl;
    out << parameter.init      << endl;
    out << stepDLA->value()    << endl;
    out << hitDLA->value()     << endl;
    out << clusterDLA->value() << endl;
    out << parameter.cellSize  << endl;
    QApplication::restoreOverrideCursor();
    
    statusBar()->showMessage(tr("File saved"), 5000);
}

void MainWindow::start()
{
    if (glStructure->gen && glStructure->gen->run) {
        statusBar()->showMessage(tr("Calculation already runned"), 5000);
        return;
    }
    progressBar->setValue(0);
    surfaceArea->clear();
    densityAero->clear();
    porosityAero->clear();
    sizesEdit->setStyleSheet("");
    bool ok = false;
    QString text = sizesEdit->text();
    QRegExp regExp("([0-9]+) *x *([0-9]+) *x *([0-9]+)");
    Sizes size;
    int sizemax = 0;
    if (regExp.exactMatch(text)) {
        int x = regExp.cap(1).toInt();
        int y = regExp.cap(2).toInt();
        int z = regExp.cap(3).toInt();
        if (x >= 20 && x <= 500 && y >= 20 && y <= 500 && z >= 20 && z <= 500) {
            size.x = x;
            size.y = y;
            size.z = z;
            ok = true;
            if (x > sizemax) {
                sizemax = x;
            }
            if (y > sizemax) {
                sizemax = y;
            }
            if (z > sizemax) {
                sizemax = z;
            }
        } else {
            sizesEdit->setStyleSheet("* { background-color: rgb(255, 50, 0); }");
            statusBar()->showMessage(tr("Sizes interval from 50 to 500 nm!"));
            QMessageBox::warning(this, tr("Warning"), tr("Sizes interval from 50 to 500 nm!"));
            return;
        }
    }
    if (!ok) {
        sizesEdit->setStyleSheet("* { background-color: rgb(255, 50, 0); }");
        statusBar()->showMessage(tr("Error parsing sizes!"));
        QMessageBox::warning(this, tr("Warning"), tr("Error parsing sizes!"));
        return;
    }
    double por = double(poreDLA->value()) / 100;
    int initial = initDLA->value();
    int step = stepDLA->value();
    int hit = hitDLA->value();
    size_t cluster = clusterDLA->value();
    double cellsize = cellSize->value();
    
    parameter.method   = structureType->currentIndex();
    parameter.sizes    = sizesEdit->text().toStdString();
    parameter.porosity = poreDLA->value();
    parameter.init     = initDLA->value();
    parameter.step     = stepDLA->value();
    parameter.hit      = hitDLA->value();
    parameter.cluster  = clusterDLA->value();
    parameter.cellSize = cellSize->value();
    
    glStructure->setCamera(sizemax);
    if (glStructure->gen) {
        delete glStructure->gen;
        glStructure->gen = nullptr;
    }
    switch (structureType->currentIndex()) {
        case 0 : // pH < 7 - MultiDLA
            std::cout << "Organic? (MultiDLA)" << std::endl;
            currentType = gen_mdla;
            glStructure->gen = new MultiDLA(this);
            break;
        case 1 : // pH > 7 - Spheres Inorganic
            std::cout << "Inorganic? (Spheres)" << std::endl;;
            currentType = gen_osm;
            glStructure->gen = new OSM(this);
            break;
        case 2 : // DLCA
            std::cout << "(Clusters DLCA)" << std::endl;
            currentType = gen_dlca;
            glStructure->gen = new DLCA(this);
            break;
        default :
            std::cout << "Undefined method!\n";
            QMessageBox::warning(this, tr("Warning"), tr("Undefined method"));
            return;
    }
    updateGenerator();
    glStructure->gen->run = true;
    glStructure->gen->cancel(false);
    
    std::thread t(threadGen, size, por, initial, step, hit, cluster, cellsize);
    t.detach();
    
    clearLayout(genLayout1);
    createLayout1();
    retranslate();
    generateButton->setEnabled(false);
    
    while (true) {
        if (glStructure->gen) {
            while (true) {
                if (glStructure->gen->field()) {
                    sizesLabel2->setText(tr("Sizes (in nm):") + tr("%1x%2x%3")
                        .arg(glStructure->gen->field()->sizes().x)
                        .arg(glStructure->gen->field()->sizes().y)
                        .arg(glStructure->gen->field()->sizes().z));
                    genLayout1->addRow(sizesLabel2);
                    break;
                }
            }
            break;
        }
    }
    genLayout1->addRow(cellSizeLabel2);
    genLayout1->addRow(poreLabel2);
    if (parameter.method == 0) {
        genLayout1->addRow(initLabel2);
        genLayout1->addRow(stepLabel2);
        genLayout1->addRow(hitLabel2);
    }
    genLayout1->addRow(clusterLabel2);
    genLayout1->addRow(statusLabel, progressBar);
    genLayout1->addRow(generateButton);
    genLayout1->addRow(stopButton);
}

void MainWindow::threadGen(const Sizes& sizes, double por, int initial, int step,
        int hit, size_t cluster, double cellsize)
{
    glStructure->gen->generate(sizes, por, initial, step, hit, cluster, cellsize);
    glStructure->gen->run = false;
}

void MainWindow::threadRunDistr(double cellSize, double dFrom, double dTo, double dStep)
{
    distributor->calculate(glStructure->gen->field(), cellSize, dFrom, dTo, dStep);
}

void MainWindow::stop()
{
    generateButton->setEnabled(true);
    if (glStructure->gen) {
        glStructure->gen->cancel(true);
    }
}

void MainWindow::stopDistr()
{
    distributor->cancel();
    waitDialog->setWindowTitle(tr("Cancelling..."));
    statusBar()->showMessage(tr("Pore distribution calculating cancelling..."), 5000);
}

void MainWindow::closeWaitDialog()
{
    waitDialog->close();
}

void MainWindow::distrFinished()
{
    distr = distributor->distribution();
    QDialog* distrDialog = new QDialog(this);
    distrDialog->setMinimumSize(350,250);
    distrDialog->setWindowTitle(tr("Pore distribution"));

    QFormLayout*layout = new QFormLayout;
    //layout->addRow(new QLabel(tr("Pore distribution don't work yet")));
    layout->addRow(new QLabel(tr("Testing version")));
    // table!
    QTableWidget* table = new QTableWidget;
    table->setRowCount(distr.size());
    table->setColumnCount(3);
    QStringList header;
    header.append(tr("Pore size, nm"));
    header.append(tr("Volume, nm3"));
    header.append(tr("Percentage, %"));
    table->setHorizontalHeaderLabels(header);
    double sum = 0.0;
    if (!distr.empty()) {
        double prevVol = distr.back().vol;
        distr.back().count = prevVol / ((4.0 / 3) * M_PI * distr.back().r * distr.back().r * distr.back().r);
        for (int i = distr.size() - 2; i >= 0; --i) {
            double currVol = distr[i].vol - prevVol;
            distr[i].count = currVol / ((4.0 / 3) * M_PI * distr[i].r * distr[i].r * distr[i].r);
            sum += distr[i].count;
            prevVol = distr[i].vol;
        }
        for (uint i = 0; i < distr.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(QString::number(2 * distr[i].r)));
            table->setItem(i, 1, new QTableWidgetItem(QString::number(distr[i].vol)));
            double x = 100 * distr[i].count / sum;
            table->setItem(i, 2, new QTableWidgetItem(QString::number(x)));
        }
    }
    table->resizeColumnsToContents();
    
    layout->addRow(table);
    // end
    distrDialog->setLayout(layout);
    distrDialog->exec();
    
    delete distrDialog;
}

void MainWindow::propLoad()
{
    statusBar()->showMessage(tr("Not available yet!"));
    return;
}

void MainWindow::propCalc()
{
    surfaceArea->clear();
    densityAero->clear();
    porosityAero->clear();
    double sqrArea = 0.0;
    double denAero = 0.0;
    double porosity = 0.0;

    if (!glStructure->gen->finished()) {
        statusBar()->showMessage(tr("Structure not ready yet!"));
        return;
    }
    sqrArea = glStructure->gen->surfaceArea(density->value());
    
    glStructure->gen->density(density->value(), denAero, porosity);
    surfaceArea->setText(tr(dtos(sqrArea, 2, true).c_str()));
    densityAero->setText(tr(dtos(denAero, 2, true).c_str()));
    porosityAero->setText(tr(dtos((porosity) * 100, 2, true).c_str()));
}

void MainWindow::distCalc()
{
    double dFrom = double(distFrom->value());
    double dTo   = double(distTo->value());
    double dStep = double(distStep->value());
    if (dFrom > dTo) {
        statusBar()->showMessage(tr("dFrom > dTo!"));
        return;
    }

    //if (dTo > cellSize->value() * glStructure->multiDLA->GetField()->GetSize().GetCoord(0)) {
    //    statusBar()->showMessage(tr("dFrom > dTo!"));
    //    return;
    //}
    if (!glStructure->gen || !glStructure->gen->finished()) {
        statusBar()->showMessage(tr("Structure not ready yet!"));
        return;
    }
    std::thread t(threadRunDistr, cellSize->value(), dFrom, dTo, dStep);
    t.detach();
    
    waitDialog->setWindowTitle(tr("Wait"));
    waitDialog->exec();
}

void MainWindow::updateGenerator()
{
    QString text;
    switch (currentType) {
        case gen_mdla :
            text = "MultiDLA";
            break;
        case gen_osm :
            text = "OSM";
            break;
        case gen_dlca :
            text = "DLCA";
            break;
        default :
            currentMethod->setText(tr("Undefined"));
            return;
    }
    currentMethod->setText(text);
}

void MainWindow::restructGL()
{
    if (drawGL->isChecked()) {
        glStructure->restruct();
    }
}

void MainWindow::axesGL()
{
    glStructure->showAxes = showAxes->isChecked();
    if (drawGL->isChecked()) {
        glStructure->restruct();
    }
}

void MainWindow::borderGL()
{
    glStructure->showBorders = showBorders->isChecked();
    if (drawGL->isChecked()) {
        glStructure->restruct();
    }
}

void MainWindow::changeType(int value)
{
    bool enabled = (value == 0);
    initDLA->setEnabled(enabled);
    stepDLA->setEnabled(enabled);
    hitDLA->setEnabled(enabled);
    initLabel->setEnabled(enabled);
    stepLabel->setEnabled(enabled);
    hitLabel->setEnabled(enabled);
}

void MainWindow::changeDrawGL()
{
    glArea->setEnabled(drawGL->isChecked());
    glStructure->drawGL = drawGL->isChecked();
    showAxes->setEnabled(drawGL->isChecked());
    showBorders->setEnabled(drawGL->isChecked());
    restructGL();
}

bool MainWindow::event(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate(); 
    }
    return QMainWindow::event(event);
}

void MainWindow::retranslate()
{
    // menu
    newAct->setText(tr("&New")); 
    newAct->setStatusTip(tr("Create a new file"));
    openAct->setText(tr("&Open...")); 
    openAct->setStatusTip(tr("Open an existing file"));
    saveAct->setText(tr("&Save")); 
    saveAct->setStatusTip(tr("Save the document to disk"));
    saveAsAct->setText(tr("Save &As...")); 
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    saveImageAct->setText(tr("Save &Image"));
    saveImageAct->setStatusTip(tr("Save the image to disk"));
    exportDLAAct->setText(tr("&Save structure"));
    exportDLAAct->setStatusTip(tr("Export the structure under a new name"));
    importDLAAct->setText(tr("&Load structure"));
    importDLAAct->setStatusTip(tr("Import the structure from file"));
    exitAct->setText(tr("E&xit"));
    updateAct->setText(tr("Check for &updates"));
    aboutAct->setText(tr("&About"));
    aboutQtAct->setText(tr("About &Qt"));
    feedbackAct->setText(tr("&Feedback"));
    
    fileMenu->setTitle(tr("&File"));
    settingsMenu->setTitle(tr("&Settings"));
    languageMenu->setTitle(tr("&Language"));
    effectsMenu->setTitle(tr("&Effects"));
    effectsDisableAct->setText(tr("Disable"));
    effectsLambertAct->setText(tr("Lambert"));
    effectsWrapAroundAct->setText(tr("Wrap-around"));
    effectsPhongAct->setText(tr("Phong"));
    effectsBlinnAct->setText(tr("Blinn"));
    effectsIsotropWardAct->setText(tr("Isotropic Ward"));
    effectsOrenNayarAct->setText(tr("Oren-Nayar"));
    effectsCookTorranceAct->setText(tr("Cook-Torrance"));
    effectsAnisotropAct->setText(tr("Anisotropic"));
    effectsAnisotropWardAct->setText(tr("Anisotropic Ward"));
    effectsMinnaertAct->setText(tr("Minnaert"));
    effectsAshikhminShirleyAct->setText(tr("Ashikhmin-Shirley"));
    effectsCartoonAct->setText(tr("Cartoon"));
    effectsGoochAct->setText(tr("Gooch"));
    effectsRimAct->setText(tr("Rim"));
    effectsSubsurfaceAct->setText(tr("Subsurface"));
    effectsBidirectionalAct->setText(tr("Bidirectional"));
    effectsHemisphericAct->setText(tr("Hemispheric"));
    effectsTrilightAct->setText(tr("Trilight"));
    effectsLommelSeeligerAct->setText(tr("Lommel-Seeliger"));
    effectsStraussAct->setText(tr("Strauss"));
    settingsAct->setText(tr("Settings"));
    helpMenu->setTitle(tr("&Help"));
    
    // panels
    currMethodLabel->setText(tr("Generation method"));
    drawGL->setText(tr("Draw structure"));
    showAxes->setText(tr("Show axes"));
    showBorders->setText(tr("Show borders"));
    colorLabel->setText(tr("Color:"));
    colorButton->setText(tr("Select"));
    statusLabel->setText(tr("Status:"));
    
    panelBox->setTitle(tr("Generation"));
    methodLabel->setText(tr("Method:"));
    sizesLabel->setText(tr("Sizes (in nm):"));
    cellSizeLabel->setText(tr("Line cell size, nm:"));
    poreLabel->setText(tr("Porosity, %:"));
    initLabel->setText(tr("Init count:"));
    stepLabel->setText(tr("Step:"));
    hitLabel->setText(tr("Hit:"));
    clusterLabel->setText(tr("Cluster:"));
    startButton->setText(tr("Generate"));
    if (glStructure->gen) {
        if (glStructure->gen->field()) {
            sizesLabel2->setText(tr("Sizes (in nm):") + tr("%1x%2x%3").arg(glStructure->gen->field()->sizes().x)
                .arg(glStructure->gen->field()->sizes().y)
                .arg(glStructure->gen->field()->sizes().z));
        } else {
            sizesLabel2->setText(tr("Sizes (in nm):") + tr("%1x%2x%3").arg(0).arg(0).arg(0));
        }
    } else {
        sizesLabel2->setText(tr("Sizes (in nm):") + tr("%1x%2x%3").arg(0).arg(0).arg(0));
    }
    cellSizeLabel2->setText(tr("Line cell size, nm:") + QString::number(parameter.cellSize));
    poreLabel2->setText(tr("Porosity, %:") + QString::number(parameter.porosity));
    initLabel2->setText(tr("Init count:") + QString::number(parameter.init));
    stepLabel2->setText(tr("Step:") + QString::number(parameter.step));
    hitLabel2->setText(tr("Hit:") + QString::number(parameter.hit));
    clusterLabel2->setText(tr("Cluster:") + QString::number(parameter.cluster));
    generateButton->setText(tr("Generate new"));
    stopButton->setText(tr("Stop"));
    
    cancelDistrButton->setText(tr("Stop"));
    
    propsBox->setTitle(tr("Properties"));
    densityLabel->setText(tr("Density, kg/m<sup>3</sup>:"));
    
    tabProps->setTabText(0, tr("Specific surface area"));
    surfaceAreaLabel->setText(tr("Specific surface area, m<sup>2</sup>/g:"));
    densityAeroLabel->setText(tr("Aerogel density, kg/m<sup>3</sup>:"));
    porosityAeroLabel->setText(tr("Current porosity, %:"));
    propButton->setText(tr("Calculate"));
    
    tabProps->setTabText(1, tr("Distribution"));
    distFromLabel->setText(tr("from, nm:"));
    distToLabel->setText(tr("to, nm:"));
    distStepLabel->setText(tr("step, nm:"));
    distButton->setText(tr("Calculate"));
    
    updateGenerator();
}
