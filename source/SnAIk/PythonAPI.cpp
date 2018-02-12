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
		static std::shared_ptr<API> getSharedAPIInstance()
		{
			return std::shared_ptr<API>(&API::getInstance(), NullDeleter());
		}

		BOOST_PYTHON_MODULE(SnAIk)
		{
			class_<API, std::shared_ptr<API>, boost::noncopyable>("API", no_init)
				.def("getInstance", getSharedAPIInstance)
				.staticmethod("getInstance")

				.def("getMove", &API::getMove)
				.def("setMove", &API::setMove)
				.def("getSnake", &API::getSnake)
				.def("setSnake", &API::setSnake)
				.def("isMoveAvailable", &API::isMoveAvailable)
				.def("isSnakeAvailable", &API::isSnakeAvailable)
				.def("runSimulation", &API::runSimulation)
				.def("initSim", &API::initSim)
				.def("step", &API::step)
				;

			class_<SnakeMove>("SnakeMove")
				.def_readwrite("segment", &SnakeMove::mSegment)
				.def_readwrite("torque", &SnakeMove::mTorque)
				.def_readwrite("direction", &SnakeMove::mDirection)
				;

			class_<SnakeSnapshot>("SnakeSnapshot")
				.def_readwrite("segmentsNo", &SnakeSnapshot::mSegmentsNo)
				.def_readwrite("segments", &SnakeSnapshot::mSegments)
				.def_readwrite("averagePosition", &SnakeSnapshot::mAveragePosition)
				;

			class_<SegmentSnapshot>("SegmentSnapshot")
				.def_readwrite("rotation", &SegmentSnapshot::mRotation)
				.def_readwrite("position", &SegmentSnapshot::mPosition)
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

#include "libs/python/test/module_tail.cpp"