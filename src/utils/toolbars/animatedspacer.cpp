/**
 * Copyright 2009 Christopher Eby <kreed@kreed.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Arora nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "animatedspacer.h"

AnimatedSpacer::AnimatedSpacer(const QSize &size, QWidget *parent)
    : QWidget(parent)
    , m_resizing(false)
    , m_deleteLater(false)
    , m_offset(QSize(0, 0))
{
    setMinimumSize(size);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

static QSize subtract(const QSize &a, const QSize &b)
{
    QSize size = a - b;
    if (size.width() < 0)
        size.setWidth(0);
    if (size.height() < 0)
        size.setHeight(0);
    return size;
}

static QSize interpolate(const QSize &start, const QSize &end, qreal percent)
{
    return QSize(start.width() + int((end.width() - start.width()) * percent)
               , start.height() + int((end.height() - start.height()) * percent));
}

bool AnimatedSpacer::step()
{
    if (!m_resizing)
        return false;

    if (qFuzzyCompare(m_percent, 1)) {
        m_resizing = false;
        setMinimumSize(subtract(m_endSize, m_offset));
        m_offset = QSize(0, 0);
        if (m_deleteLater)
            deleteLater();
        return false;
    }

    setMinimumSize(subtract(interpolate(m_startSize, m_endSize, m_percent), m_offset));

    m_percent += 0.1;

    return true;
}

void AnimatedSpacer::resize(const QSize &size)
{
    m_percent = 0;
    m_startSize = minimumSize();
    m_endSize = size;
    m_resizing = true;
}

void AnimatedSpacer::setOffset(const QSize &size)
{
    m_offset = size;
    setMinimumSize(subtract(minimumSize(), size));
}

void AnimatedSpacer::remove()
{
    m_deleteLater = true;
    resize(QSize(0, 0));
}
