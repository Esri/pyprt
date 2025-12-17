# Copyright (c) 2012-2024 Esri R&D Center Zurich

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#   http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# A copy of the license is available in the repository's LICENSE file.

import os
import pyprt
import pytest

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'tests', 'data', filename)


def asset_output_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'output', filename)


def test_verticesnumber_candler():
    rpk = asset_file('candler.rpk')
    attrs = {}
    shape_geo_from_obj = pyprt.InitialShape(
        asset_file('candler_footprint.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': False, 'emitGeometry': True})
    assert len(model[0].get_vertices()) == 97072 * 3


def test_facesnumber_candler():
    rpk = asset_file('candler.rpk')
    attrs = {}
    shape_geo_from_obj = pyprt.InitialShape(
        asset_file('candler_footprint.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': False, 'emitGeometry': True})
    assert len(model[0].get_faces()) == 47208


def test_facesnumber_triangulation():
    rpk = asset_file('extrusion_rule.rpk')
    attrs = {}
    shape_geo_from_obj = pyprt.InitialShape(
        asset_file('candler_footprint.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder',
                             {'emitReport': False, 'emitGeometry': True, 'triangulate': True})
    assert len(model[0].get_faces()) == 28


def test_report_green():
    rpk = asset_file('envelope2002.rpk')
    attrs = {'report_but_not_display_green': True, 'seed': 666}
    shape_geo_from_obj = pyprt.InitialShape(asset_file('building_parcel.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': True, 'emitGeometry': False})
    ground_truth_dict = {'Floor area_n': 20.0, 'Greenery Area_n': 574.0, 'Number of trees_n': 114.0,
                         'green area_n': 460.0, 'total report for optimisation_n': 20.0, 'Floor area_sum': 10099.37,
                         'Floor area_avg': 504.97, 'Greenery Area_sum': 2123.40, 'Greenery Area_avg': 3.70,
                         'Number of trees_sum': 114.0, 'Number of trees_avg': 1.0, 'green area_sum': 1553.40,
                         'green area_avg': 3.38, 'total report for optimisation_sum': 807.95,
                         'total report for optimisation_avg': 40.40, 'Floor area_min': 294.37, 'Floor area_max': 874.87,
                         'Greenery Area_min': 0.49, 'Greenery Area_max': 408.27, 'Number of trees_min': 1.0,
                         'Number of trees_max': 1.0, 'green area_min': 0.49, 'green area_max': 408.27,
                         'total report for optimisation_min': 23.55, 'total report for optimisation_max': 69.99}
    rep = model[0].get_report()
    rep_round = {x: round(z, 2) for x, z in rep.items()}
    assert rep_round == ground_truth_dict


def test_noreport():
    rpk = asset_file('extrusion_rule.rpk')
    attrs = {}
    shape_geo = pyprt.InitialShape([-10.0, 0.0, 5.0, -5.0, 0.0, 6.0, 20.0, 0.0, 5.0, 15.0, 0.0, 3.0])
    m = pyprt.ModelGenerator([shape_geo])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': False})
    assert model[0].get_report() == {}


def test_nogeometry():
    rpk = asset_file('extrusion_rule.rpk')
    attrs = {}
    shape_geo = pyprt.InitialShape([-10.0, 0.0, 5.0, -5.0, 0.0, 6.0, 20.0, 0.0, 5.0, 15.0, 0.0, 3.0])
    m = pyprt.ModelGenerator([shape_geo])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitGeometry': False})
    assert model[0].get_vertices() == []


def test_buildingheight():
    rpk = asset_file('extrusion_rule.rpk')
    attrs = {'minBuildingHeight': 23.0,
             'maxBuildingHeight': 23.0}
    shape_geo = pyprt.InitialShape([-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
    m = pyprt.ModelGenerator([shape_geo])
    models = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
    assert len(models) == 1
    model = models[0]
    vertices = model.get_vertices()
    for y in vertices[1:-1:3]:
        assert y == pytest.approx(0.0, 1e-3) or abs(y) == pytest.approx(23, 1e-3)


def test_faces_data():
    rpk = asset_file('candler.rpk')
    attrs = {}
    shape_geo_from_obj = pyprt.InitialShape(asset_file('candler_footprint.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': False})
    face_sum = sum(model[0].get_faces())
    assert face_sum == len(model[0].get_indices())


def test_path_geometry_initshapes():
    rpk = asset_file('extrusion_rule.rpk')
    attrs = {}
    shape_geo = pyprt.InitialShape([-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
    shape_geo_from_obj = pyprt.InitialShape(asset_file('building_parcel.obj'))
    m1 = pyprt.ModelGenerator([shape_geo])
    m2 = pyprt.ModelGenerator([shape_geo_from_obj])
    m3 = pyprt.ModelGenerator([shape_geo, shape_geo_from_obj])
    model1 = m1.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
    model2 = m2.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
    model3 = m3.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
    assert model1[0].get_report() == model3[0].get_report()
    assert model2[0].get_report() == model3[1].get_report()
    assert model1[0].get_vertices() == model3[0].get_vertices()
    assert model2[0].get_vertices() == model3[1].get_vertices()


def test_initial_shape_with_hole():
    rpk = asset_file('FacesHolesVerticesrule.rpk')
    attrs = {}
    shape_with_hole = pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0, 2, 0, 2, 8, 0, 8, 2, 0, 8],
                                         [0, 1, 2, 3, 4, 5, 6], [4, 3], [[0, 1]])

    encoder_options = {'outputPath': os.path.dirname(asset_output_file(''))}
    os.makedirs(encoder_options['outputPath'], exist_ok=True)

    m = pyprt.ModelGenerator([shape_with_hole])
    m.generate_model([attrs], rpk, 'com.esri.prt.codecs.OBJEncoder', encoder_options)

    expected_file = os.path.join(encoder_options['outputPath'], 'CGAPrint.txt')
    expected_content = ("14\n"
                        "9\n"
                        "2\n")

    assert os.path.exists(expected_file)
    with open(expected_file, 'r') as cga_print_file:
        cga_print = cga_print_file.read()
        assert cga_print == expected_content


def test_cga_prints_green():
    rpk = asset_file('envelope2002.rpk')
    attrs = {'report_but_not_display_green': True, 'seed': 2}
    shape_geo_from_obj = pyprt.InitialShape(asset_file('building_parcel.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': True, 'emitGeometry': False})

    assert model[0].get_cga_prints() == str(attrs['seed']) + "\n"


def test_cga_errors_holes():
    rpk = asset_file('FacesHolesVerticesrule.rpk')
    attrs = {}
    shape_with_hole_with_error = pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0, 2, 0, 2, 8, 0, 8, 2, 0, 8],
                                                    [0, 1, 2, 3, 4, 5, 6], [4, 3], [[0, 1, 1]])
    m = pyprt.ModelGenerator([shape_with_hole_with_error])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': True, 'emitGeometry': False})
    expected_error_count = 1 if pyprt.get_api_version()[0] > 2 else 0
    assert len(model[0].get_cga_errors()) == expected_error_count


def test_attributesvalue_fct():
    rpk = asset_file('extrusion_rule.rpk')
    attrs = {'maxBuildingHeight': 35.0}
    attrs2 = {}
    shape_geo_from_obj = pyprt.InitialShape(asset_file('building_parcel.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj, shape_geo_from_obj])
    model = m.generate_model([attrs, attrs2], rpk, 'com.esri.pyprt.PyEncoder', {})
    assert model[0].get_attributes() == {'maxBuildingHeight': 35.0, 'OBJECTID': 0.0, 'minBuildingHeight': 10.0,
                                         'buildingColor': '#FF00FF', 'text': 'salut'}
    assert model[1].get_attributes() == {'OBJECTID': 0.0, 'minBuildingHeight': 10.0, 'buildingColor': '#FF00FF',
                                         'text': 'salut', 'maxBuildingHeight': 30.0}


def test_attributesvalue_fct_arrays():
    rpk = asset_file('arrayAttrs.rpk')
    attrs = {'arrayAttrFloat': [0.0, 1.0, 2.0]}
    shape_geo_from_obj = pyprt.InitialShape(asset_file('building_parcel.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
    assert model[0].get_attributes() == {'arrayAttrFloat': [0.0, 1.0, 2.0], 'arrayAttrBool': [False],
                                         'arrayAttrString': ['uhm']}


def test_attributesvalue_fct_arrays2d():
    rpk = asset_file('arrayAttrs2d.rpk')
    attrs = {}
    shape_geo_from_obj = pyprt.InitialShape(asset_file('building_parcel.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
    assert model[0].get_attributes() == {'arrayAttrBool': [[False, True], [True, False]],
                                         'arrayAttrFloat': [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]],
                                         'arrayAttrString': [['first', 'row'],
                                                             ['second', 'row'],
                                                             ['third', 'row'],
                                                             ['fourth', 'row']]}


def test_dynamic_imports():
    if pyprt.get_api_version()[0] < 3:
        import pytest
        pytest.skip("test case only supported with PRT >= 3.0")
    rpk = asset_file("dynamic_imports.rpk")  # RPK created with CE 2023.0
    attrs = {}
    shape_geo_from_obj = pyprt.InitialShape(asset_file('quad0.obj'))
    m = pyprt.ModelGenerator([shape_geo_from_obj])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
    assert len(model) == 1
    assert model[0].get_report() == {'myHeight_n': 1.0, 'myHeight_sum': 10.0, 'myHeight_avg': 10.0,
                                     'myHeight_min': 10.0,
                                     'myHeight_max': 10.0}
