# Copyright (c) 2012-2022 Esri R&D Center Zurich

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#   https://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# A copy of the license is available in the repository's LICENSE file.

import os
import pyprt

pyprt.initialize_prt()

# Define the input geometry
shape_geometry = pyprt.InitialShape([0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])

# Setup ModelGenerator instance for input geometry
model_generator = pyprt.ModelGenerator([shape_geometry])

# Setup generation parameters
repo_path = os.path.dirname(os.path.realpath(__file__))
rpk = os.path.join(repo_path, 'tests/data/extrusion_rule.rpk')
shape_attributes = {'shapeName': 'myShape', 'seed': 555}
encoder = 'com.esri.pyprt.PyEncoder'
encoder_options = {'emitReport': True, 'emitGeometry': True}

# Generate the model
generated_models = model_generator.generate_model([shape_attributes], rpk, encoder, encoder_options)

# Access the result
for model in generated_models:
    index = model.get_initial_shape_index()
    cga_report = model.get_report()
    vertices = model.get_vertices()
    print(f"Model {index} has vertices: {vertices} and reports {cga_report}")

pyprt.shutdown_prt()
