#pragma once

#include "API.hpp"
#include <forward_list>
#include "boost/python.hpp"

using SnakeMove = API::SnakeMoveStruct;
using SnakeSnapshot = API::SnakeSnapshotStruct;
using SegmentSnapshot = API::SegmentSnapshotStruct;
namespace boost {
	namespace python {

		BOOST_PYTHON_MODULE(simulator)
		{
			class_<API, std::shared_ptr<API>, boost::noncopyable>("Simulator", no_init)
				.def("getInstance", &getSharedAPIInstance)
				.staticmethod("getInstance")

				.def("getMove", &API::getMove)
				.def("setMove", &API::setMove)
				.def("getSnake", &API::getSnake)
				.def("setSnake", &API::setSnake)
				.def("isMoveAvailable", &API::isMoveAvailable)
				.def("isSnakeAvailable", &API::isSnakeAvailable)
				;

			class_<SnakeMove>("SnakeMove")
				.def_readwrite("segment", &API::SnakeMoveStruct.mSegment)
				.def_readwrite("torque", &SnakeMove.mTorque)
				.def_readwrite("direction", &SnakeMove.mDirection)
				;
			/*
				class_<SnakeSnapshot>("SnakeSnapshot")
					.def_readonly("segmentsNo", &SnakeSnapshot.mSegmentsNo)
					.def_readonly("segments", &SnakeSnapshot.mSegments)
					.def_readonly("averagePosition", &SnakeSnapshot.mAveragePosition)
				;

				class_<SegmentSnapshot>("SegmentSnapshot")
					.def_readonly("rotation", &SegmentSnapshot.mRotation)
					.def_readonly("position", &SegmentSnapshot.mPosition)
				;
			*/
			//def("toList", toList, "convert std::forward_list<SegmentSnapshot> to python list");
			to_python_converter<SegmentSnapshot, SegmentSnapshot_to_tuple>();
			to_python_converter<SnakeSnapshot, SnakeSnapshot_to_tuple>();
		}




		// to-python convert for SegmentSnapshot
		struct SegmentSnapshot_to_tuple
		{
			static PyObject* convert(SegmentSnapshot const& s)
			{
				return incref(object(make_tuple(s.mRotation, s.mPosition)).ptr());
			}
		};

		// to-python convert for SegmentSnapshot
		struct SnakeSnapshot_to_tuple
		{
			static PyObject* convert(SnakeSnapshot const& s)
			{
				return incref(object(make_tuple(s.mSegmentsNo, s.mSegments, m.mAveragePosition)).ptr());
			}
		};

		static list toList(std::forward_list<SegmentSnapshot> const& self)
		{
			list t;
			for (const auto it = self.begin(); it != self.end(); ++it)
				t.append(*it);
			return t;
		}

		static std::shared_ptr<API> getSharedAPIInstance()
		{
			return std::shared_ptr<API>(&API::getInstance(), NullDeleter());
		}

	} // namespace boost::python
}