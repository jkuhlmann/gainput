#! /usr/bin/env python
# encoding: utf-8

VERSION='1'
APPNAME='gainput'

top = '.'
out = 'build'

# Contains some code from https://github.com/tom5760/xmp3/blob/master/wscript (Copyright (c) 2012 Tom Wambold, MIT license).

import sys
import os.path
import platform
import shutil

def options(opt):
	opt.load('compiler_cxx')
	opt.load('eclipse')
	opt.load('msvs')
	cross = opt.add_option_group('Cross Compiling')
	cross.add_option('--cross-android', action='store_const', dest='cross',
		const='android', default=None,
		help='Cross compile for Android')
	cross.add_option('--cross-android-ndk', action='store',
		default=os.getenv('ANDROID_NDK'),
		help='Android NDK directory.')
	cross.add_option('--cross-android-level', action='store', type=int,
		default=9, help='Android platform level to use.')
	cross.add_option('--cross-android-arch', action='store', default='armv6',
		help='Android architecture to use (armv6, armv7).')

def configure(cnf):
	cnf.setenv('debug')
	cnf.define('GAINPUT_LIB_BUILD', 1)
	cnf.define('DEBUG', 1)
	cnf.env.CXXFLAGS += []
	
	cnf.env.cross = 'none'
	if cnf.options.cross == 'android':
		cnf.env.cross = cnf.options.cross
		cnf.env.android_ndk = cnf.options.cross_android_ndk
		cnf.env.android_level = cnf.options.cross_android_level
		cnf.env.android_arch = cnf.options.cross_android_arch
	
		cnf.define('ANDROID', 1)

		if cnf.options.cross_android_arch in ('armv6', 'armv7'):
			cnf.env.CXXFLAGS += ['-fpic', '-ffunction-sections',
					'-funwind-tables', '-fstack-protector', '-D__ARM_ARCH_5__',
					'-D__ARM_ARCH_5T__', '-D__ARM_ARCH_5E__',
					'-D__ARM_ARCH_5TE__']

			cnf.env.CXXFLAGS += ['-mthumb', '-Os', '-fomit-frame-pointer',
				'-fno-strict-aliasing', '-finline-limit=64']

			if cnf.options.cross_android_arch == 'armv7':
				cnf.env.CXXFLAGS += ['-march=armv7-a', '-mfloat-abi=softfp', '-mfpu=vfp']
				cnf.env.LINKFLAGS += ['-Wl,--fix-cortex-a8']
			elif cnf.options.cross_android_arch == 'armv6':
				cnf.env.CXXFLAGS += ['-march=armv5te', '-mtune=xscale', '-msoft-float']

			arch = 'arm'
		else:
			arch = cnf.options.cross_android_arch

		path = os.path.join(cnf.options.cross_android_ndk, 'toolchains',
				'arm-linux-androideabi-4.6', 'prebuilt',
				'linux-x86', 'bin')
		cnf.find_program('arm-linux-androideabi-g++', var='CXX', path_list=[path])

		cnf.env.android_sysroot = os.path.join( cnf.options.cross_android_ndk, 'platforms',
				'android-{0}'.format( cnf.options.cross_android_level),
				'arch-{0}'.format(arch))
		cnf.env.cross_android_ndk = cnf.options.cross_android_ndk

		cnf.env.CXXFLAGS += ['--sysroot=' + cnf.env.android_sysroot]
		cnf.env.LINKFLAGS += ['--sysroot=' + cnf.env.android_sysroot]
		cnf.env.INCLUDES += [os.path.join(cnf.env.android_sysroot, 'usr/include')]
		cnf.env.INCLUDES += [os.path.join(cnf.options.cross_android_ndk, 'sources/cxx-stl/stlport/stlport/')]
		cnf.env.INCLUDES += [os.path.join(cnf.options.cross_android_ndk, 'sources/android/native_app_glue/')]
	
	cnf.load('compiler_cxx')

	if cnf.options.cross == 'android':
		cnf.env.CXXFLAGS += ['-Wall', '-g', '-fno-rtti', '-fno-exceptions']
	elif sys.platform.startswith('linux'):
		cnf.env.CXXFLAGS += ['-Wall', '-g', '-fno-rtti', '-fno-exceptions']
		cnf.check(compiler='cxx',
		          lib='X11',
	        	  mandatory=True, 
		          uselib_store='LIBX11')
		cnf.check(compiler='cxx',
	        	  lib='GL',
		          mandatory=True, 
		          uselib_store='LIBGL')
		cnf.check(compiler='cxx',
	        	  lib='rt',
		          mandatory=True, 
		          uselib_store='LIBRT')
	else:
		cnf.check_libs_msvc('kernel32 user32 gdi32')
		try:
			cnf.check(compiler='cxx',
				  lib='Xinput9_1_0',
			          mandatory=True, 
			          uselib_store='XINPUT')
		except:
			cnf.check(compiler='cxx',
				  lib='Xinput',
			          mandatory=True, 
			          uselib_store='XINPUT')
		cnf.check(compiler='cxx',
			  lib='Ws2_32',
		          mandatory=True, 
		          uselib_store='WINSOCK')

	cnf.setenv('release', env=cnf.env.derive())
	cnf.load('compiler_cxx')
	cnf.undefine('DEBUG')
	cnf.define('NDEBUG', 1)
	cnf.env.CXXFLAGS = ['-O2']
	if sys.platform.startswith('linux'):
		cnf.env.CXXFLAGS += ['-fno-rtti', '-fno-exceptions', '-s']

	cnf.setenv('dev', env=cnf.env.derive())
	cnf.load('compiler_cxx')
	cnf.define('GAINPUT_DEV', 1)
	cnf.env.CXXFLAGS = ['-O2']
	if sys.platform.startswith('linux'):
		cnf.env.CXXFLAGS += ['-Wall', '-g', '-fno-rtti', '-fno-exceptions']

def build(bld):
	lib_sources = bld.path.ant_glob('lib/source/gainput/**/*.cpp')
	usetheselib = ''
	if (sys.platform.startswith('win')):
		usetheselib = 'KERNEL32 USER32 GDI32 XINPUT WINSOCK'
	bld.shlib(features='cxx cxxshlib',
		source=lib_sources, 
		includes='lib/include/', 
		use=usetheselib,
		target='gainput')
	bld.stlib(features='cxx cxxstlib',
		source=lib_sources, 
		includes='lib/include/', 
		use=usetheselib,
		target='gainputstatic')
	bld.recurse('samples/basic/')
	bld.recurse('samples/dynamic/')
	bld.recurse('samples/gesture/')
	bld.recurse('samples/listener/')

from waflib.Build import BuildContext, CleanContext, \
        InstallContext, UninstallContext

contexts = (BuildContext, CleanContext, InstallContext, UninstallContext)
for x in 'debug dev release'.split():
	for y in contexts:
		name = y.__name__.replace('Context','').lower()
		class tmp(y):
			cmd = name + '_' + x
			variant = x
	for y in contexts:
		class tmp(y):
			variant = 'release'

