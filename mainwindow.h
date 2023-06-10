#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <memory>
#include <colibry/ORBManager.h>
#include "renderarea.h"
#include "PenCaseI.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(colibry::ORBManager& om, QWidget *parent = nullptr);
    ~MainWindow();
    UId new_user();
public slots:
    // interface
    void clear();
    void quit();
    void change_color();
    // corba
    void new_pen(Pen_i* pen);       // called by PenCase to connect pen signals
    void set_color(unsigned int uid, const QColor& c);
    void move_to(UId uid, const QPoint& p);
    void line_to(UId uid, const QPoint& p);
    void aline_to(UId uid, const QPoint& p);
private:
    Ui::MainWindow *ui;
    colibry::ORBManager& orb;
    PortableServer::POA_var ppoa;
    PenCase_i* case_i;
    const UId mydid = 0;     // default drawing user id
    static std::map<QString,QColor> colormap;
    size_t nusers = 0;      // connected users (i.e., pens)
};

#endif // MAINWINDOW_H
