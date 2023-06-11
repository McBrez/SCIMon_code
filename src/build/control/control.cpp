// Qt includes
#include <QApplication>
#include <QThread>

// UI includes
#include "dialog.h"

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <control_worker_wrapper.hpp>>


INITIALIZE_EASYLOGGINGPP

#define MESSAGE_DISTRIBUTOR_DEFAULT_INTERVAL 100

int main(int argc, char *argv[])
{
    LOG(INFO) << "Starting up SCIMon Control.";
    QApplication app(argc, argv);

    Dialog mainWin;

    // Starting the UI.
    mainWin.show();
    return app.exec();
}
