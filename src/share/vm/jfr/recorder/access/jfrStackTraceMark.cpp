/*
 * Copyright (c) 2011, 2019, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#include "precompiled.hpp"
#include "jfr/recorder/jfrEventSetting.inline.hpp"
#include "jfr/recorder/jfrEventSetting.hpp"
#include "jfr/recorder/access/jfrStackTraceMark.hpp"
#include "jfr/recorder/access/jfrThreadData.hpp"
#include "jfr/recorder/stacktrace/jfrStackTraceRepository.hpp"
#include "runtime/thread.inline.hpp"

JfrStackTraceMark::JfrStackTraceMark() : _t(Thread::current()), _previous_id(0), _previous_hash(0) {
  JfrThreadData* const trace_data = _t->trace_data();
  if (trace_data->has_cached_stack_trace()) {
    _previous_id = trace_data->cached_stack_trace_id();
    _previous_hash = trace_data->cached_stack_trace_hash();
  }
  trace_data->set_cached_stack_trace_id(JfrStackTraceRepository::record(Thread::current(), 0, WALK_BY_DEFAULT));
}

JfrStackTraceMark::JfrStackTraceMark(Thread* t) : _t(t), _previous_id(0), _previous_hash(0) {
  JfrThreadData* const trace_data = _t->trace_data();
  if (trace_data->has_cached_stack_trace()) {
    _previous_id = trace_data->cached_stack_trace_id();
    _previous_hash = trace_data->cached_stack_trace_hash();
  }
  trace_data->set_cached_stack_trace_id(JfrStackTraceRepository::record(t, 0, WALK_BY_DEFAULT));
}

JfrStackTraceMark::JfrStackTraceMark(TraceEventId eventId) : _t(NULL), _previous_id(0), _previous_hash(0) {
  if (JfrEventSetting::has_stacktrace(eventId)) {
    _t = Thread::current();
    JfrThreadData* const trace_data = _t->trace_data();
    if (trace_data->has_cached_stack_trace()) {
      _previous_id = trace_data->cached_stack_trace_id();
      _previous_hash = trace_data->cached_stack_trace_hash();
    }
    StackWalkMode mode = JfrEventSetting::stack_walk_mode(eventId);
    trace_data->set_cached_stack_trace_id(JfrStackTraceRepository::record(_t, 0, mode));
  }
}

JfrStackTraceMark::JfrStackTraceMark(TraceEventId eventId, Thread* t) : _t(NULL), _previous_id(0), _previous_hash(0) {
  if (JfrEventSetting::has_stacktrace(eventId)) {
    _t = t;
    JfrThreadData* const trace_data = _t->trace_data();
    if (trace_data->has_cached_stack_trace()) {
      _previous_id = trace_data->cached_stack_trace_id();
      _previous_hash = trace_data->cached_stack_trace_hash();
    }
    StackWalkMode mode = JfrEventSetting::stack_walk_mode(eventId);
    trace_data->set_cached_stack_trace_id(JfrStackTraceRepository::record(_t, 0, mode));
  }
}

JfrStackTraceMark::~JfrStackTraceMark() {
  if (_previous_id != 0) {
    _t->trace_data()->set_cached_stack_trace_id(_previous_id, _previous_hash);
  } else {
    if (_t != NULL) {
      _t->trace_data()->clear_cached_stack_trace();
    }
  }
}
