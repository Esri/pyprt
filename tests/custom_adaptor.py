import unittest
import pyprt
import io


class MyAdaptor(pyprt.StreamAdaptor):
	def create_stream(self, uri):
		py_stream = io.StringIO("Hello\nWorld\nThis is PyBind11!\n")
		return pyprt.Stream(py_stream)

	def destroy_stream(self, stream):
		pass


class MyAdaptorFactory(pyprt.StreamAdaptorFactory):
	def __init__(self):
		super().__init__()

	def create_adaptor(self):
		return MyAdaptor()

	def get_merrit(self):
		return 5.0

	def get_id(self):
		return "MyAdaptorFactory"

	def get_name(self):
		return "MyAdaptorFactory"

	def get_description(self):
		return "MyAdaptorFactory"

	def can_handle_uri(self, uri):
		return True


class AdaptorTest(unittest.TestCase):
	def test_custom_adaptor(self):
		pyprt.register_adaptor_factory(MyAdaptorFactory())
