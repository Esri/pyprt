import unittest
import os
import sys

SDK_PATH = os.path.join(os.getcwd(), "install", "bin")
sys.path.append(SDK_PATH)

import pyprt

class MainTest(unittest.TestCase):
    def test_print(self): # the test functions name has to start with "test_"
        self.assertEqual(pyprt.print_val(47), 47)

    def setUpModule(self):
        pyprt.initialize_prt(SDK_PATH)

    def tearDownModule(self):
        pyprt.shutdown_prt()

if __name__ == '__main__':
    unittest.main()
