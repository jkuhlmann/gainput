#! /usr/bin/env python
# encoding: utf-8

VERSION='1'
APPNAME='gainput'

top = '.'
out = 'build'

import sys

def options(opt):
	opt.load('compiler_cxx')
	opt.load('eclipse')
	opt.load('msvs')

def configure(cnf):
	cnf.setenv('debug')
	cnf.load('compiler_cxx')
	cnf.define('GAINPUT_LIB_BUILD', None)
	cnf.env.CXXFLAGS = ['-g', '-Wall']
	if (sys.platform.startswith('linux')):
		cnf.check(compiler='cxx',
		          lib='X11',
	        	  mandatory=True, 
		          uselib_store='LIBX11')
		cnf.check(compiler='cxx',
	        	  lib='GL',
		          mandatory=True, 
		          uselib_store='LIBGL')
	else:
		cnf.check_libs_msvc('kernel32 user32 gdi32')
		cnf.check(compiler='cxx',
	        	  lib='Xinput',
		          mandatory=True, 
		          uselib_store='XINPUT')
	
	cnf.setenv('release', env=cnf.env.derive())
	cnf.load('compiler_cxx')
	cnf.env.CXXFLAGS = ['-O2']

def build(bld):
	lib_sources = bld.path.ant_glob('lib/source/gainput/*.cpp')
	usetheselib = ''
	if (sys.platform.startswith('win')):
		usetheselib = 'KERNEL32 USER32 GDI32 XINPUT'
	bld.shlib(features='cxx cxxshlib',
		source=lib_sources, 
		includes='lib/include/', 
		use=usetheselib,
		target='gainput')
	basic_sources = bld.path.ant_glob('samples/basic/*.cpp')
	usethese = 'gainput'
	if (sys.platform.startswith('win')):
		usethese = 'gainput KERNEL32 USER32 GDI32 XINPUT'
	bld.program(features='cxx cxxprogram',
		source=basic_sources, 
		includes='lib/include/', 
		target='basicsample',
		use = usethese,
		uselib=['LIBX11', 'LIBGL'])

from waflib.Build import BuildContext, CleanContext, \
        InstallContext, UninstallContext

for x in 'debug release'.split():
        for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
                name = y.__name__.replace('Context','').lower()
                class tmp(y):
                        cmd = name + '_' + x
                        variant = x

