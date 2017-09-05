import distutils.util
import os.path
import sys

mydir = os.path.dirname(os.path.abspath(__file__))
plat_specifier = 'lib.{0}-{1}'.format(distutils.util.get_platform(),
                                      sys.version[0:3])
sys.path.insert(0, os.path.join(mydir, 'build', plat_specifier))

from _ucomm import *
