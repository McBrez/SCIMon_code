#include <QApplication>
#include "dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Dialog mainWin;
    mainWin.show();
    return app.exec();
}
