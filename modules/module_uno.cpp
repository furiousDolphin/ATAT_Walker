
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include "System.hpp"
#include "ValueManager.hpp"

namespace py = pybind11;

PYBIND11_MODULE(module_uno, m) 
{
     m.doc() = "";
    
     py::class_<System>(m, "System")
          .def(py::init<Eigen::VectorXd, Eigen::VectorXd>(),
               py::arg("b_coeffs"),
               py::arg("a_coeffs"))
          .def("step_response", 
               &System::step_response, 
               "oblicza przebieg odpowiedzi skokowej dla podanego wektora czasu",
               py::arg("t_dense"))
          .def("impulse_response", 
               &System::impulse_response, 
               "oblicza przebieg odpowiedzi impulsowej dla podanego wektora czasu",
               py::arg("t_dense"))
          .def("set_forcing_func", 
               &System::set_forcing_func, 
               "",
               py::arg("func") )
          .def("do_RK4_step", 
               &System::do_RK4_step, 
               "",
               py::arg("dt") );

     py::class_<SecondOrderSystem, System> sos(m, "SecondOrderSystem");

     sos.def(py::init<>())
          .def("get_params", &SecondOrderSystem::get_params, py::return_value_policy::reference_internal);

     py::class_<SecondOrderSystem::Params>(sos, "Params")
          .def_readonly("zeta", &SecondOrderSystem::Params::zeta)
          .def_readonly("f", &SecondOrderSystem::Params::f)
          .def_readonly("r", &SecondOrderSystem::Params::r)
          .def("__getitem__", [](SecondOrderSystem::Params &p, size_t i) -> ValueManager& {
               if (i == 0) return p.zeta;
               if (i == 1) return p.r;
               if (i == 2) return p.f;
               throw py::index_error();
          }, py::return_value_policy::reference_internal);
               

     py::class_<ValueManager>(m, "ValueManager")
          .def(py::init<>())
          .def("get_val",
               &ValueManager::get_val,
               "")
          .def("set_val",
               &ValueManager::set_val,
               "",
               py::arg("new_val"))
          .def("check_and_reset_dirty",
               &ValueManager::check_and_reset_dirty,
               "")
          .def_readwrite("getter", &ValueManager::getter)
          .def_readwrite("setter", &ValueManager::setter);
}