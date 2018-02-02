/***************************************************************************
 *   file klfpreviewbuilderthread.cpp
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
/* $Id: klfpreviewbuilderthread.cpp 603 2011-02-26 23:14:55Z phfaist $ */

#include "klfpreviewbuilderthread.h"

KLFPreviewBuilderThread::KLFPreviewBuilderThread(QObject *parent, KLFBackend::klfInput input,
                         KLFBackend::klfSettings settings)
  : QThread(parent), _input(input), _settings(settings), _hasnewinfo(false), _abort(false)
{
}
KLFPreviewBuilderThread::~KLFPreviewBuilderThread()
{
  _mutex.lock();
  _abort = true;
  _condnewinfoavail.wakeOne();
  _mutex.unlock();
  wait();
}

void KLFPreviewBuilderThread::run()
{
  KLFBackend::klfInput input;
  KLFBackend::klfSettings settings;
  KLFBackend::klfOutput output;
  QImage img;

  for (;;) {
    _mutex.lock();
    bool abrt = _abort;
    _mutex.unlock();
    if (abrt)
      return;

    // fetch info
    _mutex.lock();
    input = _input;
    settings = _settings;
    settings.epstopdfexec = "";
    _hasnewinfo = false;
    _mutex.unlock();
    // render equation
    //  no performance improvement noticed with lower DPI:
    //    // force 240 DPI (we're only a preview...)
    //    input.dpi = 240;

    if ( input.latex.trimmed().isEmpty() ) {
      emit previewAvailable(QImage(), 0);
    } else {
      // and GO!
      output = KLFBackend::getLatexFormula(input, settings);
      img = output.result;
      if (!output.status == 0)
       img = QImage();
    }

    _mutex.lock();
    bool abort = _abort;
    bool hasnewinfo = _hasnewinfo;
    _mutex.unlock();

    if (abort)
      return;
    if (hasnewinfo)
      continue;

    emit previewAvailable(img, output.status != 0);

    _mutex.lock();
    _condnewinfoavail.wait(&_mutex);
    _mutex.unlock();
  }
}
bool KLFPreviewBuilderThread::inputChanged(const KLFBackend::klfInput& input)
{
  QMutexLocker mutexlocker(&_mutex);
  if (_input == input) {
    return false;
  }
  _input = input;
  _hasnewinfo = true;
  _condnewinfoavail.wakeOne();
  return true;
}
void KLFPreviewBuilderThread::settingsChanged(const KLFBackend::klfSettings& settings)
{
  _mutex.lock();
  _settings = settings;
  _hasnewinfo = true;
  _condnewinfoavail.wakeOne();
  _mutex.unlock();
}
