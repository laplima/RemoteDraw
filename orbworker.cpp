#include "orbworker.h"
#include <memory>
#include <colibry/ORBManager.h>
#include "orbworker.h"

ORBWorker::ORBWorker(colibry::ORBManager& om, QObject *parent)
    : QThread(parent), m_om{om}
{
}

ORBWorker::~ORBWorker()
{
}

void ORBWorker::run()
{
    m_om.run();
    emit orbfinished();
}
