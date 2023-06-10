#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QPixmap>
#include <vector>

struct DrawingInfo {
    QColor color;
    QPoint pos;
};

using UId = unsigned int;

class RenderArea : public QWidget {
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = nullptr);
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void clear();

    UId new_user(const DrawingInfo& di);    // creates new user with di and returns its drawing id
    void move_to(UId uid, const QPoint& a);
    void line_to(UId uid, const QPoint& a);
    void set_color(UId uid, const QColor& c);

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
signals:
public slots:
protected:
    void paintEvent(QPaintEvent* e) override;
private:
    QPixmap pixmap;
    std::vector<DrawingInfo> dusers;    // drawing users
    bool validate(UId uid) const { return uid < dusers.size(); }
    void draw_line(UId uid, const QPoint& a, const QPoint& b);
    UId default_user;   // user drawing with the mouse
};

#endif
