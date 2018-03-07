/***************************************************************************
 *   file klfpreviewbuilderthread.h
 *   This file is part of a file that was part of the KLatexFormula Project.
 *   Copyright (C) 2011 by Philippe Faist
 *   philippe.faist at bluewin.ch
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/* $Id: klfpreviewbuilderthread.h 603 2011-02-26 23:14:55Z phfaist $ */

#ifndef KLFPREVIEWBUILDERTHREAD_H
#define KLFPREVIEWBUILDERTHREAD_H

#include "klfbackend.h"
#include "klfdefs.h"
#include <QThread>
#include <QWaitCondition>

/**
 * A helper that runs in a different thread that generates previews in real-time as user types text,
 * without blocking the GUI.
 */
class KLF_EXPORT KLFPreviewBuilderThread : public QThread
{
  Q_OBJECT

public:
  KLFPreviewBuilderThread(QObject *parent, KLFBackend::klfInput input, KLFBackend::klfSettings settings);
  virtual ~KLFPreviewBuilderThread();
  void run();

signals:
  void previewAvailable(const QImage& preview, bool latexerror);

public slots:
  bool inputChanged(const KLFBackend::klfInput& input);
  void settingsChanged(const KLFBackend::klfSettings& settings);

protected:
  KLFBackend::klfInput _input;
  KLFBackend::klfSettings _settings;

  QMutex _mutex;
  QWaitCondition _condnewinfoavail;

  bool _hasnewinfo;
  bool _abort;
};

#endif // KLFPREVIEWBUILDERTHREAD_H
