/*
 * Copyright (c) 2003-2009, John Wiegley.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of New Artisans LLC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pyinterp.h"
#include "pyutils.h"
#include "amount.h"

#include <boost/python/exception_translator.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/args.hpp>

namespace ledger {

using namespace boost::python;

#ifdef INTEGER_MATH
amount_t py_round_0(const amount_t& amount) {
  return amount.round();
}
amount_t py_round_1(const amount_t& amount, amount_t::precision_t prec) {
  return amount.round(prec);
}
#endif

double py_to_double_0(amount_t& amount) {
  return amount.to_double();
}
double py_to_double_1(amount_t& amount, bool no_check) {
  return amount.to_double(no_check);
}

long py_to_long_0(amount_t& amount) {
  return amount.to_long();
}
long py_to_long_1(amount_t& amount, bool no_check) {
  return amount.to_long(no_check);
}

boost::optional<amount_t> py_value_0(const amount_t& amount) {
  return amount.value();
}
boost::optional<amount_t> py_value_1(const amount_t& amount,
				     const boost::optional<datetime_t>& moment) {
  return amount.value(moment);
}
boost::optional<amount_t> py_value_2(const amount_t& amount,
				     const boost::optional<datetime_t>& moment,
				     const boost::optional<commodity_t&>& in_terms_of) {
  return amount.value(moment, in_terms_of);
}

void py_parse_2(amount_t& amount, object in, unsigned char flags) {
  if (PyFile_Check(in.ptr())) {
    pyifstream instr(reinterpret_cast<PyFileObject *>(in.ptr()));
    amount.parse(instr, flags);
  } else {
    PyErr_SetString(PyExc_IOError,
		    "Argument to amount.parse(file) is not a file object");
  }
}
void py_parse_1(amount_t& amount, object in) {
  py_parse_2(amount, in, 0);
}

void py_parse_str_1(amount_t& amount, const string& str) {
  amount.parse(str);
}
void py_parse_str_2(amount_t& amount, const string& str, unsigned char flags) {
  amount.parse(str, flags);
}

void py_print(amount_t& amount, object out) {
  if (PyFile_Check(out.ptr())) {
    pyofstream outstr(reinterpret_cast<PyFileObject *>(out.ptr()));
    amount.print(outstr);
  } else {
    PyErr_SetString(PyExc_IOError,
		    "Argument to amount.print_(file) is not a file object");
  }
}

void py_read_1(amount_t& amount, object in) {
  if (PyFile_Check(in.ptr())) {
    pyifstream instr(reinterpret_cast<PyFileObject *>(in.ptr()));
    amount.read(instr);
  } else {
    PyErr_SetString(PyExc_IOError,
		    "Argument to amount.read(file) is not a file object");
  }
}
void py_read_2(amount_t& amount, const std::string& str) {
  const char * p = str.c_str();
  amount.read(p);
}

void py_write_xml_1(amount_t& amount, object out) {
  if (PyFile_Check(out.ptr())) {
    pyofstream outstr(reinterpret_cast<PyFileObject *>(out.ptr()));
    amount.write_xml(outstr);
  } else {
    PyErr_SetString(PyExc_IOError,
		    "Argument to amount.write_xml(file) is not a file object");
  }
}
  void py_write_xml_2(amount_t& amount, object out, const int depth) {
  if (PyFile_Check(out.ptr())) {
    pyofstream outstr(reinterpret_cast<PyFileObject *>(out.ptr()));
    amount.write_xml(outstr, depth);
  } else {
    PyErr_SetString(PyExc_IOError,
		    "Argument to amount.write_xml(file, depth) is not a file object");
  }
}

#define EXC_TRANSLATOR(type)				\
  void exc_translate_ ## type(const type& err) {	\
    PyErr_SetString(PyExc_ArithmeticError, err.what());	\
  }

EXC_TRANSLATOR(amount_error)

void export_amount()
{
  class_< amount_t > ("Amount")
    .def("initialize", &amount_t::initialize)
    .staticmethod("initialize")
    .def("shutdown", &amount_t::shutdown)
    .staticmethod("shutdown")

    .add_static_property("current_pool",
			 make_getter(&amount_t::current_pool,
				     return_value_policy<reference_existing_object>()))

    .add_static_property("keep_base", &amount_t::keep_base)

    .add_static_property("keep_price", &amount_t::keep_price)
    .add_static_property("keep_date", &amount_t::keep_date)
    .add_static_property("keep_tag", &amount_t::keep_tag)

    .add_static_property("stream_fullstrings",
			 make_getter(&amount_t::stream_fullstrings),
			 make_setter(&amount_t::stream_fullstrings))

    .def(init<double>())
    .def(init<long>())
    .def(init<std::string>())

    .def("exact", &amount_t::exact, args("value"),
	 "Construct an amount object whose display precision is always equal to its\n\
internal precision.")
    .staticmethod("exact")

    .def(init<amount_t>())

    .def("compare", &amount_t::compare)

    .def(self == self)
    .def(self == long())
    .def(long() == self)
    .def(self == double())
    .def(double() == self)

    .def(self != self)
    .def(self != long())
    .def(long() != self)
    .def(self != double())
    .def(double() != self)

    .def(! self)

    .def(self <  self)
    .def(self <  long())
    .def(long() < self)
    .def(self <  double())
    .def(double() < self)

    .def(self <= self)
    .def(self <= long())
    .def(long() <= self)
    .def(self <= double())
    .def(double() <= self)

    .def(self >  self)
    .def(self >  long())
    .def(long() > self)
    .def(self >  double())
    .def(double() > self)

    .def(self >= self)
    .def(self >= long())
    .def(long() >= self)
    .def(self >= double())
    .def(double() >= self)

    .def(self += self)
    .def(self += long())
    .def(self += double())

    .def(self	  + self)
    .def(self	  + long())
    .def(long()	  + self)
    .def(self	  + double())
    .def(double() + self)

    .def(self -= self)
    .def(self -= long())
    .def(self -= double())

    .def(self	  - self)
    .def(self	  - long())
    .def(long()	  - self)
    .def(self	  - double())
    .def(double() - self)

    .def(self *= self)
    .def(self *= long())
    .def(self *= double())

    .def(self	  * self)
    .def(self	  * long())
    .def(long()	  * self)
    .def(self	  * double())
    .def(double() * self)

    .def(self /= self)
    .def(self /= long())
    .def(self /= double())

    .def(self	  /  self)
    .def(self	  /  long())
    .def(long()	  / self)
    .def(self	  /  double())
    .def(double() / self)

    .def("precision", &amount_t::precision)

    .def("negate", &amount_t::negate)
    .def("in_place_negate", &amount_t::in_place_negate,
	 return_value_policy<reference_existing_object>())
    .def(- self)

    .def("abs", &amount_t::abs)
    .def("__abs__", &amount_t::abs)

#ifdef INTEGER_MATH
    .def("round", py_round_0)
    .def("round", py_round_1)
#endif
    .def("unround", &amount_t::unround)

    .def("reduce", &amount_t::reduce)
    .def("in_place_reduce", &amount_t::in_place_reduce,
	 return_value_policy<reference_existing_object>())

    .def("unreduce", &amount_t::unreduce)
    .def("in_place_unreduce", &amount_t::in_place_unreduce,
	 return_value_policy<reference_existing_object>())

    .def("value", py_value_0)
    .def("value", py_value_1)
    .def("value", py_value_2)

    .def("sign", &amount_t::sign)
    .def("__nonzero__", &amount_t::is_nonzero)
    .def("is_nonzero", &amount_t::is_nonzero)
    .def("is_zero", &amount_t::is_zero)
    .def("is_realzero", &amount_t::is_realzero)
    .def("is_null", &amount_t::is_null)

    .def("to_double", py_to_double_0)
    .def("to_double", py_to_double_1)
    .def("__float__", py_to_double_0)
    .def("to_long", py_to_long_0)
    .def("to_long", py_to_long_1)
    .def("__int__", py_to_long_0)
    .def("to_string", &amount_t::to_string)
    .def("__str__", &amount_t::to_string)
    .def("to_fullstring", &amount_t::to_fullstring)
    .def("__repr__", &amount_t::to_fullstring)

    .def("fits_in_double", &amount_t::fits_in_double)
    .def("fits_in_long", &amount_t::fits_in_long)

    .def("quantity_string", &amount_t::quantity_string)

    .def("commodity", &amount_t::commodity,
	 return_value_policy<reference_existing_object>())
    .def("set_commodity", &amount_t::set_commodity,
	 with_custodian_and_ward<1, 2>())

    .def("has_commodity", &amount_t::has_commodity)
    .def("clear_commodity", &amount_t::clear_commodity)
    .def("number", &amount_t::number)

    .def("annotate", &amount_t::annotate)
    .def("is_annotated", &amount_t::is_annotated)
#if 0
    .def("annotation", &amount_t::annotation)
#endif
    .def("strip_annotations", &amount_t::strip_annotations)

    .def("parse", py_parse_1)
    .def("parse", py_parse_2)
    .def("parse", py_parse_str_1)
    .def("parse", py_parse_str_2)

    .def("parse_conversion", &amount_t::parse_conversion)
    .staticmethod("parse_conversion")

    .def("print_", py_print)

    .def("read", py_read_1)
    .def("read", py_read_2)
    .def("write", &amount_t::write)

    .def("read_xml", &amount_t::read_xml)
    .def("write_xml", py_write_xml_1)
    .def("write_xml", py_write_xml_2)

    .def("dump", &amount_t::dump)

    .def("valid", &amount_t::valid)
    ;

  enum_< amount_t::parse_flags_enum_t >("AmountParse")
    .value("DEFAULT",    amount_t::PARSE_DEFAULT)
    .value("NO_MIGRATE", amount_t::PARSE_NO_MIGRATE)
    .value("NO_REDUCE",  amount_t::PARSE_NO_REDUCE)
    .value("SOFT_FAIL",  amount_t::PARSE_SOFT_FAIL)
    ;

  register_optional_to_python<amount_t>();

  implicitly_convertible<double, amount_t>();
  implicitly_convertible<long, amount_t>();
  implicitly_convertible<string, amount_t>();

#define EXC_TRANSLATE(type) \
  register_exception_translator<type>(&exc_translate_ ## type);

  EXC_TRANSLATE(amount_error);
}

} // namespace ledger
