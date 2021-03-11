import unittest

# noinspection PyUnresolvedReferences
from dotnet import *


# noinspection PyUnresolvedReferences
class TestImportMachinery(unittest.TestCase):

    def setUp(self):
        pass

    def test_from_imports(self):

        from System import Int32
        self.assertEqual(Int32.Parse("123"), 123)


if __name__ == '__main__':
    unittest.main()
