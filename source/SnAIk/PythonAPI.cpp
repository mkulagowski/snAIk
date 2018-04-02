#pragma once

#define BOOST_PYTHON_STATIC_LIB
#include "API.hpp"
#include "boost/python.hpp"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"

using SnakeMove = API::SnakeMoveStruct;
using SnakeSnapshot = API::SnakeSnapshotStruct;
using SegmentSnapshot = API::SegmentSnapshotStruct;
using VectStruct = API::VectStruct;

struct NullDeleter
{
	void operator()(const void*) {}
};

namespace boost {
	namespace python {
		static std::shared_ptr<API> GetSharedAPIInstance()
		{
			return std::shared_ptr<API>(&API::GetInstance(), NullDeleter());
		}

		BOOST_PYTHON_MODULE(SnAIk)
		{
			class_<API, std::shared_ptr<API>, boost::noncopyable>("API", no_init)
				.def("GetInstance", GetSharedAPIInstance)
				.staticmethod("GetInstance")
				.def("GetMove", &API::GetMove)
				.def("AddMove", &API::AddMove)
				.def("SetMove", &API::SetMove)
				.def("GetSnake", &API::GetSnake)
				.def("SetSnake", &API::SetSnake)
				.def("IsMoveAvailable", &API::IsMoveAvailable)
				.def("IsSnakeAvailable", &API::IsSnakeAvailable)
				.def("RunSimulation", &API::RunSimulation)
				.def("Init", &API::Init)
				.def("Step", &API::Step)
				;

			class_<SnakeMove>("SnakeMove")
				.def_readwrite("segment", &SnakeMove::mSegment)
				.def_readwrite("torque", &SnakeMove::mTorque)
				;

			class_<SnakeSnapshot>("SnakeSnapshot")
				.def_readwrite("segmentsNo", &SnakeSnapshot::mSegmentsNo)
				.def_readwrite("segments", &SnakeSnapshot::mSegments)
				;

			class_<SegmentSnapshot>("SegmentSnapshot")
				.def_readwrite("rotation", &SegmentSnapshot::mRotation)
				.def_readwrite("position", &SegmentSnapshot::mPosition)
				.def_readwrite("angularVelocity", &SegmentSnapshot::mAngularVelocity)
				.def_readwrite("speed", &SegmentSnapshot::mLinearSpeed)
				;

			class_<VectStruct>("VectStruct")
				.def_readwrite("x", &VectStruct::x)
				.def_readwrite("y", &VectStruct::y)
				.def_readwrite("z", &VectStruct::z)
				;

			class_<API::SegmentList>("SegmentList")
				.def(vector_indexing_suite<API::SegmentList>())
				;
		}

	} // namespace boost::python
}
