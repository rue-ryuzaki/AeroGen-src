#include "mainwindow.h"

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "baseformats.h"
#include "checker.h"
#include "functions.h"

StructureGL* MainWindow::m_glStructure;
Distributor* MainWindow::m_distributor;

MainWindow::MainWindow()
    : m_translator(this),
      m_languageRuAct(tr("&Русский"), this),
      m_languageEnAct(tr("&English"), this)
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
        uint32_t hmax = 45;
        QWidget* widget1 = new QWidget;
        QFormLayout* layout1 = new QFormLayout;
        m_currentMethod.setToolTip(QString::fromStdString("MultiDLA\n\nOSM\n\nDLCA\n\n") + tr("Undefined"));
        updateGenerator();
        m_currentMethod.setFixedWidth(100);
        widget1->setMaximumHeight(hmax);
        m_currentMethod.setReadOnly(true);
        layout1->addRow(&m_currMethodLabel, &m_currentMethod);
        widget1->setLayout(layout1);
        centralLayout->addWidget(widget1, 0, 0);
        
        QWidget* widget2 = new QWidget;
        //widget2->setMaximumHeight(hmax);
        QFormLayout* layout2 = new QFormLayout;
        m_drawGL.setChecked(true);
        connect(&m_drawGL, SIGNAL(clicked()), this, SLOT(changeDrawGL()));
        layout2->addRow(&m_drawGL);

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
        .arg(int(m_glStructure->colors[0] * 255)).arg(int(m_glStructure->colors[1] * 255))
        .arg(int(m_glStructure->colors[2] * 255)));
    if (!loadSettings()) {
        loadDefault();
    }
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
    retranslate();
#ifdef _WIN32
// windows
    resize(800, 550);
#else
    resize(840, 600);
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
    QString fileName = QFileDialog::getOpenFileName(this);
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
    for (size_t i = 1; i < imgs.size(); ++i) {
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
    uint32_t w = m_glArea.width();
    uint32_t h = m_glArea.height();
    uchar* imageData = (uchar*)malloc(w * h * 3);
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
    QString filters = txts[0]->Filter();
    for (size_t i = 1; i < txts.size(); ++i) {
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

    m_glStructure->gen->save(fileName.toStdString(), pTxt->Format());

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
            idx = i;
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
            case 3 : // MultixDLA
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
               "<p>Golubchikov Mihail</p><p> </p><p>Changelog:</p>").arg((QString)program_version));
    label1->setWordWrap(true);
    gLayout->addWidget(label1, 0, 1, 2, 1);
    //create changelog
    QStringList versions;
    versions.push_back(tr("<p>1.1.5 - Refactoring and fix bugs</p>"));
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
        m_glStructure->colors[0] = color.redF();
        m_glStructure->colors[1] = color.greenF();
        m_glStructure->colors[2] = color.blueF();
        m_glStructure->colors[3] = color.alphaF();
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
        uint32_t x = regExp.cap(1).toInt();
        uint32_t y = regExp.cap(2).toInt();
        uint32_t z = regExp.cap(3).toInt();
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
    double por  = double(m_poreDLA->value()) / 100.0;
    uint32_t initial = m_initDLA->value();
    uint32_t step    = m_stepDLA->value();
    uint32_t hit     = m_hitDLA->value();
    uint32_t cluster = m_clusterDLA->value();
    double cellsize  = m_cellSize->value();

    m_parameter.method   = m_structureType->currentIndex();
    m_parameter.sizes    = m_sizesEdit->text().toStdString();
    m_parameter.porosity = m_poreDLA->value();
    m_parameter.init     = initial;
    m_parameter.step     = step;
    m_parameter.hit      = hit;
    m_parameter.cluster  = cluster;
    m_parameter.cellSize = cellsize;

    m_glStructure->setCamera(sizemax);
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
        case 3 : // MultiXDLA
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

    std::thread t(threadGen, size, por, initial, step, hit, cluster, cellsize);
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
    if (m_parameter.method == 0) {
        m_genLayout1->addRow(m_initLabel2);
        m_genLayout1->addRow(m_stepLabel2);
        m_genLayout1->addRow(m_hitLabel2);
    }
    m_genLayout1->addRow(m_clusterLabel2);
    m_genLayout1->addRow(m_statusLabel, m_progressBar);
    m_genLayout1->addRow(m_generateButton);
    m_genLayout1->addRow(m_stopButton);
}

void MainWindow::stop()
{
    m_generateButton->setEnabled(true);
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
    m_distr = m_distributor->distribution();
    QDialog* distrDialog = new QDialog(this);
    distrDialog->setMinimumSize(350, 250);
    distrDialog->setWindowTitle(tr("Pore distribution"));

    QFormLayout*layout = new QFormLayout;
    //layout->addRow(new QLabel(tr("Pore distribution don't work yet")));
    layout->addRow(new QLabel(tr("Testing version")));
    // table!
    QTableWidget* table = new QTableWidget;
    table->setRowCount(m_distr.size());
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({ tr("Pore size, nm"), tr("Volume, nm3"), tr("Percentage, %") });
    double sum = 0.0;
    if (!m_distr.empty()) {
        double prevVol = m_distr.back().vol;
        m_distr.back().count = prevVol / ((4.0 / 3.0) * M_PI * m_distr.back().r * m_distr.back().r * m_distr.back().r);
        for (int32_t i = int32_t(m_distr.size()) - 2; i >= 0; --i) {
            double currVol = m_distr[i].vol - prevVol;
            m_distr[i].count = currVol / ((4.0 / 3.0) * M_PI * m_distr[i].r * m_distr[i].r * m_distr[i].r);
            sum += m_distr[i].count;
            prevVol = m_distr[i].vol;
        }
        for (size_t i = 0; i < m_distr.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(QString::number(2 * m_distr[i].r)));
            table->setItem(i, 1, new QTableWidgetItem(QString::number(m_distr[i].vol)));
            double x = 100.0 * m_distr[i].count / sum;
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
    sqrArea = m_glStructure->gen->surfaceArea(m_density.value());

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
    std::thread t(threadRunDistr, m_cellSize->value(), dFrom, dTo, dStep);
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
    }
}

void MainWindow::settings()
{
    int res = SettingsForm::dialog(tr("Settings"), m_setParams, m_glStructure->params);
    if (res == QDialogButtonBox::Yes) {
        // save settings && update ?
        if (m_drawGL.isChecked()) {
            m_glStructure->restruct();
        }
    } else {
        // do nothing
    }
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
        m_glStructure->restruct();
    }
}

void MainWindow::axesGL()
{
    m_glStructure->showAxes = m_showAxes.isChecked();
    if (m_drawGL.isChecked()) {
        m_glStructure->restruct();
    }
}

void MainWindow::borderGL()
{
    m_glStructure->showBorders = m_showBorders.isChecked();
    if (m_drawGL.isChecked()) {
        m_glStructure->restruct();
    }
}

void MainWindow::switchShaders()
{
    int32_t shaders = -1;
    if (QObject::sender() == &m_effectsDisableAct) {
        shaders = 0;
    }
    if (QObject::sender() == &m_effectsLambertAct) {
        shaders = 1;
    }
    if (QObject::sender() == &m_effectsWrapAroundAct) {
        shaders = 2;
    }
    if (QObject::sender() == &m_effectsPhongAct) {
        shaders = 3;
    }
    if (QObject::sender() == &m_effectsBlinnAct) {
        shaders = 4;
    }
    if (QObject::sender() == &m_effectsIsotropWardAct) {
        shaders = 5;
    }
    if (QObject::sender() == &m_effectsOrenNayarAct) {
        shaders = 6;
    }
    if (QObject::sender() == &m_effectsCookTorranceAct) {
        shaders = 7;
    }
    if (QObject::sender() == &m_effectsAnisotropAct) {
        shaders = 8;
    }
    if (QObject::sender() == &m_effectsAnisotropWardAct) {
        shaders = 9;
    }
    if (QObject::sender() == &m_effectsMinnaertAct) {
        shaders = 10;
    }
    if (QObject::sender() == &m_effectsAshikhminShirleyAct) {
        shaders = 11;
    }
    if (QObject::sender() == &m_effectsCartoonAct) {
        shaders = 12;
    }
    if (QObject::sender() == &m_effectsGoochAct) {
        shaders = 13;
    }
    if (QObject::sender() == &m_effectsRimAct) {
        shaders = 14;
    }
    if (QObject::sender() == &m_effectsSubsurfaceAct) {
        shaders = 15;
    }
    if (QObject::sender() == &m_effectsBidirectionalAct) {
        shaders = 16;
    }
    if (QObject::sender() == &m_effectsHemisphericAct) {
        shaders = 17;
    }
    if (QObject::sender() == &m_effectsTrilightAct) {
        shaders = 18;
    }
    if (QObject::sender() == &m_effectsLommelSeeligerAct) {
        shaders = 19;
    }
    if (QObject::sender() == &m_effectsStraussAct) {
        shaders = 20;
    }
    if (shaders == -1) {
        return;
    }
    m_glStructure->needInit = shaders;
    if (shaders != 0 && !m_glStructure->supportShaders()) {
        if (m_glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shaders = 0;
        }
    }
    m_glStructure->enableShader(shaders);
    shaders = m_glStructure->shadersStatus();
    m_effectsDisableAct.setChecked(shaders == 0);
    m_effectsLambertAct.setChecked(shaders == 1);
    m_effectsWrapAroundAct.setChecked(shaders == 2);
    m_effectsPhongAct.setChecked(shaders == 3);
    m_effectsBlinnAct.setChecked(shaders == 4);
    m_effectsIsotropWardAct.setChecked(shaders == 5);
    m_effectsOrenNayarAct.setChecked(shaders == 6);
    m_effectsCookTorranceAct.setChecked(shaders == 7);
    m_effectsAnisotropAct.setChecked(shaders == 8);
    m_effectsAnisotropWardAct.setChecked(shaders == 9);
    m_effectsMinnaertAct.setChecked(shaders == 10);
    m_effectsAshikhminShirleyAct.setChecked(shaders == 11);
    m_effectsCartoonAct.setChecked(shaders == 12);
    m_effectsGoochAct.setChecked(shaders == 13);
    m_effectsRimAct.setChecked(shaders == 14);
    m_effectsSubsurfaceAct.setChecked(shaders == 15);
    m_effectsBidirectionalAct.setChecked(shaders == 16);
    m_effectsHemisphericAct.setChecked(shaders == 17);
    m_effectsTrilightAct.setChecked(shaders == 18);
    m_effectsLommelSeeligerAct.setChecked(shaders == 19);
    m_effectsStraussAct.setChecked(shaders == 20);

    restructGL();
}

void MainWindow::switchToLanguage()
{
    int32_t language = -1;
    if (QObject::sender() == &m_languageRuAct) {
        language = 0;
    }
    if (QObject::sender() == &m_languageEnAct) {
        language = 1;
    }
    bool wasDialog = false;
    if (QObject::sender() == &m_buttonRu) {
        wasDialog = true;
        language = 0;
    }
    if (QObject::sender() == &m_buttonEn) {
        wasDialog = true;
        language = 1;
    }
    if (language == -1) {
        return;
    }
    m_translator.load(m_locales[language]);
    m_languageRuAct.setChecked(language == 0);
    m_languageEnAct.setChecked(language == 1);
    retranslate();
    if (wasDialog) {
        m_langDialog.accept();
    }
}

void MainWindow::changeType(int value)
{
    bool enabled = (value == 0);
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
    m_genLayout1->addRow(m_startButton);
    //m_panelBox.setLayout(genLayout);
}

void MainWindow::closeGen()
{
    //generateDialog->close();
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

void MainWindow::selectLanguage()
{
    m_langDialog.setFixedSize(170, 200);
    
    QFormLayout* layout = new QFormLayout;
    layout->addRow(new QLabel(tr("Select language:")));
    uint32_t w = 150;
    uint32_t h = 80;

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
    uint32_t shaders = m_glStructure->shadersStatus();
    if (m_languageRuAct.isChecked()) {
        language = 0;
    }
    if (m_languageEnAct.isChecked()) {
        language = 1;
    }
    if (language == -1) {
        std::cerr << "Error language" << std::endl;
        return;
    }
    std::ofstream out;
    out.open(m_settingsFile.c_str(), std::ios_base::trunc);
    out << "lang    = " << language << "\n";
    out << "color   = " << m_glStructure->colors[0] << ";" << m_glStructure->colors[1] << ";" << m_glStructure->colors[2] << "\n";
    out << "shaders = " << shaders << "\n";
    
    out.close();
}

bool MainWindow::loadSettings()
{
    if (!fileExists(m_settingsFile.c_str())) {
        return false;
    }
    
    IniParser parser(m_settingsFile.c_str());
    int32_t language = -1;
    std::string slang = parser.property("lang");
    try {
        language = stoi(slang);
    } catch (...) {
        std::cerr << "Error parsing lang" << std::endl;
        return false;
    }
    
    int32_t shaders = -1;
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
        for (size_t i = 0; i < 3; ++i) {
            colors[i] = QString::fromStdString(vcolors[i]).toFloat();// stof(vcolors[i], &sz);
        }
    } catch (...) {
        std::cerr << "Error parsing colors" << std::endl;
        return false;
    }
    // alpha not used
    for (size_t i = 0; i < 3; ++i) {
        if (colors[i] < 0 || colors[i] > 1) {
            std::cerr << "Error parsing colors. Expect values between 0 & 1." << std::endl;;
            return false;
        }
        m_glStructure->colors[i] = colors[i];
    }
    m_colorButton.setStyleSheet(QString("* { background-color: rgb(%1, %2, %3); }")
            .arg(int(m_glStructure->colors[0] * 255)).arg(int(m_glStructure->colors[1] * 255))
            .arg(int(m_glStructure->colors[2] * 255)));
    defaultShaders();
    m_glStructure->needInit = shaders;
    if (shaders != 0 && !m_glStructure->supportShaders()) {
        if (m_glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shaders = 0;
        }
    }
    m_glStructure->enableShader(shaders);
    //shaders = glStructure->ShadersStatus();
    m_effectsDisableAct.setChecked(shaders == 0);
    m_effectsLambertAct.setChecked(shaders == 1);
    m_effectsWrapAroundAct.setChecked(shaders == 2);
    m_effectsPhongAct.setChecked(shaders == 3);
    m_effectsBlinnAct.setChecked(shaders == 4);
    m_effectsIsotropWardAct.setChecked(shaders == 5);
    m_effectsOrenNayarAct.setChecked(shaders == 6);
    m_effectsCookTorranceAct.setChecked(shaders == 7);
    m_effectsAnisotropAct.setChecked(shaders == 8);
    m_effectsAnisotropWardAct.setChecked(shaders == 9);
    m_effectsMinnaertAct.setChecked(shaders == 10);
    m_effectsAshikhminShirleyAct.setChecked(shaders == 11);
    m_effectsCartoonAct.setChecked(shaders == 12);
    m_effectsGoochAct.setChecked(shaders == 13);
    m_effectsRimAct.setChecked(shaders == 14);
    m_effectsSubsurfaceAct.setChecked(shaders == 15);
    m_effectsBidirectionalAct.setChecked(shaders == 16);
    m_effectsHemisphericAct.setChecked(shaders == 17);
    m_effectsTrilightAct.setChecked(shaders == 18);
    m_effectsLommelSeeligerAct.setChecked(shaders == 19);
    m_effectsStraussAct.setChecked(shaders == 20);
    if (shaders < 0 || shaders > 20 || shaders == 7 || shaders == 8
            || shaders == 9 || shaders == 11) {
        std::cout << "Error shaders value" << std::endl;
        return false;
    }
    restructGL();

    if (language == 0 || language == 1) {
        m_translator.load(m_locales[language]);
        m_languageRuAct.setChecked(language == 0);
        m_languageEnAct.setChecked(language == 1);
        return true;
    }
    return false;
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
            .arg(int(m_glStructure->colors[0] * 255)).arg(int(m_glStructure->colors[1] * 255))
            .arg(int(m_glStructure->colors[2] * 255)));
    // shaders
    uint32_t shaders = 0; // default disabled
    m_glStructure->needInit = shaders;
    if (shaders != 0 && !m_glStructure->supportShaders()) {
        if (m_glStructure->isInitialized()) {
            QMessageBox::warning(this, tr("Shaders support"), tr("Shaders not supported on your PC"));
            shaders = 0;
        }
    }
    m_glStructure->enableShader(shaders);
    //shaders = glStructure->ShadersStatus();
    m_effectsDisableAct.setChecked(shaders == 0);
    m_effectsLambertAct.setChecked(shaders == 1);
    m_effectsWrapAroundAct.setChecked(shaders == 2);
    m_effectsPhongAct.setChecked(shaders == 3);
    m_effectsBlinnAct.setChecked(shaders == 4);
    m_effectsIsotropWardAct.setChecked(shaders == 5);
    m_effectsOrenNayarAct.setChecked(shaders == 6);
    m_effectsCookTorranceAct.setChecked(shaders == 7);
    m_effectsAnisotropAct.setChecked(shaders == 8);
    m_effectsAnisotropWardAct.setChecked(shaders == 9);
    m_effectsMinnaertAct.setChecked(shaders == 10);
    m_effectsAshikhminShirleyAct.setChecked(shaders == 11);
    m_effectsCartoonAct.setChecked(shaders == 12);
    m_effectsGoochAct.setChecked(shaders == 13);
    m_effectsRimAct.setChecked(shaders == 14);
    m_effectsSubsurfaceAct.setChecked(shaders == 15);
    m_effectsBidirectionalAct.setChecked(shaders == 16);
    m_effectsHemisphericAct.setChecked(shaders == 17);
    m_effectsTrilightAct.setChecked(shaders == 18);
    m_effectsLommelSeeligerAct.setChecked(shaders == 19);
    m_effectsStraussAct.setChecked(shaders == 20);
    
    restructGL();
    // language
    selectLanguage();
}

void MainWindow::defaultShaders()
{
    // base
    m_glStructure->params.specPower      = 30.0f;
    m_glStructure->params.specColor[0]   = 0.7f;
    m_glStructure->params.specColor[1]   = 0.7f;
    m_glStructure->params.specColor[2]   = 0.0f;
    m_glStructure->params.specColor[3]   = 1.0f;
    // shaders
    // wrap
    m_glStructure->params.wrap_factor    = 0.5f;
    // iso-ward
    m_glStructure->params.iso_ward_k     = 10.0f;
    // oren
    m_glStructure->params.oren_a         = 1.0f;
    m_glStructure->params.oren_b         = 0.45f;
    // minnaert
    m_glStructure->params.minnaert_k     = 0.8f;
    // cartoon
    m_glStructure->params.cartoon_edgePower = 3.0f;
    // gooch
    m_glStructure->params.gooch_diffuseWarm = 0.45f;
    m_glStructure->params.gooch_diffuseCool = 0.45f;
    // rim
    m_glStructure->params.rim_rimPower   = 8.0f;
    m_glStructure->params.rim_bias       = 0.3f;
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
    m_effectsDisableAct.setText(tr("Disable"));
    m_effectsLambertAct.setText(tr("Lambert"));
    m_effectsWrapAroundAct.setText(tr("Wrap-around"));
    m_effectsPhongAct.setText(tr("Phong"));
    m_effectsBlinnAct.setText(tr("Blinn"));
    m_effectsIsotropWardAct.setText(tr("Isotropic Ward"));
    m_effectsOrenNayarAct.setText(tr("Oren-Nayar"));
    m_effectsCookTorranceAct.setText(tr("Cook-Torrance"));
    m_effectsAnisotropAct.setText(tr("Anisotropic"));
    m_effectsAnisotropWardAct.setText(tr("Anisotropic Ward"));
    m_effectsMinnaertAct.setText(tr("Minnaert"));
    m_effectsAshikhminShirleyAct.setText(tr("Ashikhmin-Shirley"));
    m_effectsCartoonAct.setText(tr("Cartoon"));
    m_effectsGoochAct.setText(tr("Gooch"));
    m_effectsRimAct.setText(tr("Rim"));
    m_effectsSubsurfaceAct.setText(tr("Subsurface"));
    m_effectsBidirectionalAct.setText(tr("Bidirectional"));
    m_effectsHemisphericAct.setText(tr("Hemispheric"));
    m_effectsTrilightAct.setText(tr("Trilight"));
    m_effectsLommelSeeligerAct.setText(tr("Lommel-Seeliger"));
    m_effectsStraussAct.setText(tr("Strauss"));
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
    m_generateButton->setText(tr("Generate new"));
    m_stopButton->setText(tr("Stop"));

    m_cancelDistrButton.setText(tr("Stop"));

    m_propsBox.setTitle(tr("Properties"));
    m_densityLabel.setText(tr("Density, kg/m<sup>3</sup>:"));

    m_tabProps.setTabText(0, tr("Specific surface area"));
    m_surfaceAreaLabel.setText(tr("Specific surface area, m<sup>2</sup>/g:"));
    m_densityAeroLabel.setText(tr("Aerogel density, kg/m<sup>3</sup>:"));
    m_porosityAeroLabel.setText(tr("Current porosity, %:"));
    m_propButton.setText(tr("Calculate"));

    m_tabProps.setTabText(1, tr("Distribution"));
    m_distFromLabel.setText(tr("from, nm:"));
    m_distToLabel.setText(tr("to, nm:"));
    m_distStepLabel.setText(tr("step, nm:"));
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
        case gen_mxdla :
            text = "MxDLA";
            break;
        default :
            m_currentMethod.setText(tr("Undefined"));
            return;
    }
    m_currentMethod.setText(text);
}

void MainWindow::threadGen(const Sizes& sizes, double por, uint32_t initial, uint32_t step,
                           uint32_t hit, uint32_t cluster, double cellsize)
{
    m_glStructure->gen->generate(sizes, por, initial, step, hit, cluster, cellsize);
    m_glStructure->gen->run = false;
}

void MainWindow::threadRunDistr(double cellSize, double dFrom, double dTo, double dStep)
{
    m_distributor->calculate(m_glStructure->gen->field(), cellSize, dFrom, dTo, dStep);
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

    connect(&m_settingsAct, SIGNAL(triggered()), this, SLOT(settings()));
    connect(&m_updateAct, SIGNAL(triggered()), this, SLOT(updates()));
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
    m_locales.push_back(":/lang/aerogen_ru.qm");
    m_locales.push_back(":/lang/aerogen_en.qm");

    m_languageRuAct.setIcon(QIcon(":/ru.png"));
    m_languageRuAct.setCheckable(true);
    connect(&m_languageRuAct, SIGNAL(triggered()), this, SLOT(switchToLanguage()));

    m_languageEnAct.setIcon(QIcon(":/en.png"));
    m_languageEnAct.setCheckable(true);
    m_languageEnAct.setChecked(true);
    connect(&m_languageEnAct, SIGNAL(triggered()), this, SLOT(switchToLanguage()));
    
    m_languageMenu->addAction(&m_languageRuAct);
    m_languageMenu->addAction(&m_languageEnAct);
    
    // shaders
    m_effectsDisableAct.setCheckable(true);
    m_effectsDisableAct.setChecked(true);
    connect(&m_effectsDisableAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsLambertAct.setCheckable(true);
    connect(&m_effectsLambertAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsWrapAroundAct.setCheckable(true);
    connect(&m_effectsWrapAroundAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsPhongAct.setCheckable(true);
    connect(&m_effectsPhongAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsBlinnAct.setCheckable(true);
    connect(&m_effectsBlinnAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsIsotropWardAct.setCheckable(true);
    connect(&m_effectsIsotropWardAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsOrenNayarAct.setCheckable(true);
    connect(&m_effectsOrenNayarAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsCookTorranceAct.setCheckable(true);
    m_effectsCookTorranceAct.setEnabled(false);
    connect(&m_effectsCookTorranceAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsAnisotropAct.setCheckable(true);
    m_effectsAnisotropAct.setEnabled(false);
    connect(&m_effectsAnisotropAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsAnisotropWardAct.setCheckable(true);
    m_effectsAnisotropWardAct.setEnabled(false);
    connect(&m_effectsAnisotropWardAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsMinnaertAct.setCheckable(true);
    connect(&m_effectsMinnaertAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsAshikhminShirleyAct.setCheckable(true);
    m_effectsAshikhminShirleyAct.setEnabled(false);
    connect(&m_effectsAshikhminShirleyAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsCartoonAct.setCheckable(true);
    connect(&m_effectsCartoonAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsGoochAct.setCheckable(true);
    connect(&m_effectsGoochAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsRimAct.setCheckable(true);
    connect(&m_effectsRimAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsSubsurfaceAct.setCheckable(true);
    connect(&m_effectsSubsurfaceAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsBidirectionalAct.setCheckable(true);
    connect(&m_effectsBidirectionalAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsHemisphericAct.setCheckable(true);
    connect(&m_effectsHemisphericAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsTrilightAct.setCheckable(true);
    connect(&m_effectsTrilightAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsLommelSeeligerAct.setCheckable(true);
    connect(&m_effectsLommelSeeligerAct, SIGNAL(triggered()), this, SLOT(switchShaders()));

    m_effectsStraussAct.setCheckable(true);
    connect(&m_effectsStraussAct, SIGNAL(triggered()), this, SLOT(switchShaders()));
    
    m_effectsMenu->addAction(&m_effectsDisableAct);
    m_effectsMenu->addSeparator();
    m_effectsMenu->addAction(&m_effectsLambertAct);
    m_effectsMenu->addAction(&m_effectsWrapAroundAct);
    m_effectsMenu->addAction(&m_effectsPhongAct);
    m_effectsMenu->addAction(&m_effectsBlinnAct);
    m_effectsMenu->addAction(&m_effectsIsotropWardAct);
    m_effectsMenu->addAction(&m_effectsOrenNayarAct);
    m_effectsMenu->addAction(&m_effectsCookTorranceAct);
    m_effectsMenu->addAction(&m_effectsAnisotropAct);
    m_effectsMenu->addAction(&m_effectsAnisotropWardAct);
    m_effectsMenu->addAction(&m_effectsMinnaertAct);
    m_effectsMenu->addAction(&m_effectsAshikhminShirleyAct);
    m_effectsMenu->addAction(&m_effectsCartoonAct);
    m_effectsMenu->addAction(&m_effectsGoochAct);
    m_effectsMenu->addAction(&m_effectsRimAct);
    m_effectsMenu->addAction(&m_effectsSubsurfaceAct);
    m_effectsMenu->addAction(&m_effectsBidirectionalAct);
    m_effectsMenu->addAction(&m_effectsHemisphericAct);
    m_effectsMenu->addAction(&m_effectsTrilightAct);
    m_effectsMenu->addAction(&m_effectsLommelSeeligerAct);
    m_effectsMenu->addAction(&m_effectsStraussAct);
}

void MainWindow::loadFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return;
    }
    m_curFile = fileName;
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_structureType->setCurrentIndex(in.readLine().toInt());
    m_sizesEdit->setText  (in.readLine());
    m_poreDLA->setValue   (in.readLine().toDouble());
    m_initDLA->setValue   (in.readLine().toInt());
    m_stepDLA->setValue   (in.readLine().toInt());
    m_hitDLA->setValue    (in.readLine().toInt());
    m_clusterDLA->setValue(in.readLine().toInt());
    m_cellSize->setValue  (in.readLine().toDouble());
    m_parameter.method   = m_structureType->currentIndex();
    m_parameter.sizes    = m_sizesEdit->text().toStdString();
    m_parameter.porosity = m_poreDLA->value();
    m_parameter.init     = m_initDLA->value();
    m_parameter.step     = m_stepDLA->value();
    m_parameter.hit      = m_hitDLA->value();
    m_parameter.cluster  = m_clusterDLA->value();
    m_parameter.cellSize = m_cellSize->value();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("File loaded"), 5000);
}

void MainWindow::saveFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return;
    }
    m_curFile = fileName;
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << m_parameter.method    << endl;
    out << QString::fromStdString(m_parameter.sizes) << endl;
    out << m_parameter.porosity  << endl;
    out << m_parameter.init      << endl;
    out << m_stepDLA->value()    << endl;
    out << m_hitDLA->value()     << endl;
    out << m_clusterDLA->value() << endl;
    out << m_parameter.cellSize  << endl;
    QApplication::restoreOverrideCursor();

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
#ifdef _WIN32
// windows
    m_tabProps.setFixedSize(m_panelWidth, 160);
#elif defined __linux__
//linux
    m_tabProps.setFixedSize(m_panelWidth, 180);
#elif defined __APPLE__
// apple
    m_tabProps.setFixedSize(m_panelWidth, 180);
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
    m_structureType->addItems({ tr("(MultiDLA)"), tr("(OSM)"), tr("(DLCA)"), tr("(MxDLA)") });
    m_structureType->setCurrentIndex(m_parameter.method);

    connect(m_structureType, SIGNAL(activated(int)), this, SLOT(changeType(int)));
    m_methodLabel = new QLabel;

    m_sizesEdit = new QLineEdit(QString::fromStdString(m_parameter.sizes));
    m_sizesLabel = new QLabel;

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

    bool enabled = (m_parameter.method == 0);
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
