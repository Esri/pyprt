import unittest
import os
import sys

SDK_PATH = os.path.join(os.getcwd(), "build", "lib.win-amd64-3.6", "PyPRT", "pyprt", "bin")
sys.path.append(SDK_PATH)

import pyprt


class MainTest(unittest.TestCase):
    def test_print(self): # the test functions name has to start with "test_"
        self.assertEqual(pyprt.print_val(47), 47)
