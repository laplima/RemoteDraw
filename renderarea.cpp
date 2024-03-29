#include "renderarea.h"
#include <QPainterPath>
#include <QMouseEvent>
#include <fmt/format.h>
#define SPDLOG_FMT_EXTERNAL
#include <spdlog/spdlog.h>

RenderArea::RenderArea(QWidget *parent)
    : QWidget{parent}, pixmap{800,608}
{
    QPalette palette;
    palette.setColor(QPalette::Base, QColorConstants::LightGray);
    setPalette(palette);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    clear();

    default_user = new_user({Qt::black, {0,0}});

    pm_painter.begin(&pixmap);
    pm_painter.setRenderHint(QPainter::Antialiasing);
}

RenderArea::~RenderArea()
{
    pm_painter.end();
}

QSize RenderArea::sizeHint() const
{
    return QSize(800,608);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(800,608);
}

UId RenderArea::new_user(const DrawingInfo& di)
{
    dusers.emplace_back(di);
    return dusers.size()-1;
}

void RenderArea::rm_user(UId uid)
{
    dusers.erase(dusers.begin() + uid);
}

void RenderArea::clear()
{
    pixmap.fill(QColorConstants::White);
    this->update();
}


void RenderArea::draw_line(UId uid, const QPoint& a, const QPoint& b)
{
    // uid is supposed to be valid
    // QPainter p{&pixmap};
    // p.setRenderHint(QPainter::Antialiasing);
    // p.setPen({dusers[uid].color, 2.0});
    // p.drawLine(a,b);

    pm_painter.setPen({dusers[uid].color, 2.0});
    pm_painter.drawLine(a,b);

    this->update();
}

void RenderArea::move_to(UId uid, const QPoint& a)
{
    if (validate(uid))
        dusers[uid].pos = a;
}

void RenderArea::line_to(UId uid, const QPoint& a)
{
    if (validate(uid)) {
        draw_line(uid,dusers[uid].pos,a);
        dusers[uid].pos = a;
    }
}

void RenderArea::set_color(UId uid, const QColor& c)
{
    if (validate(uid))
        dusers[uid].color = c;
    if (CORBA::is_nil(pen_ref))
        return;
    try {
        pen_ref->set_color(c.red(),c.green(),c.blue());
    } catch (const CORBA::Exception& e) {
        spdlog::error("Communication with peer RD failed.");
        pen_ref = Pen::_nil();
    }
}

void RenderArea::mousePressEvent(QMouseEvent* e)
{
    move_to(default_user, e->pos());
    if (CORBA::is_nil(pen_ref))
        return;
    try {
        pen_ref->move_to(e->pos().x(),e->pos().y());
    } catch (const CORBA::Exception& e) {
        spdlog::error("Communication with peer RD failed.");
        pen_ref = Pen::_nil();
    }
}

void RenderArea::mouseMoveEvent(QMouseEvent* e)
{
    line_to(default_user, e->pos());
    if (CORBA::is_nil(pen_ref))
        return;
    try {
        if (async)
            pen_ref->aline_to(e->pos().x(),e->pos().y());
        else
            pen_ref->line_to(e->pos().x(),e->pos().y());
    } catch (const CORBA::Exception& e) {
        spdlog::error("Communication with peer RD failed.");
        pen_ref = Pen::_nil();
    }
}

void RenderArea::paintEvent(QPaintEvent* e)
{
    QPainter painter{this};

//    painter.save();

    // frame
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColorConstants::Red);
    painter.drawRect(0,0,800,608);

    // pixmap
    painter.drawPixmap(0,0, pixmap);

    // other...
//    QPainterPath path;
//    path.moveTo(20, 80);
//    path.lineTo(20, 30);
//    path.cubicTo(80, 0, 50, 50, 80, 80);
//    painter.setPen(Qt::SolidLine);
//    painter.setBrush(Qt::SolidPattern);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.drawPath(path);

//    painter.restore();
}

void RenderArea::toggle_async(bool checked)
{
    async = !async;
}
