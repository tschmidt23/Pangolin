/* This file is part of the Pangolin Project.
 * http://github.com/stevenlovegrove/Pangolin
 *
 * Copyright (c) Andrey Mnatsakanov
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "glvbo.hpp"
#include <pangolin/gl/glvbo.h>

namespace py_pangolin {

  void bind_glvbo(pybind11::module &m){

    m.def("MakeTriangleStripIboForVbo", (pangolin::GlBuffer (*)(int, int))&pangolin::MakeTriangleStripIboForVbo, pybind11::arg("w"), pybind11::arg("h"), pybind11::return_value_policy::move);

    m.def("RenderVbo", &pangolin::RenderVbo, pybind11::arg("vbo"), pybind11::arg("mode")=GL_POINTS);
    m.def("RenderVboCbo", &pangolin::RenderVboCbo, pybind11::arg("vbo"), pybind11::arg("cbo"), pybind11::arg("draw_color")=true, pybind11::arg("mode")=GL_POINTS);
    m.def("RenderVboIbo", &pangolin::RenderVboIbo, pybind11::arg("vbo"), pybind11::arg("ibo"), pybind11::arg("draw_mesh")=true, pybind11::arg("mode")=GL_TRIANGLE_STRIP);
    m.def("RenderVboIboCbo", &pangolin::RenderVboIboCbo, pybind11::arg("vbo"), pybind11::arg("ibo"), pybind11::arg("cbo"), pybind11::arg("draw_mesh")=true, pybind11::arg("mode")=GL_POINTS, pybind11::arg("mode")=GL_TRIANGLE_STRIP);
    m.def("RenderVboIboNbo", &pangolin::RenderVboIboNbo, pybind11::arg("vbo"), pybind11::arg("ibo"), pybind11::arg("nbo"), pybind11::arg("draw_mesh")=true, pybind11::arg("draw_normals")=true, pybind11::arg("mode")=GL_TRIANGLE_STRIP);
    m.def("RenderVboIboCboNbo", &pangolin::RenderVboIboCboNbo, pybind11::arg("vbo"), pybind11::arg("ibo"), pybind11::arg("cbo"), pybind11::arg("nbo"), pybind11::arg("draw_mesh")=true, pybind11::arg("draw_color")=true, pybind11::arg("draw_normals")=true, pybind11::arg("mode")=GL_TRIANGLE_STRIP);

  }


}  // py_pangolin
