#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QTextEdit>

#include "Console.h"

atsui::Console* console;

void SetupWindow( int argc, char *argv[] )
{
    QMainWindow* window = new QMainWindow;
    window->resize( 800, 600 );
    QWidget* centralWidget = new QWidget(window);
    QGridLayout* layout = new QGridLayout(centralWidget);
    console = new atsui::Console;
    layout->addWidget(console, 0, 0, 1, 1);
    window->setCentralWidget(centralWidget);
    window->show( );
}

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    SetupWindow( argc, argv );

    return app.exec( );
}
