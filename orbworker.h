#ifndef ORBWORKER_H
#define ORBWORKER_H

#include <QThread>
#include <memory>
#include <colibry/ORBManager.h>

class ORBWorker : public QThread {
    Q_OBJECT
public:
    explicit ORBWorker(colibry::ORBManager& om, QObject *parent = nullptr);
    virtual ~ORBWorker();
signals:
    void orbfinished();
protected:
    void run() override;
private:
    colibry::ORBManager& m_om;
};

#endif // ORBWORKER_H
