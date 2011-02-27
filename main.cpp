/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#include <QApplication>

#include "mainwin.h"

int main(int iInArgc, char *pacArgv[])
{
    QApplication oApp(iInArgc, pacArgv);
    MainWin oMainWin;
    oMainWin.show();
    return oApp.exec();
}
