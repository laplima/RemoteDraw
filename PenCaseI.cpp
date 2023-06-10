#include "PenCaseI.h"
#include <QColor>
#include <QPoint>
#include <string>
#include "mainwindow.h"

using namespace std;

Pen_i::Pen_i (unsigned int id, QObject* parent)
    : QObject{parent}, mydid{id}
{
}

void Pen_i::set_color(::CORBA::Octet r, ::CORBA::Octet g, ::CORBA::Octet b)
{
    emit color(mydid, {r,g,b});
}

void Pen_i::move_to(::CORBA::Short x, ::CORBA::Short y)
{
    emit moveto(mydid, {x,y});
}

void Pen_i::line_to (::CORBA::Short x, ::CORBA::Short y)
{
    emit lineto(mydid, {x,y});
}

void Pen_i::aline_to (::CORBA::Short x, ::CORBA::Short y)
{
    emit alineto(mydid, {x,y});
}

// PenCase

PenCase_i::PenCase_i (PortableServer::POA_ptr ppoa, MainWindow* w, QObject* p)
    : QObject{p}, poa{PortableServer::POA::_duplicate(ppoa)}, win{w}
{
}

PenCase_i::~PenCase_i ()
{
    // free allocated objects?
}

::Pen_ptr PenCase_i::new_pen()
{
    unsigned int did = win->new_user();
    string id = "pen-" + to_string(did);

    return get_pen(id.c_str());

    // PortableServer::ObjectId_var pen_id = PortableServer::string_to_ObjectId(id.c_str());
    // auto* c = new Pen_i{did};

    // connect(c, &Pen_i::color, win, &MainWindow::set_color);
    // connect(c, &Pen_i::moveto, win, &MainWindow::move_to);
    // connect(c, &Pen_i::lineto, win, &MainWindow::line_to);
    // connect(c, &Pen_i::alineto, win, &MainWindow::aline_to);

    // poa->activate_object_with_id(pen_id.in(), c);
    // CORBA::Object_ptr ref = poa->id_to_reference(pen_id.in());
    // return Pen::_duplicate(Pen::_narrow(ref));
}

::Pen_ptr PenCase_i::get_pen(const char* name)
{
    PortableServer::ObjectId_var pen_id = PortableServer::string_to_ObjectId(name);
    CORBA::Object_ptr ref;
	try {
		ref = poa->id_to_reference(pen_id.in());
	} catch (const PortableServer::POA::ObjectNotActive&) {
        // create new
        auto* c = new Pen_i{win->new_user()};
	    connect(c, &Pen_i::color, win, &MainWindow::set_color);
	    connect(c, &Pen_i::moveto, win, &MainWindow::move_to);
	    connect(c, &Pen_i::lineto, win, &MainWindow::line_to);
	    connect(c, &Pen_i::alineto, win, &MainWindow::aline_to);
	    poa->activate_object_with_id(pen_id.in(), c);
        ref = poa->id_to_reference(pen_id.in());
	}
	return Pen::_duplicate(Pen::_narrow(ref));
}
