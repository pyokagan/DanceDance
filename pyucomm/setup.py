import glob
import os.path
try:
    from setuptools import setup, Extension
except ImportError:
    from distutils.core import setup
    from distutils.extension import Extension

sources = []
sources += glob.glob(os.path.join('..', 'libraries', 'ucomm', 'src', '*.c'))
sources += ['pyucomm.c']

ext = Extension(name='_ucomm',
                sources=sources)

setup(name='pyucomm',
      version='0.0.1',
      description='Python bindings for ucomm',
      ext_modules=[ext])
