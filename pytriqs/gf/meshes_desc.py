from wrap_generator import *
import re

module = module_(full_name = "pytriqs.gf.meshes", doc = "All the meshes", app_name="triqs")
module.use_module('lattice_tools')
module.add_include("<triqs/gfs.hpp>")
module.add_include("<triqs/python_tools/converters/string.hpp>")
module.add_include("<triqs/python_tools/converters/arrays.hpp>")
module.add_include("<triqs/python_tools/converters/h5.hpp>")
module.add_include("<triqs/python_tools/converters/vector.hpp>")
module.add_include("<triqs/python_tools/converters/function.hpp>")
module.add_include("<triqs/python_tools/converters/gf.hpp>")
module.add_using("namespace triqs::arrays")
module.add_using("namespace triqs::gfs")
module.add_using("triqs::utility::mini_vector")
module.add_preamble("""
""")

## --------------------------------------------------
##                    WARNING
# The names of the Meshes MUST be MeshXXX, with XXX = ImFreq, ...
# where XXX is the name appearing in hdf5 in multivar mesh (gf/h5.hpp, gf_h5_name trait).
## --------------------------------------------------


########################
##   enums
########################

module.add_enum(c_name = "statistic_enum",
                c_namespace = "triqs::gfs",
                values = ["Fermion","Boson"])
module.add_enum(c_name = "matsubara_mesh_opt",
                c_namespace = "triqs::gfs",
                values = ["all_frequencies","positive_frequencies_only"])

########################
##   Mesh generic
########################

def make_mesh(py_type, c_tag, index_type='long'):

    m = class_( py_type = py_type,
            c_type = "gf_mesh<%s>"%c_tag,
            c_type_absolute = "triqs::gfs::gf_mesh<triqs::gfs::%s>"%c_tag,
            hdf5 = True,
            serializable= "tuple",
            is_printable= True,
            comparisons = "== !="
           )

    m.add_method("long index_to_linear(%s i)"%index_type, doc = "index -> linear index")
    m.add_len(calling_pattern = "int result = self_c.size()", doc = "Size of the mesh")
    #c_cast_type = "dcomplex" if not (c_tag == "brillouin_zone" or c_tag=="cyclic_lattice") else "triqs::arrays::vector<double>"
    #m.add_iterator(c_cast_type = c_cast_type)
    m.add_iterator() #c_cast_type = c_cast_type)

    #m.add_property("values"
    m.add_method("PyObject * values()", 
                calling_pattern = """
                    auto cls= pyref::get_class("pytriqs.gf", "MeshValueGenerator", /* raise_exception */ true);
                    pyref kw = PyDict_New();
                    pyref args = PyTuple_Pack(1, self);
                    auto result = PyObject_Call(cls, args, kw)
                    """, doc = "A numpy array of all the values of the mesh points")
    
    m.add_method_copy()
    m.add_method_copy_from()

    return m

########################
##   MeshImFreq
########################

m = make_mesh( py_type = "MeshImFreq", c_tag = "imfreq")
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max=1025)")
m.add_method("""int last_index()""")
m.add_method("""int first_index()""")
m.add_method("""bool positive_only()""")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.domain().beta",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
               doc = "Statistic")

module.add_class(m)

########################
##   MeshImTime
########################

m = make_mesh(py_type = "MeshImTime", c_tag = "imtime")
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.domain().beta",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
               doc = "Statistic")

module.add_class(m)

########################
##   MeshLegendre
########################


# the domain
dom = class_( py_type = "GfLegendreDomain",
        c_type = "legendre_domain",
        c_type_absolute = "triqs::gfs::legendre_domain",
        serializable= "tuple",
       )
dom.add_constructor(signature = "(double beta, statistic_enum S, int n_max)")
module.add_class(dom)

# the mesh
m = make_mesh( py_type = "MeshLegendre", c_tag = "legendre")
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max=1025)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.domain().beta",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
               doc = "Statistic")

module.add_class(m)

########################
##   MeshReFreq
########################

m = make_mesh(py_type = "MeshReFreq", c_tag = "refreq")
m.add_constructor(signature = "(double omega_min, double omega_max, int n_max)")

m.add_property(name = "omega_min",
               getter = cfunction(calling_pattern="double result = self_c.x_min()",
               signature = "double()",
               doc = "Inverse temperature"))

m.add_property(name = "omega_max",
               getter = cfunction(calling_pattern="double result = self_c.x_max()",
               signature = "double()",
               doc = "Inverse temperature"))

module.add_class(m)

########################
##   MeshReTime
########################

m = make_mesh(py_type = "MeshReTime", c_tag = "retime")
m.add_constructor(signature = "(double t_min, double t_max, int n_max)")

m.add_property(name = "t_min",
               getter = cfunction(calling_pattern="double result = self_c.x_min()",
               signature = "double()",
               doc = "Inverse temperature"))

m.add_property(name = "t_max",
               getter = cfunction(calling_pattern="double result = self_c.x_max()",
               signature = "double()",
               doc = "Inverse temperature"))

module.add_class(m)

########################
##   MeshBrillouinZone
########################

m = make_mesh( py_type = "MeshBrillouinZone", c_tag = "brillouin_zone", index_type = 'triqs::utility::mini_vector<int,3>' )
m.add_constructor(signature = "(triqs::lattice::brillouin_zone b, int n_k)")
m.add_constructor(signature = "(triqs::lattice::brillouin_zone b, matrix_view<int> periodization_matrix)")
m.add_method(name="locate_neighbours", signature="triqs::utility::mini_vector<int,3> locate_neighbours(triqs::arrays::vector<double> x)")
m.add_method(name="index_to_linear", signature="long index_to_linear(triqs::utility::mini_vector<int,3> x)")
module.add_class(m)

########################
##   MeshCyclicLattice
########################

m = make_mesh( py_type = "MeshCyclicLattice", c_tag = "cyclic_lattice", index_type = 'triqs::utility::mini_vector<int,3>' )
#m.add_constructor(signature = "(triqs::gfs::cyclic_lattice b, int n_Rx, int n_Ry, int n_Rz)")
m.add_constructor(signature = "(triqs::lattice::bravais_lattice b, matrix_view<int> periodization_matrix)")
#m.add_method(name="locate_neighbours", signature="triqs::utility::mini_vector<int,3> locate_neighbours(triqs::arrays::vector<double> x)")
#m.add_method(name="index_to_linear", signature="long index_to_linear(triqs::utility::mini_vector<int,3> x)")
module.add_class(m)


##   Code generation
module.generate_code()

