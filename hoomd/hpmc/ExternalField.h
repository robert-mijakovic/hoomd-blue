// Copyright (c) 2009-2022 The Regents of the University of Michigan.
// Part of HOOMD-blue, released under the BSD 3-Clause License.

// inclusion guard
#ifndef _EXTERNAL_FIELD_H_
#define _EXTERNAL_FIELD_H_

/*! \file ExternalField.h
    \brief Declaration of ExternalField base class
*/

#include "hoomd/Compute.h"
#include "hoomd/VectorMath.h"

#include "HPMCCounters.h" // do we need this to keep track of the statistics?

#ifndef __HIPCC__
#include <pybind11/pybind11.h>
#endif

namespace hoomd
    {
namespace hpmc
    {
class ExternalField : public Compute
    {
    public:
    ExternalField(std::shared_ptr<SystemDefinition> sysdef) : Compute(sysdef) { }
    /*! calculateBoltzmannWeight(uint64_t timestep)
        method used to calculate the boltzmann weight contribution for the
        external field of the entire system. This is used to interface with
        global moves such as the NPT box updater. The boltzmann factor can
        be calculated by the quotient of the boltzmann weight post move and
        boltzmann weight pre move. Example:
        Scalar bw1 = external->calculateBoltzmannWeight(timestep);
        // make a move updating m_pdata and any other system info (shape, position, orientation,
       etc.) Scalar bw2 = external->calculateBoltzmannWeight(timestep); pacc = min(1, bw2/bw1);
    */
    virtual Scalar calculateBoltzmannWeight(uint64_t timestep)
        {
        return 0;
        }
    /*! Calculate deltaE for the whole system
        Used for box resizing
    */
    virtual double calculateDeltaE(uint64_t timestep,
                                   const Scalar4* const position_old,
                                   const Scalar4* const orientation_old,
                                   const BoxDim* const box_old)
        {
        return 0;
        }

    virtual bool hasVolume()
        {
        return false;
        }

    virtual Scalar getVolume()
        {
        return 0;
        }
    };
//! Compute that accepts or rejects moves according to some external field
/*! **Overview** <br>

    \ingroup hpmc_computes
*/
template<class Shape> class ExternalFieldMono : public ExternalField
    {
    public:
    ExternalFieldMono(std::shared_ptr<SystemDefinition> sysdef) : ExternalField(sysdef) { }

    ~ExternalFieldMono() { }

    //! needed for Compute. currently not used.
    virtual void compute(uint64_t timestep) { }

    //! method to calculate the energy difference for the proposed move.
    virtual double energydiff(uint64_t timestep,
                              const unsigned int& index,
                              const vec3<Scalar>& position_old,
                              const Shape& shape_old,
                              const vec3<Scalar>& position_new,
                              const Shape& shape_new)
        {
        return 0;
        }

    virtual void reset(uint64_t timestep) { }
    };

namespace detail
    {
template<class Shape> void export_ExternalFieldInterface(pybind11::module& m, std::string name)
    {
    pybind11::class_<ExternalFieldMono<Shape>, Compute, std::shared_ptr<ExternalFieldMono<Shape>>>(
        m,
        (name + "Interface").c_str())
        .def(pybind11::init<std::shared_ptr<SystemDefinition>>())
        .def("compute", &ExternalFieldMono<Shape>::compute)
        .def("energydiff", &ExternalFieldMono<Shape>::energydiff)
        .def("calculateBoltzmannWeight", &ExternalFieldMono<Shape>::calculateBoltzmannWeight)
        .def("calculateDeltaE", &ExternalFieldMono<Shape>::calculateDeltaE);
    }

    }  // end namespace detail
    }  // end namespace hpmc
    }  // end namespace hoomd
#endif // end inclusion guard
