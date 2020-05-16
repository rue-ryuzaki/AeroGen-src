#include "mainwindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef QWT_DEFINED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#pragma GCC diagnostic pop
#endif // QWT_DEFINED

#include "multidla/multidla.h"
#include "osm/osm.h"
#include "dlca/dlca.h"
#include "dlca2/dlca2.h"
#include "mxdla/mxdla.h"
#include "xdla/xdla.h"

#include "baseformats.h"
#include "checker.h"
#include "functions.h"
#include "settingsform.h"

namespace aerogen {
StructureGL* MainWindow::m_glStructure;
Distributor* MainWindow::m_distributor;

const std::string settingsFile = "settings.json";

void clearLayout(QLayout* layout)
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

MainWindow::MainWindow()
    : QMainWindow(),
      m_panelWidth(),
      m_setParams(),
      m_locales(),
      m_translator(this),
      m_currentType(gen_none),
      m_centralWidget(),
      m_glArea(),
      m_panelBox(nullptr),
      m_propsBox(),
      m_genLayout1(nullptr),
      m_parameter(),
      m_drawGL(),
      m_showAxes(),
      m_showBorders(),
      m_tabProps(),
      m_curFile(),
      m_feedbackProblem(nullptr),
      m_feedbackName(nullptr),
      m_feedbackDescription(nullptr),
      m_structureType(nullptr),
      m_poreDLA(nullptr),
      m_initDLA(nullptr),
      m_stepDLA(nullptr),
      m_hitDLA(nullptr),
      m_clusterDLA(nullptr),
      m_distFrom(),
      m_distTo(),
      m_distStep(),
      m_distBoundary(),
      m_density(),
      m_cellSize(nullptr),
      m_boundaryType(nullptr),
      m_progressBar(nullptr),
      m_progressDistrBar(nullptr),
      m_currentMethod(),
      m_surfaceArea(),
      m_densityAero(),
      m_porosityAero(),
      m_monteCarloAero(),
      m_sizesEdit(nullptr),
      m_generateButton(nullptr),
      m_colorButton(),
      m_startButton(nullptr),
      m_cancelButton(),
      m_cancelDistrButton(),
      m_stopButton(nullptr),
      m_loadButton(),
      m_propButton(),
      m_distButton(),
      m_buttonRu(),
      m_buttonEn(),
      m_langDialog(),
      m_waitDialog(nullptr),
      m_surfaceAreaTab(),
      m_distributionTab(),
      m_currMethodLabel(),
      m_colorLabel(),
      m_statusLabel(nullptr),
      m_methodLabel(nullptr),
      m_sizesLabel(nullptr),
      m_poreLabel(nullptr),
      m_initLabel(nullptr),
      m_stepLabel(nullptr),
      m_hitLabel(nullptr),
      m_clusterLabel(nullptr),
      m_cellSizeLabel(nullptr),
      m_boundaryTypeLabel(nullptr),
      m_densityLabel(),
      m_sizesLabel2(nullptr),
      m_cellSizeLabel2(nullptr),
      m_poreLabel2(nullptr),
      m_initLabel2(nullptr),
      m_stepLabel2(nullptr),
      m_hitLabel2(nullptr),
      m_clusterLabel2(nullptr),
      m_boundaryLabel2(nullptr),
      m_surfaceAreaLabel(),
      m_densityAeroLabel(),
      m_porosityAeroLabel(),
      m_monteCarloAeroLabel(),
      m_distFromLabel(),
      m_distToLabel(),
      m_distStepLabel(),
      m_distBoundaryLabel(),
      m_fileMenu(nullptr),
      m_settingsMenu(nullptr),
      m_languageMenu(nullptr),
      m_effectsMenu(nullptr),
      m_helpMenu(nullptr),
      m_newAct(),
      m_openAct(),
      m_saveAct(),
      m_saveAsAct(),
      m_saveImageAct(),
      m_exportDLAAct(),
      m_importDLAAct(),
      m_exitAct(),
      m_languageRuAct(tr("&Русский"), this),
      m_languageEnAct(tr("&English"), this),
      m_effectsDisableAct(),
      m_effectsLambertAct(),
      m_effectsWrapAroundAct(),
      m_effectsPhongAct(),
      m_effectsBlinnAct(),
      m_effectsIsotropWardAct(),
      m_effectsOrenNayarAct(),
      m_effectsCookTorranceAct(),
      m_effectsAnisotropAct(),
      m_effectsAnisotropWardAct(),
      m_effectsMinnaertAct(),
      m_effectsAshikhminShirleyAct(),
      m_effectsCartoonAct(),
      m_effectsGoochAct(),
      m_effectsRimAct(),
      m_effectsSubsurfaceAct(),
      m_effectsBidirectionalAct(),
      m_effectsHemisphericAct(),
      m_effectsTrilightAct(),
      m_effectsLommelSeeligerAct(),
      m_effectsStraussAct(),
      m_settingsAct(),
      m_updateAct(),
      m_aboutAct(),
      m_aboutQtAct(),
      m_feedbackAct(),
      m_effectActions(),
      m_languageActions()
{
    qApp->installTranslator(&m_translator);
    setWindowIcon(QIcon(":/icon.png"));
    setCentralWidget(&m_centralWidget);
    // create GL widget
    m_glStructure = new StructureGL;
    m_distributor = new Distributor(this);
    m_glArea.setWidget(m_glStructure);
    m_glArea.setWidgetResizable(true);
    m_glArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_glArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_glArea.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_glArea.setMinimumSize(250, 250);
    
    //create settings Panel
#ifdef _WIN32
// windows
    m_panelWidth = 260;
#else
    m_panelWidth = 300;
#endif
    createPanel();
    createProps();
    createTab();
    createActions();
    createMenus();

    QGridLayout* centralLayout = new QGridLayout;
    {
        int32_t hmax = 45;
        QWidget* widget1 = new QWidget;
        QFormLayout* layout1 = new QFormLayout;
        m_currentMethod.setToolTip(QString::fromStdString("MultiDLA\n\nOSM\n\nDLCA\n\n") + tr("Undefined"));
        updateGenerator();
        m_currentMethod.setFixedWidth(100);
//        widget1->setMaximumHeight(hmax);
        m_currentMethod.setReadOnly(true);
        layout1->addRow(&m_currMethodLabel, &m_currentMethod);
        m_drawGL.setChecked(true);
        connect(&m_drawGL, SIGNAL(clicked()), this, SLOT(changeDrawGL()));
        layout1->addRow(&m_drawGL);
        widget1->setLayout(layout1);
        centralLayout->addWidget(widget1, 0, 0);
        
        QWidget* widget2 = new QWidget;
        //widget2->setMaximumHeight(hmax);
        QFormLayout* layout2 = new QFormLayout;
        m_showAxes.setChecked(false);
        m_showAxes.setEnabled(m_drawGL.isChecked());
        connect(&m_showAxes, SIGNAL(clicked()), this, SLOT(axesGL()));
        layout2->addRow(&m_showAxes);

        m_showBorders.setChecked(false);
        m_showBorders.setEnabled(m_drawGL.isChecked());
        connect(&m_showBorders, SIGNAL(clicked()), this, SLOT(borderGL()));
        layout2->addRow(&m_showBorders);
        
        widget2->setLayout(layout2);
        centralLayout->addWidget(widget2, 0, 1);
        
        QWidget* widget3 = new QWidget;
        QFormLayout* layout3 = new QFormLayout;
        m_colorButton.setAutoFillBackground(true);
        widget3->setMaximumHeight(hmax);
        connect(&m_colorButton, SIGNAL(clicked()), this, SLOT(getColor()));
        layout3->addRow(&m_colorLabel, &m_colorButton);
        widget3->setLayout(layout3);
        centralLayout->addWidget(widget3, 0, 2);

        m_progressDistrBar = new QProgressBar;
        m_progressDistrBar->setMinimum(0);
        m_progressDistrBar->setMaximum(0);
        m_progressDistrBar->setValue(0);
        connect(&m_cancelDistrButton, SIGNAL(clicked()), this, SLOT(stopDistr()));
        m_waitDialog = new QDialog(this, Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
        m_waitDialog->setFixedSize(200, 85);
        QFormLayout* waitlayout = new QFormLayout;
        waitlayout->addRow(m_progressDistrBar);
        waitlayout->addRow(&m_cancelDistrButton);
        m_waitDialog->setLayout(waitlayout);
    }
    centralLayout->addWidget(&m_glArea, 1, 0, 4, 3);
    centralLayout->addWidget(m_panelBox, 0, 3, 2, 1);
    centralLayout->addWidget(&m_propsBox, 2, 3);
    centralLayout->addWidget(&m_tabProps, 3, 3);
    QWidget* widgetE = new QWidget;
    centralLayout->addWidget(widgetE, 4, 3);
    m_centralWidget.setLayout(centralLayout);

    statusBar();
    setWindowTitle(tr("AeroGen"));
    m_colorButton.setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
                                .arg(int(m_glStructure->colors[0] * 255))
                                .arg(int(m_glStructure->colors[1] * 255))
                                .arg(int(m_glStructure->colors[2] * 255)));
    if (!loadSettings()) {
        loadDefault();
    }
#ifdef QT_NETWORK_LIB
    // check for updates!
    try {
        if (md5UpdaterHash() != updaterFileHash()) {
            if (DownloadUpdater()) {
                QThread::sleep(1);
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
#endif // QT_NETWORK_LIB
    retranslate();
#ifdef _WIN32
// windows
    resize(800, 560);
#else
    resize(840, 620);
#endif
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
    QString fileName = QFileDialog::getOpenFileName(this, QString(), QString(),
                                                    "JsonX (*.jsonx)");
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

void MainWindow::save()
{
    if (m_curFile.isEmpty()) {
        saveAs();
    } else {
        saveFile(m_curFile);
    }
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString(), QString(),
                                                    "JsonX (*.jsonx)");
    if (fileName.isEmpty()) {
        return;
    }
    if (!fileName.endsWith(".jsonx")) {
        fileName.append(".jsonx");
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
    QString filters = imgs[0]->filter();
    for (size_t i = 1; i < imgs.size(); ++i) {
        filters += ";;" + imgs[i]->filter();
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
        if (img->filter() == fileDialog.selectedNameFilter()) {
            pImg = img;
            if (!fileName.endsWith(img->extens())) {
                fileName.append(img->extens());
            }
        }
    }
    if (!pImg) {
        statusBar()->showMessage(tr("No such file format!"), 5000);
        return;
    }
    int32_t w = m_glArea.width();
    int32_t h = m_glArea.height();
    uchar* imageData = reinterpret_cast<uchar*>(malloc(size_t(w * h * 3)));
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    QImage image(imageData, w, h, QImage::Format_RGB888);

    if (image.mirrored().save(fileName.toStdString().c_str())) {
        statusBar()->showMessage(tr("Image %1 saved").arg(fileName), 5000);
    } else {
        statusBar()->showMessage(tr("Error saving image!"), 5000);
    }
}

void MainWindow::exportDLA()
{
    if (!m_glStructure->gen || !m_glStructure->gen->finished()) {
        statusBar()->showMessage(tr("Structure has not generated yet!"));
        return;
    }

    static std::vector<pTxtF> txts;
    if (txts.empty()) {
        txts.push_back(new TextDLA);
        txts.push_back(new TextTXT);
        txts.push_back(new TextDAT);
    }
    QString filters = txts[0]->filter();
    for (size_t i = 1; i < txts.size(); ++i) {
        filters += ";;" + txts[i]->filter();
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
        if (txt->filter() == fileDialog.selectedNameFilter()) {
            pTxt = txt;
            if (!fileName.endsWith(txt->extens())) {
                fileName.append(txt->extens());
            }
        }
    }
    if (!pTxt) {
        statusBar()->showMessage(tr("No such file format!"), 5000);
        return;
    }

    m_glStructure->gen->save(fileName.toStdString(), pTxt->format());

    statusBar()->showMessage(tr("Structure saved"), 5000);
}

void MainWindow::importDLA()
{
    m_surfaceArea.clear();
    m_densityAero.clear();
    m_porosityAero.clear();

    std::vector<QString> fltr;
    fltr.push_back("MultiDLA (*.dla *.txt *.dat)");
    fltr.push_back("OSM (*.dla *.txt *.dat)");
    fltr.push_back("DLCA (*.dla *.txt *.dat)");
    fltr.push_back("DLCA2 (*.dla *.txt *.dat)");
//    fltr.push_back("MxDLA (*.dla *.txt *.dat)");
    QString filters = fltr[0];
    for (size_t i = 1; i < fltr.size(); ++i) {
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

    int32_t idx = -1;
    for (size_t i = 0; i < fltr.size(); ++i) {
        if (fltr[i] == fileDialog.selectedNameFilter()) {
            idx = int32_t(i);
        }
    }
    if (m_glStructure->gen) {
        delete m_glStructure->gen;
        m_glStructure->gen = nullptr;
    }
    if (!fileName.isEmpty()) {
        switch (idx) {
            case 0 : // pH < 7 - MultiDLA
                m_currentType = gen_mdla;
                m_glStructure->gen = new MultiDLA(this);
                break;
            case 1 : // OSM
                m_currentType = gen_osm;
                m_glStructure->gen = new OSM(this);
                break;
            case 2 : // DLCA
                m_currentType = gen_dlca;
                m_glStructure->gen = new DLCA(this);
                break;
            case 3 : // DLCA2
                m_currentType = gen_dlca2;
                m_glStructure->gen = new DLCA2(this);
                break;
            case 4 : // MultixDLA
                m_currentType = gen_mxdla;
                m_glStructure->gen = new MxDLA(this);
                break;
            default :
                statusBar()->showMessage(tr("Some error!"), 5000);
                return;
        }
        if (fileName.endsWith(".dla")) {
            std::string fName = fileName.toStdString();
            m_glStructure->gen->load(fName, txt_dla);
        }
        if (fileName.endsWith(".txt")) {
            std::string fName = fileName.toStdString();
            m_glStructure->gen->load(fName, txt_txt);
        }
        if (fileName.endsWith(".dat")) {
            std::string fName = fileName.toStdString();
            m_glStructure->gen->load(fName, txt_dat);
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
               "<p>Golubchikov Mihail</p><p> </p><p>Changelog:</p>").arg(QString::fromStdString(program_version)));
    label1->setWordWrap(true);
    gLayout->addWidget(label1, 0, 1, 2, 1);
    //create changelog
    QStringList versions;
    versions.push_back(tr("<p>1.2.0 - Refactoring and fix bugs</p>"));
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
    for (const auto& str : versions) {
        vers += str;
    }
    QTextEdit* changelog = new QTextEdit(vers);
    changelog->setReadOnly(true);
    //
    gLayout->addWidget(changelog, 2, 1);
    gLayout->addWidget(new QLabel(tr("<p>RS-Pharmcenter (c) 2017</p>")), 3, 1);
    QLabel* gitLabel = new QLabel;
    gitLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    gitLabel->setOpenExternalLinks(true);
    gitLabel->setText("<a href=\"https://github.com/rue-ryuzaki/AeroGen\">GitHub binary repository</a>");
    gLayout->addWidget(gitLabel, 4, 1);
    aboutDialog->setLayout(gLayout);
    aboutDialog->exec();

    delete aboutDialog;
}

void MainWindow::getColor()
{
    QColor colorGL;
    colorGL.setRgbF(m_glStructure->colors[0], m_glStructure->colors[1],
                    m_glStructure->colors[2], m_glStructure->colors[3]);

    QColor color = QColorDialog::getColor(colorGL, this);
    if (color.isValid()) {
        m_glStructure->colors[0] = float(color.redF());
        m_glStructure->colors[1] = float(color.greenF());
        m_glStructure->colors[2] = float(color.blueF());
        m_glStructure->colors[3] = float(color.alphaF());
        m_colorButton.setStyleSheet(tr("* { background-color: rgb(%1, %2, %3); }")
            .arg(color.red()).arg(color.green())
            .arg(color.blue()));
        restructGL();
    }
}

void MainWindow::start()
{
    if (m_glStructure->gen && m_glStructure->gen->run) {
        statusBar()->showMessage(tr("Calculation already runned"), 5000);
        return;
    }
    m_progressBar->setValue(0);
    m_surfaceArea.clear();
    m_densityAero.clear();
    m_porosityAero.clear();
    m_sizesEdit->setStyleSheet("");
    bool ok = false;
    QString text = m_sizesEdit->text();
    QRegExp regExp("^([1-9][0-9]*) *x *([1-9][0-9]*) *x *([1-9][0-9]*)$");
    Sizes size;
    uint32_t sizemax = 0;
    if (regExp.exactMatch(text)) {
        uint32_t x = uint32_t(regExp.cap(1).toInt());
        uint32_t y = uint32_t(regExp.cap(2).toInt());
        uint32_t z = uint32_t(regExp.cap(3).toInt());
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
            m_sizesEdit->setStyleSheet("* { background-color: rgb(255, 50, 0); }");
            statusBar()->showMessage(tr("Sizes interval from 50 to 500 nm!"));
            QMessageBox::warning(this, tr("Warning"), tr("Sizes interval from 50 to 500 nm!"));
            return;
        }
    }
    if (!ok) {
        m_sizesEdit->setStyleSheet("* { background-color: rgb(255, 50, 0); }");
        statusBar()->showMessage(tr("Error parsing sizes!"));
        QMessageBox::warning(this, tr("Warning"), tr("Error parsing sizes!"));
        return;
    }
    m_parameter.method   = m_structureType->currentIndex();
    m_parameter.sizes    = m_sizesEdit->text().toStdString();
    m_parameter.porosity = m_poreDLA->value();
    m_parameter.init     = m_initDLA->value();
    m_parameter.step     = m_stepDLA->value();
    m_parameter.hit      = m_hitDLA->value();
    m_parameter.cluster  = m_clusterDLA->value();
    m_parameter.cellSize = m_cellSize->value();
    m_parameter.isToroid = (m_boundaryType->currentIndex() == 0);

    m_glStructure->setCamera(float(sizemax));
    if (m_glStructure->gen) {
        delete m_glStructure->gen;
        m_glStructure->gen = nullptr;
    }
    switch (m_structureType->currentIndex()) {
        case 0 : // pH < 7 - MultiDLA
            std::cout << "Organic? (MultiDLA)" << std::endl;
            m_currentType = gen_mdla;
            m_glStructure->gen = new MultiDLA(this);
            break;
        case 1 : // pH > 7 - Spheres Inorganic
            std::cout << "Inorganic? (Spheres)" << std::endl;;
            m_currentType = gen_osm;
            m_glStructure->gen = new OSM(this);
            break;
        case 2 : // DLCA
            std::cout << "(Clusters DLCA)" << std::endl;
            m_currentType = gen_dlca;
            m_glStructure->gen = new DLCA(this);
            break;
        case 3 : // DLCA2
            std::cout << "(Clusters DLCA2)" << std::endl;
            m_currentType = gen_dlca2;
            m_glStructure->gen = new DLCA2(this);
            break;
        case 4 : // MultiXDLA
            std::cout << "Organic? (MxDLA)" << std::endl;
            m_currentType = gen_mxdla;
            m_glStructure->gen = new MxDLA(this);
            break;
        default :
            std::cout << "Undefined method!\n";
            QMessageBox::warning(this, tr("Warning"), tr("Undefined method"));
            return;
    }
    updateGenerator();
    m_glStructure->gen->run = true;
    m_glStructure->gen->cancel(false);

    std::thread t(threadGen, size, m_parameter.getRunParams());
    t.detach();

    clearLayout(m_genLayout1);
    createLayout1();
    retranslate();
    m_generateButton->setEnabled(false);

    while (true) {
        if (m_glStructure->gen) {
            while (true) {
                if (m_glStructure->gen->field()) {
                    m_sizesLabel2->setText(tr("Sizes (in nm):") + tr("%1x%2x%3")
                        .arg(m_glStructure->gen->field()->sizes().x)
                        .arg(m_glStructure->gen->field()->sizes().y)
                        .arg(m_glStructure->gen->field()->sizes().z));
                    m_genLayout1->addRow(m_sizesLabel2);
                    break;
                }
            }
            break;
        }
    }
    m_genLayout1->addRow(m_cellSizeLabel2);
    m_genLayout1->addRow(m_poreLabel2);
    if (m_parameter.method == 0 || m_parameter.method == 3) {
        m_genLayout1->addRow(m_initLabel2);
        m_genLayout1->addRow(m_stepLabel2);
        m_genLayout1->addRow(m_hitLabel2);
    }
    m_genLayout1->addRow(m_clusterLabel2);
    m_genLayout1->addRow(m_boundaryLabel2);
    m_genLayout1->addRow(m_statusLabel, m_progressBar);
    m_genLayout1->addRow(m_generateButton);
    m_genLayout1->addRow(m_stopButton);
    m_panelBox->repaint();
}

void MainWindow::stop()
{
    m_generateButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    if (m_glStructure->gen) {
        m_glStructure->gen->cancel(true);
    }
}

void MainWindow::stopDistr()
{
    m_distributor->cancel();
    m_waitDialog->setWindowTitle(tr("Cancelling..."));
    statusBar()->showMessage(tr("Pore distribution calculating cancelling..."), 5000);
}

void MainWindow::closeWaitDialog()
{
    m_waitDialog->close();
}

void MainWindow::distrFinished()
{
    std::vector<distrib> distr = m_distributor->distribution();
    QDialog* distrDialog = new QDialog(this);
#ifdef QWT_DEFINED
    distrDialog->setFixedSize(700, 300);
#else
    distrDialog->setFixedSize(350, 300);
#endif // QWT_DEFINED
    distrDialog->setWindowTitle(tr("Pore distribution"));

    QGridLayout* layout = new QGridLayout;
    //layout->addRow(new QLabel(tr("Pore distribution don't work yet")));
    layout->addWidget(new QLabel(tr("Testing version")), 0, 0);
    // table!
    QTableWidget* table = new QTableWidget;
    table->setRowCount(int32_t(distr.size()));
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({ tr("Pore size, nm"), tr("Volume, nm3"), tr("Percentage, %") });
#ifdef QWT_DEFINED
    QwtPlot* plot = new QwtPlot;
    plot->setAxisTitle(QwtPlot::yLeft, tr("Percentage, %"));
    plot->setAxisTitle(QwtPlot::xBottom, tr("Pore size, nm"));
    plot->setFixedWidth(350);
    QwtPlotGrid* grid = new QwtPlotGrid;
    grid->setMajorPen(QPen(Qt::gray, 1));
    grid->attach(plot);
    QwtPlotCurve* curve = new QwtPlotCurve;
    curve->setPen(Qt::blue, 2);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::yellow),
                                      QPen(Qt::red, 2), QSize(4, 4));
    curve->setSymbol(symbol);
    QPolygonF points;
#endif // QWT_DEFINED
    double sum = 0.0;
    if (!distr.empty()) {
        double prevVol = distr.back().vol;
        distr.back().count = prevVol / VfromR(distr.back().r);
        sum += distr.back().count;
        for (int32_t i = int32_t(distr.size()) - 2; i >= 0; --i) {
            distr[i].count = (distr[i].vol - prevVol) / VfromR(distr[size_t(i)].r);
            sum += distr[size_t(i)].count;
            prevVol = distr[size_t(i)].vol;
        }
        for (size_t i = 0; i < distr.size(); ++i) {
            table->setItem(int32_t(i), 0, new QTableWidgetItem(QString::number(2.0 * distr[i].r)));
            table->setItem(int32_t(i), 1, new QTableWidgetItem(QString::number(distr[i].vol)));
            double perc = 100.0 * distr[i].count / sum;
            table->setItem(int32_t(i), 2, new QTableWidgetItem(QString::number(perc)));
#ifdef QWT_DEFINED
            points << QPointF(2.0 * distr[i].r, perc);
#endif // QWT_DEFINED
        }
    }
    table->resizeColumnsToContents();
    layout->addWidget(table, 1, 0);
#ifdef QWT_DEFINED
    curve->setSamples(points);
    curve->attach(plot);
    layout->addWidget(plot, 0, 1, 2, 1);
#endif // QWT_DEFINED
    // end
    distrDialog->setLayout(layout);
    distrDialog->exec();

    delete distrDialog;
}

void MainWindow::propCalc()
{
    m_surfaceArea.clear();
    m_densityAero.clear();
    m_porosityAero.clear();
    double sqrArea = 0.0;
    double denAero = 0.0;
    double porosity = 0.0;

    if (!m_glStructure->gen || !m_glStructure->gen->finished()) {
        statusBar()->showMessage(tr("Structure not ready yet!"));
        return;
    }
    sqrArea = m_glStructure->gen->surfaceArea(m_density.value(),
                                              uint32_t(m_monteCarloAero.value()));

    m_glStructure->gen->density(m_density.value(), denAero, porosity);
    m_surfaceArea.setText(tr(dtos(sqrArea, 2, true).c_str()));
    m_densityAero.setText(tr(dtos(denAero, 2, true).c_str()));
    m_porosityAero.setText(tr(dtos((porosity) * 100, 2, true).c_str()));
}

void MainWindow::distCalc()
{
    double dFrom = double(m_distFrom.value());
    double dTo   = double(m_distTo.value());
    double dStep = double(m_distStep.value());
    bool isToroid = (m_distBoundary.currentIndex() == 0);
    if (dFrom > dTo) {
        statusBar()->showMessage(tr("dFrom > dTo!"));
        return;
    }

    //if (dTo > cellSize->value() * glStructure->multiDLA->GetField()->GetSize().GetCoord(0)) {
    //    statusBar()->showMessage(tr("dFrom > dTo!"));
    //    return;
    //}
    if (!m_glStructure->gen || !m_glStructure->gen->finished()) {
        statusBar()->showMessage(tr("Structure not ready yet!"));
        return;
    }
    std::thread t(threadRunDistr, m_cellSize->value(), dFrom, dTo, dStep, isToroid);
    t.detach();

    m_waitDialog->setWindowTitle(tr("Wait"));
    m_waitDialog->exec();
}

void MainWindow::propLoad()
{
    statusBar()->showMessage(tr("Not available yet!"));
    return;
}

void MainWindow::setProgress(int value)
{
    m_progressBar->setValue(value);
    if (value >= m_progressBar->maximum()) {
        m_generateButton->setEnabled(true);
        m_stopButton->setEnabled(false);
    }
}

void MainWindow::settings()
{
    int res = SettingsForm::dialog(tr("Settings"), m_setParams, m_glStructure->params);
    if (res == QDialogButtonBox::Yes) {
        // save settings && update ?
        if (m_drawGL.isChecked()) {
            m_glStructure->restruct(false);
        }
    } else {
        // do nothing
    }
}

#ifdef QT_NETWORK_LIB
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

void MainWindow::updateUpdater()
{
    if (!DownloadUpdater()) {
        QMessageBox::warning(this, tr("Update updater"), tr("Update error! updater not found!"));
    }
}
#endif // QT_NETWORK_LIB

void MainWindow::feedback()
{
    QDialog* feedbackDialog = new QDialog(this);
    feedbackDialog->setFixedSize(450, 350);
    feedbackDialog->setWindowTitle(tr("Feedback"));

    QFormLayout* layout = new QFormLayout;
    m_feedbackProblem     = new QLineEdit;
    m_feedbackName        = new QLineEdit;
    m_feedbackDescription = new QTextEdit;
    layout->addRow(new QLabel(tr("Problem:")), m_feedbackProblem);
    layout->addRow(new QLabel(tr("Your name:")), m_feedbackName);
    layout->addRow(new QLabel(tr("Description:")), m_feedbackDescription);
    QPushButton* fbSend = new QPushButton(tr("Send"));
    connect(fbSend, SIGNAL(clicked()), this, SLOT(feedbackSend()));
    layout->addRow(fbSend);
    feedbackDialog->setLayout(layout);
    feedbackDialog->exec();
    delete m_feedbackProblem;
    delete m_feedbackName;
    delete m_feedbackDescription;
    delete feedbackDialog;
}

void MainWindow::feedbackSend()
{
    if (m_feedbackProblem->text().isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Problem empty"));
        return;
    } else if (m_feedbackName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Name empty"));
        return;
    } else if (m_feedbackDescription->toPlainText().isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Description empty"));
        return;
    }
    QMessageBox::warning(this, tr("Feedback"), tr("Not supported."));
}

void MainWindow::changeDrawGL()
{
    m_glArea.setEnabled(m_drawGL.isChecked());
    m_glStructure->drawGL = m_drawGL.isChecked();
    m_showAxes.setEnabled(m_drawGL.isChecked());
    m_showBorders.setEnabled(m_drawGL.isChecked());
    restructGL();
}

void MainWindow::restructGL()
{
    if (m_drawGL.isChecked()) {
        m_glStructure->restruct(true);
    }
}

void MainWindow::axesGL()
{
    m_glStructure->showAxes = m_showAxes.isChecked();
    if (m_drawGL.isChecked()) {
        m_glStructure->restruct(false);
    }
}

void MainWindow::borderGL()
{
    m_glStructure->showBorders = m_showBorders.isChecked();
    if (m_drawGL.isChecked()) {
        m_glStructure->restruct(false);
    }
}

void MainWindow::switchShaders()
{
    int32_t shader = -1;
    for (int32_t i = 0; i < m_effectActions.size(); ++i) {
        if (QObject::sender() == m_effectActions[i].first) {
            shader = i;
            break;
        }
    }
    if (shader == -1) {
        return;
    }
    m_glStructure->needInit = shader;
    if (shader != 0 && !m_glStructure->supportShaders()) {
        if (m_glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shader = 0;
        }
    }
    m_glStructure->enableShader(shader);
    shader = m_glStructure->shadersStatus();
    for (int32_t i = 0; i < m_effectActions.size(); ++i) {
        m_effectActions[i].first->setChecked(shader == i);
    }
    restructGL();
}

void MainWindow::switchToLanguage()
{
    int32_t language = -1;
    for (int32_t i = 0; i < m_languageActions.size(); ++i) {
        if (QObject::sender() == m_languageActions[i].first) {
            language = i;
            break;
        }
    }
    bool wasDialog = false;
    if (QObject::sender() == &m_buttonEn) {
        wasDialog = true;
        language = 0;
    }
    if (QObject::sender() == &m_buttonRu) {
        wasDialog = true;
        language = 1;
    }
    if (language == -1) {
        return;
    }
    m_translator.load(m_locales[language]);
    for (int32_t i = 0; i < m_languageActions.size(); ++i) {
        m_languageActions[i].first->setChecked(language == i);
    }
    retranslate();
    if (wasDialog) {
        m_langDialog.accept();
    }
}

void MainWindow::changeType(int value)
{
    bool enabled = (value == 0 || value == 3);
    m_initDLA->setEnabled(enabled);
    m_stepDLA->setEnabled(enabled);
    m_hitDLA->setEnabled(enabled);
    m_initLabel->setEnabled(enabled);
    m_stepLabel->setEnabled(enabled);
    m_hitLabel->setEnabled(enabled);
}

void MainWindow::openGen()
{
    clearLayout(m_genLayout1);
    createLayout2();
    retranslate();
    m_genLayout1->addRow(m_methodLabel, m_structureType);
    m_genLayout1->addRow(m_sizesLabel, m_sizesEdit);
    m_genLayout1->addRow(m_cellSizeLabel, m_cellSize);
    m_genLayout1->addRow(m_poreLabel, m_poreDLA);
    m_genLayout1->addRow(m_initLabel, m_initDLA);
    m_genLayout1->addRow(m_stepLabel, m_stepDLA);
    m_genLayout1->addRow(m_hitLabel, m_hitDLA);
    m_genLayout1->addRow(m_clusterLabel, m_clusterDLA);
    m_genLayout1->addRow(m_boundaryTypeLabel, m_boundaryType);
    m_genLayout1->addRow(m_startButton);
    m_panelBox->repaint();
    //m_panelBox.setLayout(genLayout);
}

void MainWindow::closeGen()
{
    //generateDialog->close();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    QMessageBox::StandardButton resBtn =
            QMessageBox::question(this, tr("Exit"), tr("Are you sure?"),
                                  QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        saveSettings();
        e->accept();
    } else {
        e->ignore();
    }
}

void MainWindow::selectLanguage()
{
    m_langDialog.setFixedSize(170, 200);
    
    QFormLayout* layout = new QFormLayout;
    layout->addRow(new QLabel(tr("Select language:")));
    int32_t w = 150;
    int32_t h = 80;

    m_buttonRu.setFlat(true);
    m_buttonRu.setStyleSheet("border-image: url(:/ru.png) 0 0 0 0 stretch stretch;");
    m_buttonRu.setFixedSize(w, h);
    m_buttonRu.setAutoFillBackground(true);
    connect(&m_buttonRu, SIGNAL(clicked()), this, SLOT(switchToLanguage()));
    layout->addRow(&m_buttonRu);

    m_buttonEn.setFlat(true);
    m_buttonEn.setStyleSheet("border-image: url(:/en.png) 0 0 0 0 stretch stretch;");
    m_buttonEn.setFixedSize(w, h);
    m_buttonEn.setAutoFillBackground(true);
    connect(&m_buttonEn, SIGNAL(clicked()), this, SLOT(switchToLanguage()));
    layout->addRow(&m_buttonEn);

    m_langDialog.setLayout(layout);
    m_langDialog.exec();
}

void MainWindow::saveSettings()
{
    int32_t language = -1;
    for (int32_t i = 0; i < m_languageActions.size(); ++i) {
        if (m_languageActions[i].first->isChecked()) {
            language = i;
            break;
        }
    }
    if (language == -1) {
        std::cerr << "Error language" << std::endl;
        return;
    }
    QVariantMap map;
    map.insert("language", language);
    QVariantList color;
    color << m_glStructure->colors[0];
    color << m_glStructure->colors[1];
    color << m_glStructure->colors[2];
    map.insert("color", color);
    map.insert("shader", m_glStructure->shadersStatus());
    map.insert("drawGL", m_glStructure->drawGL);
    map.insert("showAxes", m_glStructure->showAxes);
    map.insert("showBorders", m_glStructure->showBorders);
    QJsonObject object = QJsonObject::fromVariantMap(map);
    QJsonDocument document;
    document.setObject(object);
    QFile file(settingsFile.c_str());
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write(document.toJson());
    file.close();
}

bool MainWindow::loadSettings()
{
    if (!fileExists(settingsFile.c_str())) {
        return false;
    }
    QFile file(settingsFile.c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Error open file" << std::endl;
        return false;
    }
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();
    if (parseError.error != QJsonParseError::NoError) {
        std::cout << QString("Error `%1` json parse file %2").arg(parseError.error)
                     .arg(settingsFile.c_str()).toStdString() << std::endl;
        return false;
    }
    QJsonObject object = document.object();
    QVariantMap map = object.toVariantMap();
    int32_t language = map["language"].toInt();
    if (language < 0 || language >= m_languageActions.size()) {
        language = 0;
    }
    int32_t shader = map["shader"].toInt();
    if (shader != 0 && !m_glStructure->isSupported(shader - 1)) {
        shader = 0;
    }
    GLfloat colors[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    QVariantList color = map["color"].toList();
    for (int i = 0; i < 3; ++i) {
        if (i < color.size()) {
            colors[i] = color[i].toFloat();
            if (colors[i] < 0.0 || colors[i] > 1.0) {
                colors[i] = 0.0;
            }
        }
        m_glStructure->colors[i] = colors[i];
    }
    m_colorButton.setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
                                .arg(int(m_glStructure->colors[0] * 255))
                                .arg(int(m_glStructure->colors[1] * 255))
                                .arg(int(m_glStructure->colors[2] * 255)));
    defaultShaders();
    m_glStructure->needInit = shader;
    if (shader != 0 && !m_glStructure->supportShaders()) {
        if (m_glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shader = 0;
        }
    }
    m_glStructure->enableShader(shader);
    for (int32_t i = 0; i < m_effectActions.size(); ++i) {
        m_effectActions[i].first->setChecked(shader == i);
    }
    m_glStructure->drawGL = map["drawGL"].toBool();
    m_glStructure->showAxes = map["showAxes"].toBool();
    m_glStructure->showBorders = map["showBorders"].toBool();
    m_drawGL.setChecked(m_glStructure->drawGL);
    m_showAxes.setChecked(m_glStructure->showAxes);
    m_showBorders.setChecked(m_glStructure->showBorders);
    restructGL();

    m_translator.load(m_locales[language]);
    for (int32_t i = 0; i < m_languageActions.size(); ++i) {
        m_languageActions[i].first->setChecked(language == i);
    }
    return true;
}

void MainWindow::loadDefault()
{
    defaultShaders();
    // structure color
    m_glStructure->colors[0] = 0.0f;
    m_glStructure->colors[1] = 1.0f;
    m_glStructure->colors[2] = 0.0f;
    m_glStructure->colors[3] = 1.0f;
    m_colorButton.setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
                                .arg(int(m_glStructure->colors[0] * 255))
                                .arg(int(m_glStructure->colors[1] * 255))
                                .arg(int(m_glStructure->colors[2] * 255)));
    // shaders
    int32_t shader = 0; // default disabled
    m_glStructure->needInit = shader;
    if (shader != 0 && !m_glStructure->supportShaders()) {
        if (m_glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shader = 0;
        }
    }
    m_glStructure->enableShader(shader);
    for (int32_t i = 0; i < m_effectActions.size(); ++i) {
        m_effectActions[i].first->setChecked(shader == i);
    }
    restructGL();
    // language
    selectLanguage();
}

void MainWindow::defaultShaders()
{
    // base
    m_glStructure->params.specPower    = 30.0f;
    m_glStructure->params.specColor[0] = 0.7f;
    m_glStructure->params.specColor[1] = 0.7f;
    m_glStructure->params.specColor[2] = 0.0f;
    m_glStructure->params.specColor[3] = 1.0f;
    // shaders
    // wrap
    m_glStructure->params.wrap_factor = 0.5f;
    // iso-ward
    m_glStructure->params.iso_ward_k = 10.0f;
    // oren
    m_glStructure->params.oren_a = 1.0f;
    m_glStructure->params.oren_b = 0.45f;
    // minnaert
    m_glStructure->params.minnaert_k = 0.8f;
    // cartoon
    m_glStructure->params.cartoon_edgePower = 3.0f;
    // gooch
    m_glStructure->params.gooch_diffuseWarm = 0.45f;
    m_glStructure->params.gooch_diffuseCool = 0.45f;
    // rim
    m_glStructure->params.rim_rimPower = 8.0f;
    m_glStructure->params.rim_bias     = 0.3f;
    // subsurface
    m_glStructure->params.subsurface_matThickness = 0.56f;
    m_glStructure->params.subsurface_rimScalar    = 1.38f;
    // bidirectional
    m_glStructure->params.bidirect_color2[0] = 0.5f;
    m_glStructure->params.bidirect_color2[1] = 0.5f;
    m_glStructure->params.bidirect_color2[2] = 0.5f;
    m_glStructure->params.bidirect_color2[3] = 1.0f;
    // hemispheric
    m_glStructure->params.hemispheric_color2[0] = 0.5f;
    m_glStructure->params.hemispheric_color2[1] = 0.5f;
    m_glStructure->params.hemispheric_color2[2] = 0.5f;
    m_glStructure->params.hemispheric_color2[3] = 1.0f;
    // trilight
    m_glStructure->params.trilight_color1[0] = 0.25f;
    m_glStructure->params.trilight_color1[1] = 0.25f;
    m_glStructure->params.trilight_color1[2] = 0.25f;
    m_glStructure->params.trilight_color1[3] = 1.0f;
    m_glStructure->params.trilight_color2[0] = 0.75f;
    m_glStructure->params.trilight_color2[1] = 0.75f;
    m_glStructure->params.trilight_color2[2] = 0.75f;
    m_glStructure->params.trilight_color2[3] = 1.0f;
    // strauss
    m_glStructure->params.strauss_metal  = 0.7f;
    m_glStructure->params.strauss_smooth = 0.7f;
    m_glStructure->params.strauss_transp = 0.1f;
}

#ifdef QT_NETWORK_LIB
void MainWindow::updated()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Update AeroGen"),
            tr("Update available!\nWould you like to update application now?"),
            QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (fileExists(updaterFile.c_str())) {
            QMessageBox::warning(this, tr("Update AeroGen"), tr("Application will be closed!"));

            std::thread t(startUpdater);
            t.detach();
            QThread::sleep(1);
            exit(0);
        } else {
            QMessageBox::warning(this, tr("Update AeroGen"), tr("Update canceled! updater not found!"));
        }
        //QApplication::quit();
    } else {
        statusBar()->showMessage(tr("Update canceled!"));
    }
}
#endif // QT_NETWORK_LIB

void MainWindow::retranslate()
{
    // menu
    m_newAct.setText(tr("&New"));
    m_newAct.setStatusTip(tr("Create a new file"));
    m_openAct.setText(tr("&Open..."));
    m_openAct.setStatusTip(tr("Open an existing file"));
    m_saveAct.setText(tr("&Save"));
    m_saveAct.setStatusTip(tr("Save the document to disk"));
    m_saveAsAct.setText(tr("Save &As..."));
    m_saveAsAct.setStatusTip(tr("Save the document under a new name"));
    m_saveImageAct.setText(tr("Save &Image"));
    m_saveImageAct.setStatusTip(tr("Save the image to disk"));
    m_exportDLAAct.setText(tr("&Save structure"));
    m_exportDLAAct.setStatusTip(tr("Export the structure under a new name"));
    m_importDLAAct.setText(tr("&Load structure"));
    m_importDLAAct.setStatusTip(tr("Import the structure from file"));
    m_exitAct.setText(tr("E&xit"));
    m_updateAct.setText(tr("Check for &updates"));
    m_aboutAct.setText(tr("&About"));
    m_aboutQtAct.setText(tr("About &Qt"));
    m_feedbackAct.setText(tr("&Feedback"));

    m_fileMenu->setTitle(tr("&File"));
    m_settingsMenu->setTitle(tr("&Settings"));
    m_languageMenu->setTitle(tr("&Language"));
    m_effectsMenu->setTitle(tr("&Effects"));
    for (QPair<QAction*, QString>& effect : m_effectActions) {
        effect.first->setText(effect.second);
    }
    m_settingsAct.setText(tr("Settings"));
    m_helpMenu->setTitle(tr("&Help"));

    // panels
    m_currMethodLabel.setText(tr("Generation method"));
    m_drawGL.setText(tr("Draw structure"));
    m_showAxes.setText(tr("Show axes"));
    m_showBorders.setText(tr("Show borders"));
    m_colorLabel.setText(tr("Color:"));
    m_colorButton.setText(tr("Select"));
    m_statusLabel->setText(tr("Status:"));

    m_panelBox->setTitle(tr("Generation"));
    m_methodLabel->setText(tr("Method:"));
    m_sizesLabel->setText(tr("Sizes (in nm):"));
    m_cellSizeLabel->setText(tr("Line cell size, nm:"));
    m_poreLabel->setText(tr("Porosity, %:"));
    m_initLabel->setText(tr("Init count:"));
    m_stepLabel->setText(tr("Step:"));
    m_hitLabel->setText(tr("Hit:"));
    m_clusterLabel->setText(tr("Cluster:"));
    m_boundaryTypeLabel->setText(tr("Boundary conditions:"));
    m_startButton->setText(tr("Generate"));
    if (m_glStructure->gen) {
        if (m_glStructure->gen->field()) {
            m_sizesLabel2->setText(tr("Sizes (in nm):")
                                   + tr("%1x%2x%3").arg(m_glStructure->gen->field()->sizes().x)
                                                   .arg(m_glStructure->gen->field()->sizes().y)
                                                   .arg(m_glStructure->gen->field()->sizes().z));
        } else {
            m_sizesLabel2->setText(tr("Sizes (in nm):") + tr("%1x%2x%3").arg(0).arg(0).arg(0));
        }
    } else {
        m_sizesLabel2->setText(tr("Sizes (in nm):") + tr("%1x%2x%3").arg(0).arg(0).arg(0));
    }
    m_cellSizeLabel2->setText(tr("Line cell size, nm:") + QString::number(m_parameter.cellSize));
    m_poreLabel2->setText(tr("Porosity, %:") + QString::number(m_parameter.porosity));
    m_initLabel2->setText(tr("Init count:") + QString::number(m_parameter.init));
    m_stepLabel2->setText(tr("Step:") + QString::number(m_parameter.step));
    m_hitLabel2->setText(tr("Hit:") + QString::number(m_parameter.hit));
    m_clusterLabel2->setText(tr("Cluster:") + QString::number(m_parameter.cluster));
    m_boundaryLabel2->setText(tr("Boundary conditions:") + (m_parameter.isToroid ? tr("Toroid") : tr("Close")));
    m_generateButton->setText(tr("Generate new"));
    m_stopButton->setText(tr("Stop"));

    m_cancelDistrButton.setText(tr("Stop"));

    m_propsBox.setTitle(tr("Properties"));
    m_densityLabel.setText(tr("Density, kg/m<sup>3</sup>:"));

    m_tabProps.setTabText(0, tr("Specific surface area"));
    m_surfaceAreaLabel.setText(tr("Specific surface area, m<sup>2</sup>/g:"));
    m_densityAeroLabel.setText(tr("Aerogel density, kg/m<sup>3</sup>:"));
    m_porosityAeroLabel.setText(tr("Current porosity, %:"));
    m_monteCarloAeroLabel.setText(tr("Monte Carlo iterations:"));
    m_propButton.setText(tr("Calculate"));

    m_tabProps.setTabText(1, tr("Distribution"));
    m_distFromLabel.setText(tr("from, nm:"));
    m_distToLabel.setText(tr("to, nm:"));
    m_distStepLabel.setText(tr("step, nm:"));
    m_distBoundaryLabel.setText(tr("Boundary conditions:"));
    m_distButton.setText(tr("Calculate"));

    updateGenerator();
}

bool MainWindow::event(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    return QMainWindow::event(event);
}

void MainWindow::updateGenerator()
{
    QString text;
    switch (m_currentType) {
        case gen_mdla :
            text = "MultiDLA";
            break;
        case gen_osm :
            text = "OSM";
            break;
        case gen_dlca :
            text = "DLCA";
            break;
        case gen_dlca2 :
            text = "DLCA2";
            break;
        case gen_mxdla :
            text = "MxDLA";
            break;
        default :
            m_currentMethod.setText(tr("Undefined"));
            return;
    }
    m_currentMethod.setText(text);
}

void MainWindow::threadGen(const Sizes& sizes, const RunParams& params)
{
    m_glStructure->gen->generate(sizes, params);
    m_glStructure->gen->run = false;
}

void MainWindow::threadRunDistr(double cellSize, double dFrom, double dTo, double dStep, bool isToroid)
{
    m_distributor->calculate(m_glStructure->gen->field(), cellSize, dFrom, dTo, dStep, isToroid);
}

void MainWindow::createActions()
{
    m_newAct.setShortcuts(QKeySequence::New);
    connect(&m_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    m_openAct.setShortcuts(QKeySequence::Open);
    connect(&m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_saveAct.setShortcuts(QKeySequence::Save);
    connect(&m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAct.setShortcuts(QKeySequence::SaveAs);
    connect(&m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    //m_saveImageAct.setShortcuts(QKeySequence::Save);setShortcut(tr("Ctrl+G"));
    connect(&m_saveImageAct, SIGNAL(triggered()), this, SLOT(saveImage()));

    //m_exportingAct.setDisabled(true);//->isEnabled() = false;
    //m_exportingAct.setShortcuts(QKeySequence::SaveAs);setShortcut(tr("Ctrl+G"));
    connect(&m_exportDLAAct, SIGNAL(triggered()), this, SLOT(exportDLA()));
    connect(&m_importDLAAct, SIGNAL(triggered()), this, SLOT(importDLA()));

    m_exitAct.setShortcuts(QKeySequence::Quit);
    connect(&m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

#ifdef QT_NETWORK_LIB
    connect(&m_updateAct, SIGNAL(triggered()), this, SLOT(updates()));
#else
    m_updateAct.setEnabled(false);
#endif // QT_NETWORK_LIB
    connect(&m_settingsAct, SIGNAL(triggered()), this, SLOT(settings()));
    connect(&m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(&m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(&m_feedbackAct, SIGNAL(triggered()), this, SLOT(feedback()));
}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu("");
    m_fileMenu->addAction(&m_newAct);
    m_fileMenu->addAction(&m_openAct);
    m_fileMenu->addAction(&m_saveAct);
    m_fileMenu->addAction(&m_saveAsAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(&m_saveImageAct);
    m_fileMenu->addAction(&m_exportDLAAct);
    m_fileMenu->addAction(&m_importDLAAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(&m_exitAct);
    
    m_settingsMenu = menuBar()->addMenu("");
    m_languageMenu = m_settingsMenu->addMenu("");
    m_effectsMenu = m_settingsMenu->addMenu("");
    m_settingsMenu->addAction(&m_settingsAct);
    
    m_helpMenu = menuBar()->addMenu("");
    m_helpMenu->addAction(&m_updateAct);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(&m_aboutAct);
    m_helpMenu->addAction(&m_aboutQtAct);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(&m_feedbackAct);
    
    createSettingsMenu();
}

void MainWindow::createSettingsMenu()
{
    // language
    m_languageActions.push_back({ &m_languageEnAct, { QIcon(":/en.png"), ":/lang/aerogen_en.qm" } });
    m_languageActions.push_back({ &m_languageRuAct, { QIcon(":/ru.png"), ":/lang/aerogen_ru.qm" } });

    for (int32_t i = 0; i < m_languageActions.size(); ++i) {
        m_locales.push_back(m_languageActions[i].second.locale);
        QAction* action = m_languageActions[i].first;
        action->setIcon(m_languageActions[i].second.icon);
        action->setCheckable(true);
        connect(action, SIGNAL(triggered()), this, SLOT(switchToLanguage()));
        m_languageMenu->addAction(action);
        if (i == 0) {
            action->setChecked(true);
        }
    }
    // shaders
    m_effectActions.push_back({ &m_effectsDisableAct, tr("Disable") });
    m_effectActions.push_back({ &m_effectsLambertAct, tr("Lambert") });
    m_effectActions.push_back({ &m_effectsWrapAroundAct, tr("Wrap-around") });
    m_effectActions.push_back({ &m_effectsPhongAct, tr("Phong") });
    m_effectActions.push_back({ &m_effectsBlinnAct, tr("Blinn") });
    m_effectActions.push_back({ &m_effectsIsotropWardAct, tr("Isotropic Ward") });
    m_effectActions.push_back({ &m_effectsOrenNayarAct, tr("Oren-Nayar") });
    m_effectActions.push_back({ &m_effectsCookTorranceAct, tr("Cook-Torrance") });
    m_effectActions.push_back({ &m_effectsAnisotropAct, tr("Anisotropic") });
    m_effectActions.push_back({ &m_effectsAnisotropWardAct, tr("Anisotropic Ward") });
    m_effectActions.push_back({ &m_effectsMinnaertAct, tr("Minnaert") });
    m_effectActions.push_back({ &m_effectsAshikhminShirleyAct, tr("Ashikhmin-Shirley") });
    m_effectActions.push_back({ &m_effectsCartoonAct, tr("Cartoon") });
    m_effectActions.push_back({ &m_effectsGoochAct, tr("Gooch") });
    m_effectActions.push_back({ &m_effectsRimAct, tr("Rim") });
    m_effectActions.push_back({ &m_effectsSubsurfaceAct, tr("Subsurface") });
    m_effectActions.push_back({ &m_effectsBidirectionalAct, tr("Bidirectional") });
    m_effectActions.push_back({ &m_effectsHemisphericAct, tr("Hemispheric") });
    m_effectActions.push_back({ &m_effectsTrilightAct, tr("Trilight") });
    m_effectActions.push_back({ &m_effectsLommelSeeligerAct, tr("Lommel-Seeliger") });
    m_effectActions.push_back({ &m_effectsStraussAct, tr("Strauss") });

    for (int32_t i = 0; i < m_effectActions.size(); ++i) {
        QAction* action = m_effectActions[i].first;
        action->setCheckable(true);
        connect(action, SIGNAL(triggered()), this, SLOT(switchShaders()));
        m_effectsMenu->addAction(action);
        if (i == 0) {
            action->setChecked(true);
            m_effectsMenu->addSeparator();
        } else if (!m_glStructure->isSupported(i - 1)) {
            action->setEnabled(false);
        }
    }
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
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();
    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Error `%1` json parse file %2")
                             .arg(parseError.error)
                             .arg(fileName));
        return;
    }
    m_curFile = fileName;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QJsonObject object = document.object();
    QVariantMap map = object.toVariantMap();
    m_structureType->setCurrentIndex(map["method"].toInt());
    m_sizesEdit->setText  (map["sizes"].toString());
    m_poreDLA->setValue   (map["porosity"].toDouble());
    m_initDLA->setValue   (map["init"].toInt());
    m_stepDLA->setValue   (map["step"].toInt());
    m_hitDLA->setValue    (map["hit"].toInt());
    m_clusterDLA->setValue(map["cluster"].toInt());
    m_cellSize->setValue  (map["cellSize"].toDouble());
    m_boundaryType->setCurrentIndex(map["isToroid"].toBool() ? 0 : 1);
    m_parameter.method   = m_structureType->currentIndex();
    m_parameter.sizes    = m_sizesEdit->text().toStdString();
    m_parameter.porosity = m_poreDLA->value();
    m_parameter.init     = m_initDLA->value();
    m_parameter.step     = m_stepDLA->value();
    m_parameter.hit      = m_hitDLA->value();
    m_parameter.cluster  = m_clusterDLA->value();
    m_parameter.cellSize = m_cellSize->value();
    m_parameter.isToroid = (m_boundaryType->currentIndex() == 0);
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("File loaded"), 5000);
}

void MainWindow::saveFile(const QString& fileName)
{
    QVariantMap map;
    map.insert("method", m_parameter.method);
    map.insert("sizes", QString::fromStdString(m_parameter.sizes));
    map.insert("porosity", m_parameter.porosity);
    map.insert("init", m_parameter.init);
    map.insert("step", m_stepDLA->value());
    map.insert("hit", m_hitDLA->value());
    map.insert("cluster", m_clusterDLA->value());
    map.insert("cellSize", m_parameter.cellSize);
    map.insert("isToroid", m_parameter.isToroid);
    QJsonObject object = QJsonObject::fromVariantMap(map);
    QJsonDocument document;
    document.setObject(object);
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }
    file.write(document.toJson());
    file.close();
    m_curFile = fileName;

    statusBar()->showMessage(tr("File saved"), 5000);
}

void MainWindow::createPanel()
{
    m_panelBox = new QGroupBox;
#ifdef _WIN32
// windows
    m_panelBox->setFixedSize(m_panelWidth, 270);
#elif defined __linux__
//linux
    m_panelBox->setFixedSize(m_panelWidth, 320);
#elif defined __APPLE__
// apple
    m_panelBox->setFixedSize(m_panelWidth, 320);
#endif
    // default parameters
    m_parameter.method   = 0;
    m_parameter.sizes    = "50x50x50";
    m_parameter.porosity = 95.0;
    m_parameter.init     = 10;
    m_parameter.step     = 1;
    m_parameter.hit      = 1;
    m_parameter.cluster  = 1;
    m_parameter.cellSize = 2.0;
    //
    createLayout1();
    createLayout2();

    m_genLayout1 = new QFormLayout;
    m_genLayout1->addRow(m_methodLabel, m_structureType);
    m_genLayout1->addRow(m_sizesLabel, m_sizesEdit);
    m_genLayout1->addRow(m_cellSizeLabel, m_cellSize);
    m_genLayout1->addRow(m_poreLabel, m_poreDLA);
    m_genLayout1->addRow(m_initLabel, m_initDLA);
    m_genLayout1->addRow(m_stepLabel, m_stepDLA);
    m_genLayout1->addRow(m_hitLabel, m_hitDLA);
    m_genLayout1->addRow(m_clusterLabel, m_clusterDLA);
    m_genLayout1->addRow(m_boundaryTypeLabel, m_boundaryType);
    m_genLayout1->addRow(m_startButton);

    m_panelBox->setLayout(m_genLayout1);
}

void MainWindow::createProps()
{
    m_propsBox.setFixedSize(m_panelWidth, 65);

    QFormLayout* layout = new QFormLayout;

    //loadButton = new QPushButton(""));
    //connect(loadButton, SIGNAL(clicked()), this, SLOT(propLoad()));
    //layout->addRow(loadButton);

    m_density.setMinimum(1);
    m_density.setMaximum(100000);
    m_density.setValue(2200);
    layout->addRow(&m_densityLabel, &m_density);

    m_propsBox.setLayout(layout);
}

void MainWindow::createTab()
{
#ifdef _WIN32 // windows
    m_tabProps.setFixedSize(m_panelWidth, 180);
#elif defined __linux__ //linux
    m_tabProps.setFixedSize(m_panelWidth, 200);
#elif defined __APPLE__ // apple
    m_tabProps.setFixedSize(m_panelWidth, 200);
#endif
    // Specific surface & Aerogel density
    {
        QFormLayout* layout = new QFormLayout;

        m_surfaceArea.setReadOnly(true);
        m_surfaceAreaLabel.setStyleSheet("* sup{margin-left: -0.3em;}");
        layout->addRow(&m_surfaceAreaLabel, &m_surfaceArea);

        m_densityAero.setReadOnly(true);
        layout->addRow(&m_densityAeroLabel, &m_densityAero);

        m_porosityAero.setReadOnly(true);
        layout->addRow(&m_porosityAeroLabel, &m_porosityAero);

        m_monteCarloAero.setMinimum(1000);
        m_monteCarloAero.setMaximum(1000000);
        m_monteCarloAero.setValue(5000);
        layout->addRow(&m_monteCarloAeroLabel, &m_monteCarloAero);

        connect(&m_propButton, SIGNAL(clicked()), this, SLOT(propCalc()));
        layout->addRow(&m_propButton);

        m_surfaceAreaTab.setLayout(layout);
        m_tabProps.addTab(&m_surfaceAreaTab, "");
    }
    // Pore distribution
    {
        QFormLayout* layout = new QFormLayout;

        m_distFrom.setMinimum(1);
        m_distFrom.setMaximum(100);
        m_distFrom.setValue(2);
        layout->addRow(&m_distFromLabel, &m_distFrom);

        m_distTo.setMinimum(2);
        m_distTo.setMaximum(100);
        m_distTo.setValue(10);
        layout->addRow(&m_distToLabel, &m_distTo);

        m_distStep.setMinimum(1);
        m_distStep.setMaximum(20);
        m_distStep.setValue(2);
        layout->addRow(&m_distStepLabel, &m_distStep);

        m_distBoundary.addItems({ tr("Toroid"), tr("Closed") });
        layout->addRow(&m_distBoundaryLabel, &m_distBoundary);

        connect(&m_distButton, SIGNAL(clicked()), this, SLOT(distCalc()));
        layout->addRow(&m_distButton);

        m_distributionTab.setLayout(layout);
        m_tabProps.addTab(&m_distributionTab, "");
    }
}

void MainWindow::createLayout1()
{
    // layout 1
    m_structureType = new QComboBox;
    //
    // pH < 7 - MultiDLA Organic
    // pH > 7 - Spheres Inorganic
    // Cluster DLCA ?
    //
    m_structureType->addItems({ tr("(MultiDLA)"), tr("(OSM)"), tr("(DLCA)"), tr("(DLCA2)") });
    //m_structureType->addItems({ tr("(MultiDLA)"), tr("(OSM)"), tr("(DLCA)"), tr("(DLCA2)"), tr("(MxDLA)") });
    m_structureType->setCurrentIndex(m_parameter.method);

    connect(m_structureType, SIGNAL(activated(int)), this, SLOT(changeType(int)));
    m_methodLabel = new QLabel;

    m_sizesEdit = new QLineEdit(QString::fromStdString(m_parameter.sizes));
    m_sizesLabel = new QLabel;

    m_boundaryType = new QComboBox;
    m_boundaryType->addItems({ tr("Toroid"), tr("Closed") });
    m_boundaryType->setCurrentIndex(m_parameter.isToroid ? 0 : 1);
    m_boundaryTypeLabel = new QLabel;

    m_cellSize = new QDoubleSpinBox;
    m_cellSize->setMinimum(0.3);
    m_cellSize->setMaximum(50);
    m_cellSize->setValue(m_parameter.cellSize);
    m_cellSizeLabel = new QLabel;

    m_poreDLA = new QDoubleSpinBox;
    m_poreDLA->setMinimum(0);
    m_poreDLA->setMaximum(100);
    m_poreDLA->setValue(m_parameter.porosity);
    m_poreLabel = new QLabel;

    m_initDLA = new QSpinBox;
    m_initDLA->setMinimum(1);
    m_initDLA->setMaximum(1000);
    m_initDLA->setValue(m_parameter.init);
    m_initLabel = new QLabel;

    m_stepDLA = new QSpinBox;
    m_stepDLA->setMinimum(1);
    m_stepDLA->setValue(m_parameter.step);
    m_stepLabel = new QLabel;

    m_hitDLA = new QSpinBox;
    m_hitDLA->setMinimum(1);
    m_hitDLA->setValue(m_parameter.hit);
    m_hitLabel = new QLabel;

    m_clusterDLA = new QSpinBox;
    m_clusterDLA->setMinimum(1);
    m_clusterDLA->setValue(m_parameter.cluster);
    m_clusterLabel = new QLabel;

    m_startButton = new QPushButton;
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(start()));

    bool enabled = (m_parameter.method == 0 || m_parameter.method == 3);
    m_initDLA->setEnabled(enabled);
    m_stepDLA->setEnabled(enabled);
    m_hitDLA->setEnabled(enabled);
    m_initLabel->setEnabled(enabled);
    m_stepLabel->setEnabled(enabled);
    m_hitLabel->setEnabled(enabled);
}

void MainWindow::createLayout2()
{
    // layout 2
    m_sizesLabel2    = new QLabel;
    m_cellSizeLabel2 = new QLabel;
    m_poreLabel2     = new QLabel;
    m_initLabel2     = new QLabel;
    m_stepLabel2     = new QLabel;
    m_hitLabel2      = new QLabel;
    m_clusterLabel2  = new QLabel;
    m_boundaryLabel2 = new QLabel;

    m_progressBar = new QProgressBar;
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);

    m_statusLabel = new QLabel;

    m_generateButton = new QPushButton;
    connect(m_generateButton, SIGNAL(clicked()), this, SLOT(openGen()));

    m_stopButton = new QPushButton;
    m_stopButton->setAutoFillBackground(true);
    m_stopButton->setStyleSheet("* { background-color: rgb(255, 0, 0); }");
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));
}
} // aerogen
