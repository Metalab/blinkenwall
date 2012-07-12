/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include "helper.h"
#include "main.h"

//! [0]
Helper::Helper()
{
    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    //gradient.setColorAt(0.0, QColor(foreground_colour_r , foreground_colour_g, foreground_colour_b));
    gradient.setColorAt(0.0, Qt::white);
    //gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));
    background = QBrush(QColor(0, 0, 0));
    foreground = QBrush(gradient);
    circlePen = QPen(Qt::black);
    circlePen.setWidth(1);
    textPen = QPen(Qt::white);
    textFont.setPixelSize(50);
}
//! [0]

//! [1]
void Helper::paint(QPainter *painter, QPaintEvent *event)
{

    painter->fillRect(event->rect(), background);


//! [1]
    //painter->save();
    painter->setBrush(foreground);
    painter->setPen(circlePen);
//! [2]

    int i, x, y;
    QBrush foreground_tmp;

    i=0;
    for (y=0; y<WALL_HEIGHT; ++y) {
        for (x=0; x<WALL_WIDTH; ++x) {

            foreground_tmp = QBrush(QColor(Framebuffer[i*3], Framebuffer[i*3+1], Framebuffer[i*3+2]));
            //qDebug("i= %d", i);
            painter->setBrush(foreground_tmp);
            painter->drawRect(x*DRAW_MULTIPLICATOR,y*DRAW_MULTIPLICATOR,DRAW_MULTIPLICATOR, DRAW_MULTIPLICATOR);
            i++;
        }
    }


}

