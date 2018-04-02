#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[])
{
    srand(uint32_t(time(nullptr)));
    
    QApplication app(argc, argv);
    // create and show your widgets here
    aerogen::MainWindow* window = new aerogen::MainWindow;
    window->show();
    return app.exec();
}
