#include <QApplication>
#include <cstdlib>
#include <time.h>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    //srand((unsigned) time(0));
    //srand(time(0));
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);
    
    QApplication app(argc, argv);
    // create and show your widgets here
    MainWindow* window = new MainWindow;
    window->show();
    return app.exec();
}
