project('', 'c', version: '')

#HAVE_STRING_H

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

sources = [
    'src/libteletone_detect.c',
    'src/libteletone_generate.c',
#    'src/libteletone_generate_util.c',
]

incdir = include_directories('include')

libteletone = static_library('libteletone', sources, dependencies : [m_dep], include_directories : incdir)

test('Detector', executable('detector', 'test/detect_sample.c', link_with: [libteletone], include_directories : incdir))
test('Generator', executable('generate', 'test/generate_sample.c', link_with: [libteletone], include_directories : incdir))
