/*
    SPDX-FileCopyrightText: 2009-2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "breakcontrol.h"

#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <QHBoxLayout>
#include <qboxlayout.h>
#include <qdebug.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qrangemodeladapter.h>

void clearLayout(QLayout *);

// initial values for static variables must be done in separate cpp file
bool BreakControl::hideSkipButton = false;
bool BreakControl::hideLockButton = false;
bool BreakControl::hidePostponeButton = false;

BreakControl::BreakControl(QWidget *parent, Qt::WindowType type)
    : QWidget(parent, type)
{
    // BreakControl is rebuilt every time the config is updated
    m_vbox = new QVBoxLayout;
    m_textLabel = new QLabel(this);
    m_textLabel->setAlignment(Qt::AlignHCenter);

    QWidget *hbox = new QWidget(this);
    QHBoxLayout *hboxHBoxLayout = new QHBoxLayout(hbox);
    hboxHBoxLayout->setContentsMargins(0, 0, 0, 0);
    hboxHBoxLayout->setSpacing(25);

    m_skipButton = new QPushButton(i18n("Skip Break"), hbox);
    hboxHBoxLayout->addWidget(m_skipButton);
    QSize sizeSkip(m_skipButton->size());
    m_skipButton->setIcon(QIcon::fromTheme("dialog-cancel"));
    m_skipButton->setFixedHeight(sizeSkip.height());
    connect(m_skipButton, &QPushButton::clicked, this, &BreakControl::skip);

    m_lockButton = new QPushButton(i18n("Lock Screen"), hbox);
    hboxHBoxLayout->addWidget(m_lockButton);
    QSize sizeLock(m_lockButton->size());
    m_lockButton->setFixedHeight(sizeLock.height());
    m_lockButton->setIcon(QIcon::fromTheme("system-lock-screen"));
    connect(m_lockButton, &QPushButton::clicked, this, &BreakControl::slotLock);

    m_postponeButton = new QPushButton(i18n("Postpone Break"), hbox);
    hboxHBoxLayout->addWidget(m_postponeButton);
    QSize sizePostpone(m_postponeButton->size());
    m_postponeButton->setIcon(QIcon::fromTheme("go-next"));
    m_postponeButton->setFixedHeight(sizePostpone.height());
    connect(m_postponeButton, &QPushButton::clicked, this, &BreakControl::postpone);

    if (BreakControl::hideSkipButton) {
        m_skipButton->hide();
    } else {
        m_skipButton->show();
    }

    if (BreakControl::hideLockButton) {
        m_lockButton->hide();
    } else {
        m_lockButton->show();
    }

    if (BreakControl::hidePostponeButton) {
        m_postponeButton->hide();
    } else {
        m_postponeButton->show();
    }

    m_vbox->addWidget(m_textLabel);
    m_vbox->addWidget(hbox);

    setLayout(m_vbox);

    // these two are for screen resizing
    connect(qApp, &QGuiApplication::screenAdded, this, &BreakControl::slotCenterIt);
    connect(qApp, &QGuiApplication::screenRemoved, this, &BreakControl::slotCenterIt);

    slotCenterIt();
}

void BreakControl::updateButtonState(bool hide_skip, bool hide_lock, bool hide_postpone)
{
    BreakControl::hideSkipButton = hide_skip;
    BreakControl::hideLockButton = hide_lock;
    BreakControl::hidePostponeButton = hide_postpone;
}

void BreakControl::slotCenterIt()
{
    const QRect r(QGuiApplication::primaryScreen()->geometry());
    // top center is better for slideshow mode and such
    const QPoint top_center(r.width() / 2 - sizeHint().width() / 2, r.top());
    resize(minimumSize());
    m_textLabel->setAlignment(Qt::AlignCenter);
    move(top_center);
}

void BreakControl::slotLock()
{
    emit lock();
}

void BreakControl::setText(const QString &text)
{
    m_textLabel->setText(text);
}

void BreakControl::paintEvent(QPaintEvent *event)
{
    if (event->type() == QEvent::Paint) {
        // this all draws the blue border around the remaining break time widget

        int margin = 3;
        QPainterPath box;
        box.moveTo(rect().topLeft());
        box.lineTo(rect().bottomLeft());
        box.lineTo(rect().bottomRight());
        box.lineTo(rect().topRight());
        box.closeSubpath();

        QColor highlight = palette().highlight().color();
        highlight.setAlphaF(0.7);

        QPen pen(highlight);
        pen.setWidth(margin);

        QPainter painter(this);
        painter.setPen(pen);
        painter.drawPath(box);
    }
}
