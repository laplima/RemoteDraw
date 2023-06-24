#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "orbworker.h"
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QRandomGenerator>
#include <iostream>
#include <algorithm>
#include <fmt/format.h>
#include <QFileDialog>
#include <QFile>
#define SPDLOG_FMT_EXTERNAL
#include <spdlog/spdlog.h>

using std::flush;
using fmt::print;
using spdlog::info;
using namespace colibry;

std::vector<std::pair<QString,QColor>> MainWindow::colormap = {
    { "black", Qt::black },
    { "red", Qt::red },
    { "blue", Qt::blue },
    { "green", Qt::darkGreen },
    { "magenta", Qt::magenta }
};

MainWindow::MainWindow(ORBManager& om, const QString& name, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), orb{om}
{
    info("Seeting up UI");

    ui->setupUi(this);
    ui->name->setText(name);
    for (const auto& [name,color] : colormap)
        ui->color_cb->addItem(name);

    // ORB init
    info("Initializing ORB");
    orb.activate_rootpoa();
    ppoa = orb.create_child_poa("case", {POAPolicy::NO_IMPLICIT_ACTIVATION, POAPolicy::USER_ID});
    case_i = new PenCase_i{ppoa.in(), this};
    case_ref = orb.activate_object<PenCase>(*case_i);

    info("Starting orb thread");
    // start ORB thread
    ORBWorker* ow = new ORBWorker{om, this};
    connect(ow, &ORBWorker::orbfinished, ow, &QObject::deleteLater);
    ow->start(); // <- BLOCKS MAIN THREAD...

    // connections
    info("Making connections");
    connect(ui->quit_bt, &QPushButton::clicked, this, &MainWindow::quit);
    connect(ui->clear_bt, &QPushButton::clicked, this, &MainWindow::clear);
    connect(ui->color_cb, &QComboBox::currentIndexChanged, this, &MainWindow::change_color);
    connect(ui->exportior_bt, &QPushButton::clicked, this, &MainWindow::export_ior);
    connect(case_i, &PenCase_i::newpen, this, &MainWindow::new_pen);
    connect(ui->connect_bt, &QPushButton::clicked, this, &MainWindow::import_ior);
    connect(ui->async_rb, &QRadioButton::toggled, ui->canvas, &RenderArea::toggle_async);
}

MainWindow::~MainWindow()
{
    orb.shutdown();
    delete ui;

    // delete ior file
    if (!saved_ior_file.empty())
        QFile::remove(saved_ior_file.c_str());
}

UId MainWindow::new_user()
{
    ++nusers;
    ui->nusers->setText(QString("Connected users: ")+QString::number(nusers));
    return ui->canvas->new_user({Qt::black, {0,0}});
}

void MainWindow::change_color()
{
    auto color = ui->color_cb->currentText();
    auto p = std::find_if(begin(colormap), end(colormap), [&color](const std::pair<QString,QColor>& x) { return x.first == color; });
    if (p != colormap.end())
        ui->canvas->set_color(mydid,p->second);
}

void MainWindow::clear()
{
    ui->canvas->clear();
}

void MainWindow::export_ior()
{
    QString dir = "/tmp/" + ui->name->text() + ".ior";
    auto filename = QFileDialog::getSaveFileName(this,tr("Save IOR File"),
                                                 dir, tr("IOR (*.ior)"));
    saved_ior_file = filename.toStdString();
    orb.save_ior(saved_ior_file, case_ref.in());
}

void MainWindow::import_ior()
{
    QString dir = "/tmp";
    auto filename = QFileDialog::getOpenFileName(this, tr("Read IOR File"),
                                                 dir,tr("IOR (*.ior)"));
    if (filename.isEmpty())
        return;

    try {
        peer_ref = orb.string_to_object<PenCase>(("file://" + filename).toStdString());
        peer_pen_ref = peer_ref->get_pen(ui->name->text().toStdString().c_str());
        auto mycolor = ui->canvas->color(mydid);
        peer_pen_ref->set_color(mycolor.red(), mycolor.green(), mycolor.blue());
        ui->canvas->set_remote_pen(peer_pen_ref.in());

        QPixmap mypix{":/rdi/rdi_connected.png"};
        ui->cn_lb->setPixmap(mypix);
    } catch(const CORBA::Exception& e) {
        spdlog::error("CORBA error");
    }
}

void MainWindow::quit()
{
    close();
}

// CORBA signals ---------------------------------------------------------------------

void MainWindow::new_pen(Pen_i* p)
{
    // not needed...
}

void MainWindow::set_color(unsigned int uid, const QColor& c)
{
    ui->canvas->set_color(uid,c);
}

void MainWindow::move_to(UId uid, const QPoint& p)
{
    ui->canvas->move_to(uid, p);
}

void MainWindow::line_to(UId uid, const QPoint& p)
{
    ui->canvas->line_to(uid, p);
}

void MainWindow::aline_to(UId uid, const QPoint& p)
{
    ui->canvas->line_to(uid, p);
}
