#ifndef PENCASE_I_H
#define PENCASE_I_H

#include "PenCaseS.h"
#include <QObject>

class MainWindow;

class Pen_i : public QObject, public virtual POA_Pen {
    Q_OBJECT
public:
    Pen_i (unsigned int id, QObject* parent = nullptr);
    ~Pen_i () override = default;

    void set_color(::CORBA::Octet r, ::CORBA::Octet g, ::CORBA::Octet b) override;
    void move_to(::CORBA::Short x, ::CORBA::Short y) override;
    void line_to(::CORBA::Short x, ::CORBA::Short y) override;
    void aline_to (::CORBA::Short x, ::CORBA::Short y) override;

    [[nodiscard]] unsigned int id() const { return mydid; }
signals:
    void color(unsigned int did, const QColor& color);
    void moveto(unsigned int did, const QPoint& p);
    void lineto(unsigned int did, const QPoint& p);
    void alineto(unsigned int did, const QPoint& p);
private:
    unsigned int mydid;
};

class  PenCase_i : public QObject, public virtual POA_PenCase {
    Q_OBJECT
public:
    PenCase_i (PortableServer::POA_ptr ppoa, MainWindow* w, QObject* parent = nullptr);
    ~PenCase_i () override;
    ::Pen_ptr new_pen() override;
    ::Pen_ptr get_pen(const char* name) override;
private:
    PortableServer::POA_var poa;
    MainWindow* win;
signals:
    void newpen(Pen_i* pen);
};

#endif /* PenI_H_  */
