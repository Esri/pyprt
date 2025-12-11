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
import tempfile
import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'tests', 'data', filename)


def test_correct_execution():
    rpk = asset_file('extrusion_rule.rpk')
    attrs_1 = {}
    attrs_2 = {'minBuildingHeight': 30.0}
    attrs_3 = {'text': 'hello'}

    shape_geometry_1 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])
    shape_geometry_2 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, -10, -10, 0, -10, -10, 0, 0, -5, 0, -5])
    shape_geometry_3 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, -10, 10, 0, -10, 10, 0, 0, -5, 0, -5])
    m = pyprt.ModelGenerator(
        [shape_geometry_1, shape_geometry_2, shape_geometry_3])
    model = m.generate_model([attrs_1, attrs_2, attrs_3], rpk, 'com.esri.pyprt.PyEncoder',
                             {'emitReport': False, 'emitGeometry': True})
    assert len(model) == 3


def test_one_dict_for_all():
    rpk = asset_file('extrusion_rule.rpk')
    attrs = {}
    shape_geometry_1 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])
    shape_geometry_2 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, -10, -10, 0, -10, -10, 0, 0, -5, 0, -5])
    shape_geometry_3 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, -10, 10, 0, -10, 10, 0, 0, -5, 0, -5])
    m = pyprt.ModelGenerator(
        [shape_geometry_1, shape_geometry_2, shape_geometry_3])
    model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder',
                             {'emitReport': False, 'emitGeometry': True})
    assert len(model) == 3


def test_wrong_number_of_dict():
    rpk = asset_file('extrusion_rule.rpk')
    attrs_1 = {}
    attrs_2 = {'minBuildingHeight': 30.0}
    shape_geometry_1 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])
    shape_geometry_2 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, -10, -10, 0, -10, -10, 0, 0, -5, 0, -5])
    shape_geometry_3 = pyprt.InitialShape(
        [0, 0, 0, 0, 0, -10, 10, 0, -10, 10, 0, 0, -5, 0, -5])
    m = pyprt.ModelGenerator(
        [shape_geometry_1, shape_geometry_2, shape_geometry_3])
    model = m.generate_model([attrs_1, attrs_2], rpk, 'com.esri.pyprt.PyEncoder',
                             {'emitReport': False, 'emitGeometry': True})
    assert len(model) == 0


def test_one_dict_per_initial_shape_type():
    rpk = asset_file('extrusion_rule.rpk')
    attrs_1 = {}
    attrs_2 = {'minBuildingHeight': 30.0}
    shape_geometry_1 = pyprt.InitialShape(
        [-7.666, 0.0, -0.203, -7.666, 0.0, 44.051, 32.557, 0.0, 44.051, 32.557, 0.0, -0.203])
    shape_geometry_2 = pyprt.InitialShape(
        asset_file('building_parcel.obj'))
    m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2])
    model = m.generate_model(
        [attrs_1, attrs_2], rpk, 'com.esri.pyprt.PyEncoder', {})
    assert model[0].get_report()['Min Height.0_avg'] != model[1].get_report()['Min Height.0_avg']


def test_array_attributes():
    with tempfile.TemporaryDirectory() as output_path:
        rpk = asset_file('arrayAttrs.rpk')
        attrs = {'ruleFile': 'bin/arrayAttrs.cgb',
                 'startRule': 'Default$Init',
                 'arrayAttrFloat': [0.0, 1.0, 2.0],
                 'arrayAttrBool': [False, False, True],
                 'arrayAttrString': ['foo', 'bar', 'baz']}
        initial_shape = pyprt.InitialShape(
            [-7.666, 0.0, -0.203, -7.666, 0.0, 44.051, 32.557, 0.0, 44.051, 32.557, 0.0, -0.203])
        m = pyprt.ModelGenerator([initial_shape])
        m.generate_model([attrs], rpk, 'com.esri.prt.codecs.OBJEncoder', {'outputPath': output_path})

        expected_file = os.path.join(output_path, 'CGAPrint.txt')
        expected_content = ("arrayAttrFloat = (3)[0,1,2]\n"
                            "arrayAttrBool = (3)[false,false,true]\n"
                            "arrayAttrString = (3)[foo,bar,baz]\n")

        assert os.path.exists(expected_file)
        with open(expected_file, 'r') as cga_print_file:
            cga_print = cga_print_file.read()
            assert cga_print == expected_content


def convert_asset_to_slpk(asset_name):
    initial_shape_asset = asset_file(asset_name)

    rpk = asset_file('identity.rpk')
    rpk_attributes = {'ruleFile': 'bin/identity.cgb', 'startRule': 'Default$Init'}
    encoder_id = 'com.esri.prt.codecs.I3SEncoder'

    initial_shape = pyprt.InitialShape(initial_shape_asset)
    model_generator = pyprt.ModelGenerator([initial_shape])

    with tempfile.TemporaryDirectory() as output_path:
        encoder_options = {
            'outputPath': output_path,
            'sceneType': "Local",
            'sceneWkid': "3857"
        }
        model_generator.generate_model([rpk_attributes], rpk, encoder_id, encoder_options)

        expected_file = os.path.join(output_path, 'base_name.slpk')
        assert os.path.exists(expected_file)


def test_initial_shape_glb():
    convert_asset_to_slpk("Candler Building_0.glb")


def test_initial_shape_fbx():
    convert_asset_to_slpk("EmbeddedTextures.fbx")


def convert_asset_to_fbx(asset_name, max_dir_recursion_depth):
    initial_shape_asset = asset_file(asset_name)

    rpk = asset_file('identity.rpk')
    rpk_attributes = {'ruleFile': 'bin/identity.cgb', 'startRule': 'Default$Init'}
    encoder_id = 'com.esri.prt.codecs.FBXEncoder'

    initial_shape = pyprt.InitialShape(initial_shape_asset, max_dir_recursion_depth)
    model_generator = pyprt.ModelGenerator([initial_shape])

    with tempfile.TemporaryDirectory() as output_path:
        encoder_options = {
            'outputPath': output_path
        }
        model_generator.generate_model([rpk_attributes], rpk, encoder_id, encoder_options)

        expected_asset = os.path.join(output_path, 'base_name_0.fbx')
        expected_texture = os.path.join(output_path, 'Bonnland_102.png')
        assert os.path.exists(expected_asset) and os.path.exists(expected_texture)


def test_initial_shape_obj():
    convert_asset_to_fbx("OBJ-Bonnland/Bonnland_102.obj", 0)


def test_initial_shape_obj_with_child_dirs():
    convert_asset_to_fbx("OBJ-Bonnland/Bonnland_102.obj", 2)
